#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/configfs.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <linux/console_struct.h>
#include <linux/vt_kern.h>

#include <linux/printk.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/fs.h>
#include <linux/string.h>

MODULE_DESCRIPTION("Blink the lights on your keyboard");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bogdan Novitskiy");

#define LOG_MODULE_NAME "the_blinker: "
#define SYS_FNAME       "practice3.sys"
#define RESTORE_LEDS    0xFF
#define SYS_PERMS       0660
#define LEDS_MASK_MAX   7u

static unsigned int blink_delay_ms = 500;
module_param(blink_delay_ms, uint, 0644);
MODULE_PARM_DESC(blink_delay_ms, "Delay between LED toggles, in milliseconds");

static struct kobject *leds_kobject;
static unsigned int leds_status;
static unsigned int blink_mask;
static struct tty_driver *tty_driver;
static struct timer_list timer;

static void blink(struct timer_list *ptr);
static ssize_t sys_read(struct kobject *kobject, struct kobj_attribute *attr, char *buf);
static ssize_t sys_write(struct kobject *kobject, struct kobj_attribute *attr, const char *buf, size_t count);
static int  __init sys_init(void);
static void __exit sys_exit(void);

static struct kobj_attribute leds_attribute = __ATTR(
        leds,
        SYS_PERMS,
        sys_read,
        sys_write
);

static int __init sys_init(void)
{
        struct vc_data *vc_data;
        struct tty_struct *tty_struct;
        int error;

        printk(KERN_INFO LOG_MODULE_NAME "loading\n");

        vc_data = vc_cons[fg_console].d;
        if (!vc_data)
                return -EINVAL;

        tty_struct = vc_data->port.tty;
        if (!tty_struct)
                return -EINVAL;

        tty_driver = tty_struct->driver;
        if (!tty_driver)
                return -EINVAL;

        leds_kobject = kobject_create_and_add(SYS_FNAME, kernel_kobj);
        if (!leds_kobject)
                return -ENOMEM;

        error = sysfs_create_file(leds_kobject, &leds_attribute.attr);
        if (error) {
                printk(KERN_ERR LOG_MODULE_NAME "failed to create /sys/kernel/%s (%d)\n",
                       SYS_FNAME, error);
                kobject_put(leds_kobject);
                return error;
        }

        printk(KERN_INFO LOG_MODULE_NAME "tty driver - %s: %x %x\n",
               tty_driver->name, tty_driver->major, tty_driver->minor_start);

        timer_setup(&timer, blink, 0);
        timer.expires = jiffies + msecs_to_jiffies(blink_delay_ms);
        add_timer(&timer);

        printk(KERN_INFO LOG_MODULE_NAME "initialized\n");
        return 0;
}

static void __exit sys_exit(void)
{
        timer_delete_sync(&timer);

        if (tty_driver && tty_driver->ops && tty_driver->ops->ioctl)
                tty_driver->ops->ioctl(vc_cons[fg_console].d->port.tty, KDSETLED, RESTORE_LEDS);

        kobject_put(leds_kobject);
        printk(KERN_INFO LOG_MODULE_NAME "uninitialized\n");
}

static ssize_t sys_read(struct kobject *kobject, struct kobj_attribute *attr, char *buf)
{
        return sprintf(buf, "%u\n", blink_mask);
}

static ssize_t sys_write(struct kobject *kobject, struct kobj_attribute *attr, const char *buf, size_t count)
{
        unsigned int tmp = 0;

        if (kstrtouint(buf, 0, &tmp) != 0)
                return -EINVAL;
        if (tmp > LEDS_MASK_MAX)
                return -EINVAL;

        blink_mask = tmp;
        printk(KERN_INFO LOG_MODULE_NAME "mask set to %u\n", blink_mask);
        return (ssize_t)count;
}

static void blink(struct timer_list *ptr)
{
        if (!tty_driver || !tty_driver->ops || !tty_driver->ops->ioctl) {
                printk(KERN_WARNING LOG_MODULE_NAME "no tty\n");
                mod_timer(&timer, jiffies + msecs_to_jiffies(blink_delay_ms));
        }

        if (blink_mask == 0) {
                tty_driver->ops->ioctl(vc_cons[fg_console].d->port.tty, KDSETLED, RESTORE_LEDS);
                leds_status = 0;
        } else {
                leds_status = (leds_status == 0) ? blink_mask : 0;
                tty_driver->ops->ioctl(vc_cons[fg_console].d->port.tty, KDSETLED, leds_status);
        }

        mod_timer(&timer, jiffies + msecs_to_jiffies(blink_delay_ms));
}

module_init(sys_init);
module_exit(sys_exit);