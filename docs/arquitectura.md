# Arquitectura de la Solución — KMonitor Grupo 9

## ¿Qué es KMonitor?

KMonitor es un **Módulo Cargable del Kernel Linux (LKM)** desarrollado en lenguaje C.
Su propósito es recolectar métricas críticas del sistema directamente desde las
estructuras internas del kernel y exponerlas al espacio de usuario a través de un
archivo virtual en el directorio `/proc`.

Cuando el usuario ejecuta `cat /proc/kmonitor_grupo9`, el módulo responde mostrando:
- Memoria RAM total, libre, usada y porcentaje de uso
- Lista completa de procesos activos con PID, nombre y estado

---

## Componentes del módulo

El proyecto está compuesto por los siguientes archivos:

| Archivo | Ubicación | Descripción |
|---|---|---|
| `kmonitor_memoria.c` | `src/` | Código fuente principal del módulo |
| `Makefile` | `src/` | Instrucciones de compilación para el kernel |
| `README.md` | raíz | Instrucciones de uso del proyecto |
| `docs/arquitectura.md` | `docs/` | Este documento |
| `docs/estructuras.md` | `docs/` | Explicación de estructuras del kernel |

---

## Descripción de cada función del módulo

### `obtener_estado(long estado)`
Función auxiliar que convierte el estado numérico de un proceso en texto legible.

| Valor numérico | Texto que devuelve |
|---|---|
| TASK_RUNNING | "Running" |
| TASK_INTERRUPTIBLE | "Sleeping" |
| TASK_UNINTERRUPTIBLE | "Sleeping" |
| EXIT_ZOMBIE / EXIT_DEAD | "Zombie" |
| Cualquier otro | "Otro" |

---

### `mostrar_kmonitor(struct seq_file *archivo, void *v)`
Es la función principal del módulo. Se ejecuta automáticamente cada vez que
el usuario hace `cat /proc/kmonitor_grupo9`. Realiza dos tareas:

**Tarea 1 — Métricas de memoria:**
1. Llama a `si_meminfo(&info)` para obtener datos de RAM del kernel
2. Convierte los valores de páginas a megabytes usando desplazamiento de bits (`>> 20`)
3. Calcula el porcentaje de uso
4. Escribe los resultados con `seq_printf()`

**Tarea 2 — Lista de procesos:**
1. Adquiere el candado RCU con `rcu_read_lock()`
2. Recorre todos los procesos con `for_each_process(proceso)`
3. Por cada proceso escribe: PID, nombre y estado
4. Libera el candado con `rcu_read_unlock()`


### `abrir_archivo(struct inode *inode, struct file *file)`
Función de apertura del archivo `/proc`. Conecta el archivo virtual con
la función `mostrar_kmonitor()` usando `single_open()`.


### `iniciar_modulo(void)`
Se ejecuta automáticamente cuando se carga el módulo con `sudo insmod`.

Acciones que realiza:
1. Crea el archivo `/proc/kmonitor_grupo9` con permisos de solo lectura (0444)
2. Imprime un mensaje de confirmación en el ring buffer del kernel


### `salir_modulo(void)`
Se ejecuta automáticamente cuando se descarga el módulo con `sudo rmmod`.

Acciones que realiza:
1. Elimina el archivo `/proc/kmonitor_grupo9`
2. Imprime un mensaje de confirmación en el ring buffer del kernel


## Diagrama de flujo

```
┌─────────────────────────────────────────────────┐
│              CICLO DE VIDA DEL MÓDULO            │
└─────────────────────────────────────────────────┘

[sudo insmod kmonitor_memoria.ko]
          │
          ▼
   iniciar_modulo()
          │
          ├── proc_create("kmonitor_grupo9")
          │         │
          │         └── Crea /proc/kmonitor_grupo9 
          │
          └── printk("Módulo cargado")


[cat /proc/kmonitor_grupo9]
          │
          ▼
   abrir_archivo()
          │
          ▼
   mostrar_kmonitor()
          │
          ├── si_meminfo(&info)
          │         │
          │         ├── Calcula total_mb
          │         ├── Calcula libre_mb
          │         ├── Calcula usada_mb
          │         └── Calcula porcentaje_uso
          │
          ├── seq_printf() ── Escribe métricas de RAM
          │
          ├── rcu_read_lock() ── Protege lista de procesos
          │
          ├── for_each_process(proceso)
          │         │
          │         └── seq_printf(PID, estado, nombre)
          │               [repite por cada proceso]
          │
          └── rcu_read_unlock() ── Libera protección


[sudo rmmod kmonitor_memoria]
          │
          ▼
   salir_modulo()
          │
          ├── remove_proc_entry("kmonitor_grupo9")
          │         │
          │         └── Elimina /proc/kmonitor_grupo9 
          │
          └── printk("Módulo descargado")
```

## Flujo de interacción entre archivos

```
Makefile
   │
   └── compila ──► kmonitor_memoria.c
                         │
                         ├── #include <linux/module.h>
                         ├── #include <linux/proc_fs.h>
                         ├── #include <linux/seq_file.h>
                         ├── #include <linux/mm.h>
                         └── #include <linux/sched/signal.h>
                         │
                         ▼
                  kmonitor_memoria.ko
                         │
                    sudo insmod
                         │
                         ▼
               /proc/kmonitor_grupo9
                         │
                      cat (leer)
                         │
                         ▼
              Métricas de RAM + Procesos
```


## Tecnologías utilizadas

Tecnologias           Versión                Uso
Linux Kernel     6.17.0-23-generic    Entorno de ejecució
GCC              13.3.0               Compilador de C
make             GNU Make             Sistema de compilación 
Git              2.43.0               Control de versiones 
GitHub                                Repositorio remoto 
Ubuntu           24.04 LTS            Sistema operativo de desarrollo 

## Equipo de desarrollo — Grupo 9

Integrantes            Trabajo Realizado
David:           Desarrollo del módulo principal y demostración
Derick:          Desarrollo del código kernel
Dany:            Documentación y coordinación
Marlon:          Documentación de estructuras y video




