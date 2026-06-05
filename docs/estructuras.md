# Estructuras de Datos del Kernel — KMonitor Grupo 9

## Introducción

El módulo KMonitor interactúa directamente con las estructuras internas del
kernel Linux para obtener información de memoria y procesos. Este documento
explica cada estructura y función del kernel utilizada en el proyecto.

---

## 1. `task_struct` — La ficha de cada proceso

### ¿Qué es?
`task_struct` es la estructura más importante del kernel Linux. El kernel
crea una instancia de esta estructura por cada proceso que existe en el
sistema. Es como una **ficha de identificación** que contiene toda la
información de un proceso.

Está definida en: `include/linux/sched.h`

### ¿Para qué la usamos?
En KMonitor la usamos para leer los datos de cada proceso activo del sistema.

### Campos que utilizamos

| Campo | Tipo | Descripción | Ejemplo |
|---|---|---|---|
| `pid` | `pid_t` | Número único que identifica al proceso | 1, 2, 1024 |
| `comm[16]` | `char[]` | Nombre del programa ejecutable | "bash", "chrome" |
| `__state` | `long` | Estado actual del proceso | 0 = Running |

### Ejemplo de uso en el código
```c
struct task_struct *proceso;

for_each_process(proceso) {
    seq_printf(archivo, "PID: %d  Nombre: %s  Estado: %s\n",
        proceso->pid,
        proceso->comm,
        obtener_estado(proceso->__state));
}
```

---

## 2. `for_each_process` — El iterador de procesos

### ¿Qué es?
`for_each_process` es una **macro** del kernel que funciona como un
`for` especial para recorrer todos los procesos del sistema.

Internamente recorre una **lista enlazada circular** que el kernel
mantiene con todos los procesos activos. El punto de inicio es
`init_task`, que es el proceso número 0 — el padre de todos los procesos.

### ¿Por qué es importante?
Sin esta macro, acceder a la lista de procesos sería muy complejo y
peligroso. La macro encapsula toda esa complejidad de forma segura.

### Uso en el código
```c
struct task_struct *proceso;

for_each_process(proceso) {
    /* aquí 'proceso' apunta a cada proceso del sistema uno por uno */
    printk(KERN_INFO "PID: %d\n", proceso->pid);
}
```

### Analogía
Imagina que el kernel tiene una lista de asistencia con todos los
procesos. `for_each_process` es como pasar la lista fila por fila,
leyendo el nombre de cada proceso.

---

## 3. `sysinfo` y `si_meminfo()` — Los datos de memoria

### ¿Qué es `sysinfo`?
`sysinfo` es una **estructura** que el kernel usa para almacenar
información sobre el estado de la memoria del sistema. Es como
un contenedor que guarda todos los datos de RAM.

Está definida en: `include/linux/sysinfo.h`

### Campos que utilizamos

| Campo | Descripción |
|---|---|
| `totalram` | Total de páginas de RAM disponibles |
| `freeram` | Páginas de RAM actualmente libres |
| `mem_unit` | Tamaño en bytes de cada página (generalmente 4096) |

### ¿Qué es `si_meminfo()`?
Es la función del kernel que llena la estructura `sysinfo` con los
datos actuales de memoria. Se llama así porque viene de
**S**ystem **I**nformation **Mem**ory **Info**.

### ¿Por qué convertimos a MB?
El kernel no maneja la memoria en megabytes sino en **páginas**.
Una página = 4096 bytes = 4 KB.

Para convertir a MB usamos esta operación:
```c
total_mb = (info.totalram * info.mem_unit) >> 20;
```

El `>> 20` es un desplazamiento de bits que divide entre 1,048,576
(que es 1 MB en bytes). Es más eficiente que hacer una división normal.

### Ejemplo de uso en el código
```c
struct sysinfo info;

si_meminfo(&info);   /* llenar la estructura con datos actuales */

unsigned long total_mb   = (info.totalram * info.mem_unit) >> 20;
unsigned long libre_mb   = (info.freeram  * info.mem_unit) >> 20;
unsigned long usada_mb   = total_mb - libre_mb;
unsigned long porcentaje = (usada_mb * 100) / total_mb;
```

---

## 4. `rcu_read_lock()` y `rcu_read_unlock()` — El candado de seguridad

### ¿Qué es RCU?
RCU significa **Read-Copy-Update**. Es un mecanismo de sincronización
del kernel que permite leer datos de forma segura mientras otros
procesos los pueden estar modificando al mismo tiempo.

### ¿Por qué es necesario?
La lista de procesos del sistema cambia constantemente — cada
milisegundo pueden crearse o destruirse procesos. Si intentamos
leer la lista sin protección, podría ocurrir esto:

```
1. Empezamos a leer el proceso X
2. El kernel destruye el proceso X mientras lo leemos
3. Intentamos leer más datos de X → KERNEL PANIC
```

Con RCU esto no ocurre porque el kernel garantiza que ningún
proceso desaparecerá mientras tengamos el candado activo.

### ¿Cómo funciona en el código?
```c
rcu_read_lock();          /* adquirir el candado — "no borres nada" */

for_each_process(proceso) {
    /* aquí es seguro leer la lista de procesos */
    seq_printf(...);
}

rcu_read_unlock();        /* liberar el candado — "ya puedes borrar" */
```

### Analogía
Es como cuando un bibliotecario pone un letrero que dice
"Préstamo en proceso" — nadie puede retirar ese libro del
sistema mientras lo está procesando.

### Regla importante
Nunca se debe hacer algo que pueda "dormir" o tardar mucho
entre `rcu_read_lock()` y `rcu_read_unlock()`. El candado
debe adquirirse y liberarse lo más rápido posible.

---

## 5. Estados de los procesos

### ¿Qué es el estado de un proceso?
Todo proceso en Linux siempre se encuentra en uno de varios
estados posibles. El campo `__state` de `task_struct` almacena
un número que representa ese estado.

### Tabla de estados

| Constante | Valor | Significado | En KMonitor |
|---|---|---|---|
| `TASK_RUNNING` | 0 | El proceso está ejecutándose o listo para ejecutarse | "Running" |
| `TASK_INTERRUPTIBLE` | 1 | Dormido, puede ser despertado por señales del sistema | "Sleeping" |
| `TASK_UNINTERRUPTIBLE` | 2 | Dormido esperando I/O, NO puede ser interrumpido | "Sleeping" |
| `EXIT_ZOMBIE` | 16 | Terminó pero su proceso padre no ha leído su código de salida | "Zombie" |
| `EXIT_DEAD` | 32 | En proceso de eliminación por el kernel | "Zombie" |
| Cualquier otro | - | Estado no reconocido | "Otro" |

### ¿Cómo lo manejamos en el código?
```c
static const char *obtener_estado(long estado)
{
    if (estado == TASK_RUNNING)
        return "Running";
    else if (estado == TASK_INTERRUPTIBLE ||
             estado == TASK_UNINTERRUPTIBLE)
        return "Sleeping";
    else if (estado == EXIT_ZOMBIE ||
             estado == EXIT_DEAD)
        return "Zombie";
    else
        return "Otro";
}
```

### Ciclo de vida de un proceso
```
                    [programa inicia]
                          │
                          ▼
                     TASK_RUNNING
                    (listo o corriendo)
                          │
              ┌───────────┴───────────┐
              │                       │
              ▼                       ▼
   TASK_INTERRUPTIBLE      TASK_UNINTERRUPTIBLE
   (espera señal o evento)  (espera I/O crítico)
              │                       │
              └───────────┬───────────┘
                          │
                          ▼
                     TASK_RUNNING
                   (cuando se despierta)
                          │
                          ▼
                     EXIT_ZOMBIE
                  (terminó, espera al padre)
                          │
                          ▼
                      EXIT_DEAD
                   (kernel lo elimina)
```

---

## 6. `seq_file` — La API de escritura en /proc

### ¿Qué es?
`seq_file` es una API del kernel diseñada específicamente para
escribir datos en archivos `/proc` de forma segura y eficiente.

### ¿Por qué no usamos printf normal?
En espacio de kernel NO existe `printf()`. La función equivalente
es `printk()`, pero solo escribe en el ring buffer del kernel
(que se ve con `dmesg`), no en archivos `/proc`.

Para escribir en `/proc` se usa `seq_printf()` que funciona
exactamente igual que `printf()` pero escribe al buffer del
`seq_file`.

### ¿Por qué seq_file y no copy_to_user directamente?
`seq_file` maneja automáticamente la **paginación** cuando los
datos son muy grandes. En KMonitor tenemos más de 200 procesos
— sin seq_file podría ocurrir un desbordamiento de buffer.

### Uso en el código
```c
static int mostrar_kmonitor(struct seq_file *archivo, void *v)
{
    /* escribir en el archivo /proc exactamente como printf */
    seq_printf(archivo, "Total RAM: %lu MB\n", total_mb);
    seq_printf(archivo, "Libre:     %lu MB\n", libre_mb);
    seq_printf(archivo, "%-8d %-12s %s\n", pid, estado, nombre);
    return 0;
}
```

---

## Resumen de estructuras y funciones

| Estructura/Función | Archivo de origen | Para qué la usamos |
|---|---|---|
| `task_struct` | `linux/sched.h` | Leer PID, nombre y estado de cada proceso |
| `for_each_process()` | `linux/sched/signal.h` | Iterar sobre todos los procesos |
| `sysinfo` | `linux/sysinfo.h` | Almacenar datos de memoria RAM |
| `si_meminfo()` | `linux/mm.h` | Obtener métricas actuales de RAM |
| `rcu_read_lock/unlock()` | `linux/rcupdate.h` | Proteger la lista de procesos |
| `seq_printf()` | `linux/seq_file.h` | Escribir datos en /proc |
| `proc_create()` | `linux/proc_fs.h` | Crear el archivo /proc/kmonitor_grupo9 |
| `remove_proc_entry()` | `linux/proc_fs.h` | Eliminar el archivo /proc al descargar |

---
