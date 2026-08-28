#include <linux/kernel.h>
#include <linux/module.h>
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
#include <linux/tty.h>

MODULE_DESCRIPTION("Управление светодиодами клавиатуры через sysfs");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("RickLater0");

#define LOG_MODULE_NAME "leds blink module: "
#define SYS_FNAME       "mleds.sys"
#define RESTORE_LEDS    0xFF
#define SYS_FRIGHTS     0660
#define BLINK_DELAY     (HZ)

static struct kobject *leds_kobject;
static unsigned int leds_status;
static unsigned int blink_mask;
static struct tty_driver *tty_driver;
static struct timer_list timer;

static void blink(struct timer_list *ptr);
static ssize_t sys_read (struct kobject *kobject, struct kobj_attribute *attr, char *buf);
static ssize_t sys_write(struct kobject *kobject, struct kobj_attribute *attr, const char *buf, size_t count);
static int  __init sys_init (void);
static void __exit sys_exit (void);

static struct kobj_attribute leds_attribute =__ATTR(
        leds,
        SYS_FRIGHTS,
        sys_read,
        sys_write
);

static int __init sys_init (void)
{
        printk(KERN_INFO LOG_MODULE_NAME "loading\n");

        struct vc_data* vc_data = vc_cons[fg_console].d;
        if (!vc_data)
                return -EINVAL;
        struct tty_struct* tty_struct = vc_data->port.tty;
        if (!tty_struct)
                return -EINVAL;

        tty_driver = tty_struct->driver;
        if (!tty_struct)
                return -EINVAL;

        leds_kobject = kobject_create_and_add(SYS_FNAME, kernel_kobj);
        if(!leds_kobject)
                return -ENOMEM;

        int error = 0;
        if ((error = sysfs_create_file(leds_kobject, &leds_attribute.attr))) {
                printk(KERN_INFO LOG_MODULE_NAME "failed to create the foo file in /sys/kernel/%s\n", SYS_FNAME);
        }
        printk(KERN_INFO LOG_MODULE_NAME "tty driver - %s: %x %x\n", tty_driver->name, tty_driver->major, tty_driver->minor_start);

        timer_setup(&timer, blink, 0);
        timer.expires = jiffies + BLINK_DELAY;
        add_timer(&timer);
        printk(KERN_INFO LOG_MODULE_NAME "initialized");
        return error;
}

static void __exit sys_exit (void)
{
        timer_delete_sync(&timer);
        (tty_driver->ops->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED,RESTORE_LEDS);
        kobject_put(leds_kobject);
        printk (KERN_INFO LOG_MODULE_NAME "uninitialized\n");
}

static ssize_t sys_read(struct kobject *kobject, struct kobj_attribute *attr, char *buf)
{
        printk(KERN_INFO LOG_MODULE_NAME "file output: %u", blink_mask);
        return sprintf(buf, "%u\n", blink_mask);
}
 
static ssize_t sys_write(struct kobject *kobject, struct kobj_attribute *attr, const char *buf, size_t count)
{
        printk(KERN_INFO LOG_MODULE_NAME "tried to parse - %s", buf);
        unsigned int tmp = 0;

        if(kstrtouint(buf, 0, &tmp) != 0)
                return -EINVAL;
        if (tmp > 7)
                return -EINVAL;
        blink_mask = tmp;

        printk(KERN_INFO LOG_MODULE_NAME "parsed - %u", blink_mask);
        return (ssize_t)count;
}

static void blink(struct timer_list *ptr) {
        if (!tty_driver || !tty_driver->ops || !tty_driver->ops->ioctl) {
                mod_timer(&timer, jiffies + BLINK_DELAY);
                printk(KERN_WARNING LOG_MODULE_NAME "no tty");
                return;
        }

        if (blink_mask == 0) {
                (tty_driver->ops->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED, RESTORE_LEDS);
                leds_status = 0;
        }else {
                if (leds_status == 0)
                        leds_status = blink_mask;
                else
                        leds_status = 0;
                (tty_driver->ops->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED, leds_status);
        }

        mod_timer(&timer, jiffies + BLINK_DELAY);
        //printk(KERN_INFO "-=-=-Blink!-=-=-");
}

module_init(sys_init);
module_exit(sys_exit);