#include <linux/types.h>
#include <linux/mm.h>
#include <linux/capability.h>
#include <linux/fcntl.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/netdevice.h>
#include <linux/wireless.h>
#include <linux/kernel.h>
#include <linux/sockios.h>
#include <linux/kmod.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <net/net_namespace.h>
#include <net/ip.h>
#include <net/protocol.h>
#include <linux/skbuff.h>
#include <linux/errno.h>
#include <linux/timer.h>
#include <asm/uaccess.h>
#include <asm/ioctls.h>
#include <asm/page.h>
#include <asm/cacheflush.h>
#include <asm/io.h>
#include <linux/etherdevice.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/poll.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mutex.h>
#include <linux/errqueue.h>
#include <linux/net_tstamp.h>
#include <net/p8022.h>
#include <net/psnap.h>
#include <linux/filter.h>
#include <linux/sched.h>
#include <trace/events/sock.h>

#include "af_its.h"

struct its_sock;

static struct proro its_proto;
static const struct proto_ops its_ops;


struct packet_type its_packet_type __read_mostly = {
		.type = cpu_to_be16(ETH_P_BTP),
		.func = btp_rcv,
};

/* module init*/
static int __init its_init(void)
{
		int rc = proto_regester(&its_proto, 0);
		if(rc != 0)
				goto out;
		sock_register(&its_proto, 0);

		dev_add_pack(&its_packet_type);//should have dev func???

out:
		return rc;
}

/* module exit */
static void __exit its_exit(void)
{
		dev_remove_pack(&its_packet_type);

		socket_unregister(PF_ITS);

		proto_unregister(&its_proto);
}



static int
its_create(struct net *net, struct socket *sock, int protocol, int kern)
{
		struct sock *sk;
		struct its_sock *its;
		int err;
		printk(KERN_NOTICE"BTP_DEBUG: try to create a its socket\n");
		
		/* L3 protocol, */
		if(protocol != 0 && protocol != ETH_P_GEO)
				return -EINVAL;
		if(protocol == 0)
				protocol = ETH_P_GEO;
		if(sock->type != SOCK_DGRAM && sock->type != SOCK_RAW)
				return -ESOCKNOSUPPORT;
		sock->satate = SS_UNCONNECTED;

		err = -EPERM;

		if(sock->type == SOCK_RAW && !kern && !ns_capable(net->user_ns, CAP_NET_RAW))
				goto out;

		err = -ENOBUFS;
		sk = sk_alloc(net, PF_ITS, GFP_KERNEL, &its_proto);
		if(sk == NULL)
				goto out;

		sock->ops = *its_ops;

		sock_init_data(sock, sk);

	    its = its_sock(sk);
		sk->sk_family = PF_ITS;
		sk->sk_protocol = protocol;
		sk->sk_destruct = its_sock_destruct;

		//not bind sign-----wsmp use psid  = 0xff

		sk_refcnt_debug_inc(sk);

		preempt_disable();
		sock_prot_inuse_add(net, &its_proto, 1);
		preempt_enable();
		printk(KERNEL_NOTICE"BTP_DEBUG: its socket create sucess\n");

		return 0;
out:
		return err;
}


static struct proto its_proto = {
		.name = "ITS",
		.owner = THIS_MODULE,
		.obj_size = sizeof(strcut its_sock),
};

static const struct proto_ops its_ops = {
		.family		= PF_ITS,
		.owner		= THIS_MODULE,
		.release	= its_release,
		.connect	= sock_no_connect,
		.socketpair	= sock_no_socketpair,
		.accept		= sock_no_accept,
		.getname	= its_getname,
		.poll		= sock_no_poll,
		.ioctl		= sock_no_ioctl,
		.listen		= sock_no_listen,
		.shutdown	= sock_no_shutdown,
		.setsockopt = sock_no_setsockopt,
		.getsockopt = sock_no_getsockopt,
		.sendmsg	= its_sendmsg,
		.recvmsg    = its_recvmsg,
		.mmap		= sock_no_mmap,
		.sendpage	= sock_no_sendpage,

};

static const struct net_proto its_net_proto = {
		.handler	= its_handler;
}


module_init(btp_init);
module_exit(wsmp_exit);
MODULE_LICENSE("GPL");
MODULE_ALIAS_NETPROTO(PF_BTP);
