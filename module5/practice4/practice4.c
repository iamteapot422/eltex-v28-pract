#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/version.h>

#define DEVICE_NAME "mychardev"
#define CLASS_NAME "mychardev_class"
#define BUF_SIZE 1024

static int major_number;
static struct class *dev_class;
static struct cdev my_cdev;
static dev_t dev_num;

static char *kbuf;
static size_t data_size;
static DEFINE_MUTEX(dev_mutex);

static int dev_open(struct inode *inode, struct file *file)
{
    if (!mutex_trylock(&dev_mutex))
        return -EBUSY;
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    mutex_unlock(&dev_mutex);
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    ssize_t to_copy;

    if (*off >= data_size)
        return 0;

    to_copy = min((size_t)(data_size - *off), len);

    if (copy_to_user(buf, kbuf + *off, to_copy))
        return -EFAULT;

    *off += to_copy;
    return to_copy;
}

static ssize_t dev_write(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    size_t to_copy = min(len, (size_t)(BUF_SIZE - 1));

    if (copy_from_user(kbuf, buf, to_copy))
        return -EFAULT;

    kbuf[to_copy] = '\0';
    data_size = to_copy;

    return to_copy;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

static int __init mychardev_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0)
        return ret;

    major_number = MAJOR(dev_num);

    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    ret = cdev_add(&my_cdev, dev_num, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    dev_class = class_create(CLASS_NAME);
    if (IS_ERR(dev_class)) {
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(dev_class);
    }

    if (IS_ERR(device_create(dev_class, NULL, dev_num, NULL, DEVICE_NAME))) {
        class_destroy(dev_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_num, 1);
        return -ENODEV;
    }

    kbuf = kmalloc(BUF_SIZE, GFP_KERNEL);
    if (!kbuf) {
        device_destroy(dev_class, dev_num);
        class_destroy(dev_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_num, 1);
        return -ENOMEM;
    }

    data_size = 0;
    mutex_init(&dev_mutex);

    pr_info("mychardev: major=%d, /dev/%s created\n", major_number, DEVICE_NAME);
    return 0;
}

static void __exit mychardev_exit(void)
{
    kfree(kbuf);
    device_destroy(dev_class, dev_num);
    class_destroy(dev_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
    pr_info("mychardev: unloaded\n");
}

module_init(mychardev_init);
module_exit(mychardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Simple chardev for userspace <-> kernel exchange");