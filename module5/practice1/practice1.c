#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("Fishfish");
MODULE_AUTHOR("Novitskiy Bogdan");
MODULE_DESCRIPTION("Custom HELLO WORLD module");

static int __init hello_init(void)
{
    printk(KERN_INFO "HELLO WORLD!\n");
    return 0;
}

static void __exit hello_cleanup(void)
{
    printk(KERN_INFO "CLEANING UP module.\n");
}

module_init(hello_init);
module_exit(hello_cleanup);