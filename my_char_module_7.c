#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>

#define WR_VALUE	_IOW('a', 'a', int32_t *)
#define RD_VALUE	_IOR('a', 'b', int32_t *)

dev_t dev = 0;
static struct class *dev_class;
static struct cdev my_cdev;
int32_t value = 0;

static int __init my_char_module_init(void);
static void __exit my_char_module_exit(void);
static int my_open(struct inode *inode, struct file *file);
static int my_release(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *filep, char __user *buf, size_t len, loff_t *off);
static ssize_t my_write(struct file *filep, const char *buf, size_t len, loff_t *off);
static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_release,
	.read = my_read,
	.write = my_write,
	.unlocked_ioctl = my_ioctl,
};

static int my_open(struct inode *inode, struct file *file) {
	pr_info("Open function called\n");
	return 0;
}

static int my_release(struct inode *inode, struct file *file) {
	pr_info("Release function called\n");
	return 0;
}

static ssize_t my_read(struct file *filep, char __user *buf, size_t len, loff_t *off) {
	pr_info("Read function called\n");
	return 0;
}

static ssize_t my_write(struct file *filep, const char *buf, size_t len, loff_t *off) {
	pr_info("Write function called\n");
	return len;
}

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	switch (cmd) {
		case WR_VALUE:
			if (copy_from_user(&value, (int32_t *)arg, sizeof(value))) {
				pr_err("Error writing data in my_ioctl\n");
			}
			pr_info("Value: %d\n", value);
			break;
		case RD_VALUE:
			if (copy_to_user((int32_t *)arg, &value, sizeof(value))) {
				pr_err("Error reading data in my_ioctl\n");
			}
			break;
		default:
			pr_info("my_ioctl: default\n");
	}
	
	return 0;
}

static int __init my_char_module_init(void) {

	/* Allocating major number  */
	if (alloc_chrdev_region(&dev, 0, 1, "My_char_dev") < 0) {
		pr_err("Cannot allocate major number for My_char_dev\n");
		return -1;
	}
	pr_info("Major = %d, Minor = %d\n", MAJOR(dev), MINOR(dev));
	
	/* Creating cdev structure */
	cdev_init(&my_cdev, &fops);
	
	/* Adding char device to the system */
	if (cdev_add(&my_cdev, dev, 1) < 0) {
		pr_err("Cannot add the device to the system");
		goto r_class;
	}
	
	/* Creating struct class */
	dev_class = class_create("My_char_class");
	if (IS_ERR(dev_class)) {
		pr_err("Cannot create struct class for the device My_char_dev\n");
		goto r_class;
	}

	/* Creating a device */
	if (IS_ERR(device_create(dev_class, NULL, dev, NULL, "My_char_dev"))) {
		pr_err("Cannot create the device My_char_dev\n");
		goto r_device;
	}

	pr_info("Loading my char module\n");
	return 0;

	r_device:
	class_destroy(dev_class);

	r_class:
	unregister_chrdev_region(dev, 1);
	return -1;
}

static void __exit my_char_module_exit(void) {
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Unloading my char module\n");
}

module_init(my_char_module_init);
module_exit(my_char_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mamun");
MODULE_DESCRIPTION("A simple char device driver module");
