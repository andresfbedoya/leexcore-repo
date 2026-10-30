# TermuxBridge 0.1.0

Plugin de integración para usar una instalación existente de Termux desde LeexCore.

## Comandos

```text
termux          abre Termux
termux <cmd>    envía el comando a Termux mediante RUN_COMMAND
```

El plugin no incluye Termux, no incluye su bootstrap y no redistribuye archivos de Termux. Termux debe estar instalado por separado. La ejecución depende de que la versión instalada permita la interfaz pública `com.termux.RUN_COMMAND`.

Licencia MIT.
