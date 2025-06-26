#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define mem_size	1024

dev_t dev = 0;
static struct class *dev_class;
static struct cdev my_cdev;

uint8_t *kernel_buffer;
size_t buffer_offset = 0;

static int __init my_char_module_init(void);
static void __exit my_char_module_exit(void);
static int my_open(struct inode *inode, struct file *file);
static int my_release(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *filep, char __user *buf, size_t len, loff_t *off);
static ssize_t my_write(struct file *filep, const char *buf, size_t len, loff_t *off);
static loff_t my_llseek(struct file *filep, loff_t off, int whence);

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_release,
	.read = my_read,
	.write = my_write,
	.llseek = my_llseek,
};

static int my_open(struct inode *inode, struct file *file) {
	pr_info("Device file opened\n");
	return 0;
}

static int my_release(struct inode *inode, struct file *file) {
	pr_info("Device file closed\n");
	return 0;
}

static ssize_t my_read(struct file *filep, char __user *buf, size_t len, loff_t *off) {
	size_t bytes_to_read;
	
	pr_info("Before read: offset=%lld, buffer_offset=%zu\n", *off, buffer_offset);
	
	if (*off >= buffer_offset) {
		pr_info("Reached EOF: offset=%lld, buffer_offset=%zu\n", *off, buffer_offset);
		return 0;
	}
	
	bytes_to_read = min(len, (size_t)(buffer_offset - *off));
	
	if (bytes_to_read == 0) {
		pr_info("No bytes to read\n");
		return 0;
	}
	
	if (copy_to_user(buf, kernel_buffer + *off, bytes_to_read)) {
		pr_err("Data Read: Failed\n");
		return -EFAULT;
	}
	
	pr_info("Data Read: Done (%zu bytes)\n", bytes_to_read);
	pr_info("Data read: %.64s\n", kernel_buffer + *off);
	
	*off += bytes_to_read;
	
	return bytes_to_read;
}

static ssize_t my_write(struct file *filep, const char *buf, size_t len, loff_t *off) {
	size_t bytes_to_write;
	
	pr_info("Before write: offset=%lld, buffer_offset=%zu\n", *off, buffer_offset);
	pr_info("User buffer: %.64s\n", buf);
	
	if (*off >= mem_size) {
		return 0;
	}
	
	bytes_to_write = min(len, (size_t)(mem_size - *off));
	
	if (bytes_to_write == 0) {
		return 0;
	}
	
	if (copy_from_user(kernel_buffer + *off, buf, bytes_to_write)) {
		pr_err("Data Write: Failed!\n");
		return -EFAULT;
	}
	
	pr_info("Data Write: Done (%zu bytes)\n", bytes_to_write);
	pr_info("Data written: %.64s\n", kernel_buffer + *off);
	
	*off += bytes_to_write;
	
	if (*off > buffer_offset) {
		buffer_offset = *off;
	}
	
	return bytes_to_write;
}

static loff_t my_llseek(struct file *filep, loff_t off, int whence) {
    loff_t newpos;
    
    switch(whence) {
        case SEEK_SET: newpos = off; break;
        case SEEK_CUR: newpos = filep->f_pos + off; break;
        case SEEK_END: newpos = buffer_offset + off; break;
        default: return -EINVAL;
    }
    
    if (newpos < 0 || newpos > mem_size)
        return -EINVAL;
        
    filep->f_pos = newpos;
    return newpos;
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
	
	/* Creating physical memory */
	if ((kernel_buffer = (uint8_t *)kmalloc(mem_size, GFP_KERNEL)) == 0) {
		pr_err("Cannot allocate memory in kernel\n");
		goto r_device;
	}
	
	strcpy(kernel_buffer, "Hello World");
	buffer_offset = strlen("Hello World") + 1;

	pr_info("---------------------------\n");
	pr_info("Loading my char module\n");
	return 0;

	r_device:
	class_destroy(dev_class);

	r_class:
	unregister_chrdev_region(dev, 1);
	return -1;
}

static void __exit my_char_module_exit(void) {
	kfree(kernel_buffer);
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Unloading my char module\n");
	pr_info("---------------------------\n");
}

module_init(my_char_module_init);
module_exit(my_char_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mamun");
MODULE_DESCRIPTION("A simple char device driver module");
