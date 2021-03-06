#include <linux/device.h>  // for struct device, class
#include <linux/fs.h>      // for struct file_operations
#include <linux/init.h>    // for macro __init, __exit
#include <linux/kernel.h>  // macro, function for kernel
#include <linux/module.h>  // for loading module
#include <linux/mutex.h>   // for mutex sync
#include <linux/random.h>  // for function get_random_bytes()
#include <linux/uaccess.h> // for function copy_to_user()

MODULE_LICENSE("GPL");
MODULE_AUTHOR("18120587-18120627-18120628");
MODULE_DESCRIPTION("Simple character device to send user random number");

static DEFINE_MUTEX(rand_char_mutex); // only 1 program access at a time

#define DEVICE_NAME "random_number"
#define CLASS_NAME "random_number_class"

static int major_number; // device number
static struct class *random_number_class = NULL;
static struct device *random_number = NULL;

// Prototype
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);

static struct file_operations fops = {
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
};

static int __init random_number_init(void)
{
	printk(KERN_INFO "random number module init\n");

	// Cap phat major number
	major_number = register_chrdev(0, DEVICE_NAME, &fops);
	if (major_number < 0) {
		printk(KERN_ALERT
			"random number failed to register a major number\n");
		return major_number;
	}
	printk(KERN_INFO "random number registered a major number %d\n",
		major_number);

	// Dang ky device class
	random_number_class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(random_number_class)) {
		unregister_chrdev(major_number, DEVICE_NAME);
		printk(KERN_ALERT "random number failed to create a struct "
			"pointer class\n");
		return PTR_ERR(random_number_class); // return error for pointer
	}
	printk(KERN_INFO "random number created a struct pointer class\n");

	// Dang ky device driver
	random_number = device_create(random_number_class, NULL,
		MKDEV(major_number, 0), NULL, DEVICE_NAME);
	if (IS_ERR(random_number)) {
		class_destroy(random_number_class);
		unregister_chrdev(major_number, DEVICE_NAME);
		printk(KERN_ALERT "random number failed to create a struct "
			"pointer device\n");
		return PTR_ERR(random_number); // return error for pointer
	}
	printk(KERN_INFO "random number created a struct pointer device\n");

	// set up mutex
	mutex_init(&rand_char_mutex);

	return 0;
}

static void __exit random_number_exit(void)
{
	// cleanup mutex
	mutex_destroy(&rand_char_mutex);

	// cleanup device, class, unregister major number
	device_destroy(random_number_class, MKDEV(major_number, 0));
	class_unregister(random_number_class);
	class_destroy(random_number_class);
	unregister_chrdev(major_number, DEVICE_NAME);

	printk(KERN_INFO "random number module exit\n");
}

// call each time device is opened
static int dev_open(struct inode *inodep, struct file *filep)
{
	// check if another process is opening device
	if (!mutex_trylock(&rand_char_mutex)) {
		printk(KERN_ALERT "random number is opened by another process\n");
		return -EBUSY;
	}

	printk(KERN_INFO "random number is opened\n");
	return 0;
}

// call each time device is released
static int dev_release(struct inode *inodep, struct file *filep)
{
	// unlock mutex
	mutex_unlock(&rand_char_mutex);

	printk(KERN_INFO "random number is released\n");
	return 0;
}

// call each time device is read
// device send random number to user
static ssize_t dev_read(struct file *filep, char *buffer, size_t len,
	loff_t *offset)
{
	// random number in kernel space
	int rand_num;
	get_random_bytes(&rand_num, sizeof(rand_num));

	int status = 0;
	// copy_to_user return 0 - success
	// otherwise - fail
	status = copy_to_user(buffer, &rand_num, sizeof(rand_num));
	if (status == 0) {
		printk(KERN_INFO "random number sent %d\n", rand_num);
		return 0;
	}
	else {
		printk(KERN_INFO "random number failed to send %d\n",
			rand_num);
		return -EFAULT;
	}
}

module_init(random_number_init);
module_exit(random_number_exit);
