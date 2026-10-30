#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/stat.h>

static int memfd_copy(const char *path, const char *name) {
    int fd = (int)syscall(__NR_memfd_create, name, 0);
    if (fd < 0) return -1;
    int in = open(path, O_RDONLY);
    if (in < 0) { close(fd); return -1; }
    char b[8192]; ssize_t n;
    while ((n = read(in, b, sizeof(b))) > 0) {
        ssize_t at = 0;
        while (at < n) { ssize_t w = write(fd, b + at, (size_t)(n - at)); if (w < 0) { close(in); close(fd); return -1; } at += w; }
    }
    close(in);
    return n < 0 ? (close(fd), -1) : fd;
}

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
    char root[PATH_MAX], loader[PATH_MAX], busybox[PATH_MAX], home[PATH_MAX], path[PATH_MAX], libs[PATH_MAX], busybox_mem[64], loader_mem[64];
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
    setenv("TERM", "xterm-256color", 1);
    setenv("SECONDHOME_ROOT", root, 1);
    if (chdir(home) != 0) { perror("home"); return 68; }
    int loader_fd = memfd_copy(loader, "secondhome-musl-loader");
    int busybox_fd = memfd_copy(busybox, "secondhome-busybox");
    if (loader_fd < 0 || busybox_fd < 0) { fprintf(stderr, "No pude cargar Alpine en memoria: %s\n", strerror(errno)); return 70; }
    snprintf(loader_mem, sizeof(loader_mem), "/proc/self/fd/%d", loader_fd);
    snprintf(busybox_mem, sizeof(busybox_mem), "/proc/self/fd/%d", busybox_fd);
    char **child = calloc((size_t)argc + 2, sizeof(char *));
    if (!child) return 69;
    child[0] = loader_mem;
    child[1] = busybox_mem;
    if (argc == 2) { child[2] = "sh"; child[3] = "-i"; }
    else { for (int i = 2; i < argc; i++) child[i] = argv[i]; }
    execv(loader_mem, child);
    fprintf(stderr, "No pude iniciar SecondHome: %s\n", strerror(errno));
    return 127;
}
