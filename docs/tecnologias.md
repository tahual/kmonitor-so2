# Herramientas, Librerías y Tecnologías Utilizadas

## Librerías del Kernel Linux

### linux/module.h
Es la cabecera principal y obligatoria para cualquier módulo del kernel Linux. Proporciona las macros y funciones esenciales que permiten registrar el módulo en el kernel, definir sus metadatos como licencia, autor y descripción, y establecer las funciones de inicialización y salida mediante module_init() y module_exit(). Sin esta cabecera no es posible compilar ningún módulo del kernel.

### linux/kernel.h
Proporciona funciones fundamentales del núcleo del sistema operativo. En KMonitor se utiliza principalmente para acceder a printk(), que es el equivalente de printf() en el espacio de kernel y permite escribir mensajes en el ring buffer del sistema, consultables mediante el comando dmesg.

### linux/proc_fs.h
Proporciona la API necesaria para interactuar con el sistema de archivos virtual /proc. En KMonitor se utilizan específicamente las funciones proc_create() para crear el archivo /proc/kmonitor_grupo9 al cargar el módulo, y remove_proc_entry() para eliminarlo al descargarlo.

### linux/seq_file.h
Proporciona la API seq_file diseñada específicamente para escribir datos en archivos /proc de forma segura y eficiente. En KMonitor se utiliza seq_printf() para escribir la información de memoria y procesos. Esta API maneja automáticamente la paginación cuando la cantidad de datos es grande, evitando desbordamientos de buffer.

### linux/mm.h
Proporciona funciones relacionadas con la gestión de memoria del kernel. En KMonitor se utiliza para acceder a si_meminfo(), función que llena una estructura sysinfo con los datos actuales de la memoria RAM del sistema obtenidos directamente desde el kernel.

### linux/sysinfo.h
Define la estructura sysinfo, que actúa como contenedor de los datos de memoria del sistema. KMonitor utiliza los campos totalram, freeram y mem_unit de esta estructura para calcular la memoria total, libre, usada y el porcentaje de uso en megabytes.

### linux/sched/signal.h
Proporciona las definiciones necesarias para trabajar con procesos del sistema. En KMonitor se utiliza para acceder a la estructura task_struct y a la macro for_each_process(), que permite recorrer todos los procesos activos del sistema de forma segura.

## Herramientas de Desarrollo

### GCC 13.3.0
GCC, cuyas siglas corresponden a GNU Compiler Collection, es el compilador utilizado para transformar el código fuente en C del módulo KMonitor en código objeto compatible con el kernel Linux. Su invocación no se realizó directamente sino a través de GNU Make, que lo llama internamente al ejecutar el comando make dentro de la carpeta src.

### GNU Make
GNU Make es la herramienta de automatización de compilación utilizada en KMonitor. Mediante el archivo Makefile incluido en la carpeta src, Make sabe exactamente cómo compilar el módulo utilizando los headers del kernel instalados en el sistema. Al ejecutar el comando make, esta herramienta genera el archivo kmonitor_memoria.ko que es el módulo listo para ser cargado al kernel.

### Git 2.43.0
Git es el sistema de control de versiones utilizado por el equipo para gestionar el desarrollo colaborativo del proyecto. Permitió que cada integrante trabajara en su propia rama de forma independiente sin afectar el trabajo de los demás. Se utilizó para crear ramas, registrar commits, fusionar cambios y mantener el historial completo del desarrollo.

### GitHub
GitHub es la plataforma en línea utilizada como repositorio remoto del proyecto. A través de ella el equipo centralizó el código fuente, gestionó las ramas de desarrollo y coordinó la integración final mediante Pull Requests. El repositorio del proyecto se encuentra disponible públicamente en https://github.com/tahual/kmonitor-so2.git.

### Ubuntu 24.04 LTS
Ubuntu 24.04 LTS fue el sistema operativo utilizado para el desarrollo y las pruebas del módulo. Al ser una distribución Linux, proporciona el entorno necesario para compilar y cargar módulos del kernel. Fue ejecutado dentro de una máquina virtual VirtualBox para garantizar la seguridad del sistema anfitrión durante las pruebas.

### VirtualBox
VirtualBox es el software de virtualización utilizado para ejecutar Ubuntu de forma aislada del sistema operativo principal. Dado que KMonitor opera en espacio de kernel, cualquier error en el módulo puede provocar un Kernel Panic. El uso de VirtualBox garantizó que dichos errores no afectaran el sistema operativo anfitrión, permitiendo realizar las pruebas de forma segura.
