#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>

dev_t dev = 0;

static int __init my_char_module_init(void) {
	if (alloc_chrdev_region(&dev, 0, 1, "My_char_dev") < 0) {
		printk(KERN_INFO "Cannot allocate major number for My_char_dev\n");
		return -1;
	}
	printk(KERN_INFO "Major = %d, Minor = %d\n", MAJOR(dev), MINOR(dev));
	printk(KERN_INFO "Loading my char module\n");
	return 0;
}

static void __exit my_char_module_exit(void) {
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Unloading my char module\n");
}

module_init(my_char_module_init);
module_exit(my_char_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mamun");
MODULE_DESCRIPTION("A simple char device driver module");
