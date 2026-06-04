#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mm.h>
#include <linux/sysinfo.h>
#include <linux/sched/signal.h>

#define PROC_NAME "kmonitor_grupo9"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Grupo 9 - KMonitor");
MODULE_DESCRIPTION("Modulo de kernel para mostrar informacion de memoria y procesos en /proc");
MODULE_VERSION("2.0");

static const char *obtener_estado(long estado)
{
    if (estado == TASK_RUNNING)
        return "Running";
    else if (estado == TASK_INTERRUPTIBLE || estado == TASK_UNINTERRUPTIBLE)
        return "Sleeping";
    else if (estado == EXIT_ZOMBIE || estado == EXIT_DEAD)
        return "Zombie";
    else
        return "Otro";
}

static int mostrar_kmonitor(struct seq_file *archivo, void *v)
{
    struct sysinfo info;
    struct task_struct *proceso;

    unsigned long total_mb;
    unsigned long libre_mb;
    unsigned long usada_mb;
    unsigned long porcentaje_uso;

    si_meminfo(&info);

    total_mb = (info.totalram * info.mem_unit) / (1024 * 1024);
    libre_mb = (info.freeram * info.mem_unit) / (1024 * 1024);
    usada_mb = total_mb - libre_mb;

    if (total_mb > 0)
        porcentaje_uso = (usada_mb * 100) / total_mb;
    else
        porcentaje_uso = 0;

    seq_printf(archivo, "===== KMONITOR - GRUPO 9 =====\n\n");

    seq_printf(archivo, "===== MEMORIA =====\n");
    seq_printf(archivo, "Memoria total: %lu MB\n", total_mb);
    seq_printf(archivo, "Memoria libre: %lu MB\n", libre_mb);
    seq_printf(archivo, "Memoria usada: %lu MB\n", usada_mb);
    seq_printf(archivo, "Porcentaje de uso: %lu%%\n\n", porcentaje_uso);

    seq_printf(archivo, "===== PROCESOS ACTIVOS =====\n");
    seq_printf(archivo, "%-10s %-25s %-15s\n", "PID", "Nombre", "Estado");

    for_each_process(proceso) {
        seq_printf(archivo, "%-10d %-25s %-15s\n",
                   proceso->pid,
                   proceso->comm,
                   obtener_estado(proceso->__state));
    }

    return 0;
}

static int abrir_archivo(struct inode *inode, struct file *file)
{
    return single_open(file, mostrar_kmonitor, NULL);
}

static const struct proc_ops operaciones_proc = {
    .proc_open = abrir_archivo,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init iniciar_modulo(void)
{
    proc_create(PROC_NAME, 0, NULL, &operaciones_proc);
    printk(KERN_INFO "KMonitor Grupo 9 cargado correctamente.\n");
    return 0;
}

static void __exit salir_modulo(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "KMonitor Grupo 9 descargado correctamente.\n");
}

module_init(iniciar_modulo);
module_exit(salir_modulo);
