#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/inet.h>
#include <net/net_namespace.h>

MODULE_AUTHOR("Bogdan Novitskiy");
MODULE_DESCRIPTION("IP blacklist");
MODULE_LICENSE("Fishfish");

#define PROC_NAME "ipblock"
#define MAX_INPUT 64

struct blocked_ip {
    struct list_head list;
    __be32 addr;
};

static LIST_HEAD(blocked_list);
static DEFINE_MUTEX(blocked_lock);
static struct nf_hook_ops nf_out_hook;
static struct proc_dir_entry *proc_entry;

static bool ip_is_blocked(__be32 addr)
{
    struct blocked_ip *entry;
    bool found = false;

    mutex_lock(&blocked_lock);
    list_for_each_entry(entry, &blocked_list, list) {
        if (entry->addr == addr) {
            found = true;
            break;
        }
    }
    mutex_unlock(&blocked_lock);

    return found;
}

static int ip_add(__be32 addr)
{
    struct blocked_ip *entry;

    if (ip_is_blocked(addr))
        return -EEXIST;

    entry = kmalloc(sizeof(*entry), GFP_KERNEL);
    if (!entry)
        return -ENOMEM;

    entry->addr = addr;

    mutex_lock(&blocked_lock);
    list_add_tail(&entry->list, &blocked_list);
    mutex_unlock(&blocked_lock);

    return 0;
}

static int ip_del(__be32 addr)
{
    struct blocked_ip *entry, *tmp;
    int ret = -ENOENT;

    mutex_lock(&blocked_lock);
    list_for_each_entry_safe(entry, tmp, &blocked_list, list) {
        if (entry->addr == addr) {
            list_del(&entry->list);
            kfree(entry);
            ret = 0;
            break;
        }
    }
    mutex_unlock(&blocked_lock);

    return ret;
}

static void ip_clear_all(void)
{
    struct blocked_ip *entry, *tmp;

    mutex_lock(&blocked_lock);
    list_for_each_entry_safe(entry, tmp, &blocked_list, list) {
        list_del(&entry->list);
        kfree(entry);
    }
    mutex_unlock(&blocked_lock);
}

static unsigned int hook_func_out(void *priv, struct sk_buff *skb,
                                   const struct nf_hook_state *state)
{
    struct iphdr *ip_header;

    if (!skb)
        return NF_ACCEPT;

    ip_header = ip_hdr(skb);
    if (!ip_header)
        return NF_ACCEPT;

    if (ip_is_blocked(ip_header->daddr)) {
        printk(KERN_INFO "ipblock: dropped packet to %pI4\n", &ip_header->daddr);
        return NF_DROP;
    }

    return NF_ACCEPT;
}

static ssize_t proc_write(struct file *file, const char __user *ubuf,
                           size_t count, loff_t *ppos)
{
    char buf[MAX_INPUT];
    char cmd[8];
    char ipstr[32];
    __be32 addr;
    int ret;

    if (count == 0 || count >= MAX_INPUT)
        return -EINVAL;

    if (copy_from_user(buf, ubuf, count))
        return -EFAULT;

    buf[count] = '\0';

    if (buf[count - 1] == '\n')
        buf[count - 1] = '\0';

    ret = sscanf(buf, "%7s %31s", cmd, ipstr);

    if (strcmp(cmd, "clear") == 0) {
        ip_clear_all();
        return count;
    }

    if (ret != 2)
        return -EINVAL;

    addr = in_aton(ipstr);

    if (strcmp(cmd, "add") == 0) {
        ret = ip_add(addr);
        if (ret)
            return ret;
    } else if (strcmp(cmd, "del") == 0) {
        ret = ip_del(addr);
        if (ret)
            return ret;
    } else {
        return -EINVAL;
    }

    return count;
}

static int proc_show(struct seq_file *m, void *v)
{
    struct blocked_ip *entry;

    mutex_lock(&blocked_lock);
    list_for_each_entry(entry, &blocked_list, list) {
        seq_printf(m, "%pI4\n", &entry->addr);
    }
    mutex_unlock(&blocked_lock);

    return 0;
}

static int proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_show, NULL);
}

static const struct proc_ops proc_fops = {
    .proc_open = proc_open,
    .proc_read = seq_read,
    .proc_write = proc_write,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init ipblock_init(void)
{
    proc_entry = proc_create(PROC_NAME, 0644, NULL, &proc_fops);
    if (!proc_entry) {
        printk(KERN_ERR "ipblock: failed to create proc entry\n");
        return -ENOMEM;
    }

    nf_out_hook.hook = hook_func_out;
    nf_out_hook.hooknum = NF_INET_LOCAL_OUT;
    nf_out_hook.pf = PF_INET;
    nf_out_hook.priority = NF_IP_PRI_FIRST;

    nf_register_net_hook(&init_net, &nf_out_hook);

    printk(KERN_INFO "ipblock: module loaded\n");

    return 0;
}

static void __exit ipblock_exit(void)
{
    nf_unregister_net_hook(&init_net, &nf_out_hook);
    proc_remove(proc_entry);
    ip_clear_all();

    printk(KERN_INFO "ipblock: module unloaded\n");
}

module_init(ipblock_init);
module_exit(ipblock_exit);