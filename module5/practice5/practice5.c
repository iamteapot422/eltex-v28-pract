#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <net/sock.h>
#include <net/netlink.h>
#include <linux/skbuff.h>

#define NETLINK_USER 31
#define MSG_MAX_SIZE 1024

static struct sock *nl_sk = NULL;

static void nl_recv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    int pid;
    struct sk_buff *skb_out;
    int msg_size;
    char *msg = "Kernel response";
    int res;

    nlh = (struct nlmsghdr *)skb->data;

    pr_info("practice5: new message: %s\n",
            (char *)nlmsg_data(nlh));

    msg_size = strlen(msg);
    pid = nlh->nlmsg_pid;

    skb_out = nlmsg_new(msg_size, GFP_KERNEL);
    if (!skb_out) {
        pr_err("practice5: couldn't create skb\n");
        return;
    }

    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    NETLINK_CB(skb_out).dst_group = 0;
    memcpy(nlmsg_data(nlh), msg, msg_size);

    res = nlmsg_unicast(nl_sk, skb_out, pid);
    if (res < 0)
        pr_err("practice5: sending error, code %d\n", res);
}

static int __init netlink_test_init(void)
{
    struct netlink_kernel_cfg cfg = {
        .input = nl_recv_msg,
    };

    pr_info("practice5: initialization\n");

    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if (!nl_sk) {
        pr_err("practice5: error while creating netlink-socket\n");
        return -ENOMEM;
    }

    return 0;
}

static void __exit netlink_test_exit(void)
{
    pr_info("practice5: unloading the module\n");
    netlink_kernel_release(nl_sk);
}

module_init(netlink_test_init);
module_exit(netlink_test_exit);

MODULE_LICENSE("Fishfish");
MODULE_AUTHOR("Bogdan Novitskiy");
MODULE_DESCRIPTION("Netlnk module for userspace<->kernel");