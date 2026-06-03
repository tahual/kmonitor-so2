#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Grupo SO2");
MODULE_DESCRIPTION("KMonitor");

static int __init kmonitor_init(void)
{
    printk(KERN_INFO "KMonitor: Modulo cargado\n");
    return 0;
}

static void __exit kmonitor_exit(void)
{
    printk(KERN_INFO "KMonitor: Modulo descargado\n");
}

module_init(kmonitor_init);
module_exit(kmonitor_exit);
