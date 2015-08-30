#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");

static int __init kws_init(void)
{
    printk(KERN_ALERT "Hello, world\n");
    return 0;
}

static void __exit kws_exit(void)
{
    printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(kws_init);
module_exit(kws_exit);