#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>

char* DEVICE_NAME = "nulo";

static struct cdev cdev;

static dev_t major;

static struct class *mi_class;

static ssize_t nulo_read(struct file *filp, char __user *data, size_t s, loff_t *off) {
	printk(KERN_ALERT "Buen itento, lectura descartada :)\n");
	return 0;
}

static ssize_t nulo_write(struct file *filp, const char __user *data, size_t s, loff_t *off) {
	printk(KERN_ALERT "Buen itento, escritura descartada :)\n");
	return s;
}

static struct file_operations nulo_operaciones = {
	.owner = THIS_MODULE,
	.read = nulo_read,
	.write = nulo_write,
};

static int __init nulo_init(void) {
	printk(KERN_ALERT "Modulo nulo iniciando...\n");

	cdev_init(&cdev, &nulo_operaciones);
	alloc_chrdev_region(&major, 0, 1, DEVICE_NAME);
	cdev_add(&cdev, major, 1);
	mi_class = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(mi_class, NULL, major, NULL, DEVICE_NAME);
	
	printk(KERN_ALERT "Modulo nulo iniciado.\n");
	return 0;
}	

static void __exit nulo_exit(void) {
	printk(KERN_ALERT "Modulo nulo finalizando...\n");

	unregister_chrdev_region(major, 1);
	cdev_del(&cdev);
	device_destroy(mi_class, major);
	class_destroy(mi_class);
	
	printk(KERN_ALERT "Modulo nulo finalizado.\n");
}

module_init(nulo_init);
module_exit(nulo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("La banda de SO");
MODULE_DESCRIPTION("Una suerte de '/dev/null'");