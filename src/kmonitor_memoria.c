#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mm.h>
#include <linux/sysinfo.h>

#define PROC_NAME "kmonitor_grupoX"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Grupo KMonitor - Parte memoria: David Moscoso");
MODULE_DESCRIPTION("Modulo de kernel para mostrar informacion de memoria en /proc");
MODULE_VERSION("1.0");

static int mostrar_memoria(struct seq_file *archivo, void *v)
{
    struct sysinfo info;
    unsigned long total_mb;
    unsigned long libre_mb;
    unsigned long usada_mb;
    unsigned long porcentaje_uso;

    si_meminfo(&info);

    total_mb = (info.totalram * info.mem_unit) / (1024 * 1024);
    libre_mb = (info.freeram * info.mem_unit) / (1024 * 1024);
    usada_mb = total_mb - libre_mb;

    if (total_mb > 0) {
        porcentaje_uso = (usada_mb * 100) / total_mb;
    } else {
        porcentaje_uso = 0;
    }

    seq_printf(archivo, "===== KMONITOR - MEMORIA =====\n");
    seq_printf(archivo, "Memoria total: %lu MB\n", total_mb);
    seq_printf(archivo, "Memoria libre: %lu MB\n", libre_mb);
    seq_printf(archivo, "Memoria usada: %lu MB\n", usada_mb);
    seq_printf(archivo, "Porcentaje de uso: %lu%%\n", porcentaje_uso);

    return 0;
}

static int abrir_archivo(struct inode *inode, struct file *file)
{
    return single_open(file, mostrar_memoria, NULL);
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
    printk(KERN_INFO "KMonitor memoria cargado correctamente.\n");
    return 0;
}

static void __exit salir_modulo(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "KMonitor memoria descargado correctamente.\n");
}

module_init(iniciar_modulo);
module_exit(salir_modulo);
