# SecondHome 0.1.0

Primer cargador experimental de LeexDroid para arrancar un userland Alpine dentro de LeexCore.

## Qué hace

SecondHome ejecuta el loader musl y BusyBox del rootfs Alpine usando el kernel Linux de Android. No es una máquina virtual, no usa Termux y esta versión todavía no implementa traducción global de rutas, chroot ni PRoot.

## Uso previsto

El gestor de plugins de LeexCore instalará el rootfs y ejecutará el binario correspondiente a la arquitectura:

```text
secondhome /ruta/al/rootfs
```

Sin un rootfs extraído, el cargador no puede iniciar.

## Estado

Alpha experimental. La primera plataforma objetivo es `arm64-v8a`. Los binarios se incluyen para cuatro ABIs, pero el rootfs Alpine incluido es ARM64.

## Licencia

MIT. El rootfs Alpine mantiene sus propios avisos y licencias.
