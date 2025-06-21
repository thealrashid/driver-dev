// Passing Arguments to module

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

int int_param = 10;
int arr_param[4];
char *charp_param;
int cb_param;

module_param(int_param, int, S_IRUSR | S_IWUSR);
module_param(charp_param, charp, S_IRUSR | S_IWUSR);
module_param_array(arr_param, int, NULL, S_IRUSR | S_IWUSR);

/*----------------------- module_param_cb ------------------------------ */

int notify_param(const char *, const struct kernel_param *);

int notify_param(const char *val, const struct kernel_param *kp) {
	int res = param_set_int(val, kp);

	if (res == 0) {
		printk(KERN_INFO "Callback function called\n");
		printk(KERN_INFO "New value of cb_param = %d\n", cb_param);
		return 0;
	}

	return -1;
}

const struct kernel_param_ops my_param_ops = {
	.set = &notify_param,
	.get = &param_get_int
};

module_param_cb(cb_param, &my_param_ops, &cb_param, S_IRUGO|S_IWUSR);

/*-----------------------------------------------------------------------*/

static int __init my_char_module_init(void) {
	printk(KERN_INFO "Loading my char module\n");
	printk(KERN_INFO "int_param = %d\n", int_param);
	printk(KERN_INFO "cb_param = %d\n", cb_param);
	printk(KERN_INFO "charp_param = %s\n", charp_param);
	
	int n = sizeof(arr_param) / sizeof(arr_param[0]);
	for (int i = 0; i < n; i++) {
		printk(KERN_INFO "arr_param[%d] = %d\n", i, arr_param[i]);
	}

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
