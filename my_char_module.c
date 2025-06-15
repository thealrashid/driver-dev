#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init my_char_module_init(void) {
	printk(KERN_INFO "Loading my char module\n");
	return 0;
}

static void __exit my_char_module_exit(void) {
	printk(KERN_INFO "Unloading my char module\n");
}

module_init(my_char_module_init);
module_exit(my_char_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mamun");
MODULE_DESCRIPTION("A simple char device driver module");
