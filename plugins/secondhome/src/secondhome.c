#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>

static const char *loader_name(void) {
#if defined(__aarch64__)
    return "lib/ld-musl-aarch64.so.1";
#elif defined(__arm__)
    return "lib/ld-musl-armhf.so.1";
#elif defined(__x86_64__)
    return "lib/ld-musl-x86_64.so.1";
#elif defined(__i386__)
    return "lib/ld-musl-i386.so.1";
#else
    return NULL;
#endif
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: secondhome ROOTFS [comando] [argumentos...]\n");
        return 64;
    }
    const char *loader_rel = loader_name();
    if (!loader_rel) { fprintf(stderr, "Arquitectura no soportada\n"); return 65; }
    char root[PATH_MAX], loader[PATH_MAX], busybox[PATH_MAX], home[PATH_MAX], path[PATH_MAX], libs[PATH_MAX];
    if (!realpath(argv[1], root)) { perror("ROOTFS"); return 66; }
    snprintf(loader, sizeof(loader), "%s/%s", root, loader_rel);
    snprintf(busybox, sizeof(busybox), "%s/bin/busybox", root);
    snprintf(home, sizeof(home), "%s/home/leex", root);
    if (access(loader, X_OK) != 0 || access(busybox, X_OK) != 0) {
        fprintf(stderr, "Rootfs SecondHome incompleto: %s\n", root); return 67;
    }
    mkdir(home, 0755);
    setenv("HOME", home, 1);
    snprintf(path, sizeof(path), "%s/bin:%s/sbin:%s/usr/bin:%s/usr/sbin", root, root, root, root);
    snprintf(libs, sizeof(libs), "%s/lib:%s/usr/lib", root, root);
    setenv("PATH", path, 1);
    setenv("LD_LIBRARY_PATH", libs, 1);
    setenv("PS1", "~ $ ", 1);
    setenv("TERM", "xterm-256color", 1);
    setenv("SECONDHOME_ROOT", root, 1);
    if (chdir(home) != 0) { perror("home"); return 68; }
    char **child = calloc((size_t)argc + 2, sizeof(char *));
    if (!child) return 69;
    child[0] = loader;
    child[1] = busybox;
    if (argc == 2) { child[2] = "sh"; child[3] = "-i"; }
    else { for (int i = 2; i < argc; i++) child[i] = argv[i]; }
    execv(loader, child);
    fprintf(stderr, "No pude iniciar SecondHome: %s\n", strerror(errno));
    return 127;
}
