#ifndef __AF_BTP_H
#define __AF_BTP_H

#include <linux/types.h>
#include <net/sock.h>
/*
 * this struct is the sockaddr used when sending and receiving messages 
 * 
 */

extern char its_dev_mark[];

/* set sock addr , which refers to wsmp */
struct sockaddr{
		unsigned char type:4,
					  comprofile:2,
					  secprofile:2;
		int16_t sport;
		int16_t dport;
		int16_t dpinfofo; // no use until document update
		struct gn_addr GN_ADDR;
		unsigned char maxpklf;
		int32_t repint;
		unsigned char repiime;
		unsigned char hop_l;
		unsigned char TC;
		uint16_t length;
};


struct its_sock
{
		struct sock sk;
		unsigned char type:4,
					  comprofile:2,
					  secprofile:2;
		int16_t	sport;
		int16_t	dport;
		int16_t	dpinfo;
		struct gn_addr GN_ADDR;
		struct geoarea ga;
		unsigned char maxpklf;
		int32_t repint;
		unsigned char reptime;
		unsigned char hop_l;
		unsigned char TC;
		uint16_t length;
}

/* this function is to change pointer's type sock to type its_sock */
static struct its_sock *its_sock(struct sock *sk)
{
		return (struct its_sock *)sk;
}


static const struct net_proto_family its_family_ops = {
		.family = PF_ITS,
		.create = its_create,
		.owner = THIS_MODULE,
};


static int its_create(struct net *net, struct socket *sock, int protocol ,int kern)
{

		struct sock *sk;
		struct its_sock *isock;
		int err;
		printk(KERN_NOTICE"ITS_DEBUG: try to create an its socket\n");

		//L3 protocol, only ETH_P_geo is supported now.
		if(protocol != 0 && protocol != ETH_P_GEO)
				return -EINVAL;
		if(protocol == 0)
				protocol = ETH_P_GEO;
		//set default L3 protocol Geo

		if(sock->type != SOCK_DGRAM && sock->type != SOOCK_RAW)
				return -ESOCKTNOSUPPORT;

		sock->state = SS_UNCONNECTED;
		
		err = -EPERM;//Permission not support

		if(sock->type == SOCK_RAW && !kern && !nss_capable(net->user_ns, CAP_NET_RAW))
				goto out;
		error = -ENOBUFS;
		
		//create a its_sock
		sk = sk_alloc(net, PF_ITS, GFP_KERNEL, &its_proto);
		if(sk ==NULL)
				goto out;

		sock->ops = &its_ops;

		sock_init_data(sock, sk);

		isock = its_sk(sk);
		sk->sk_family = PF_ITS;
		sk->sk_protocol = procotol;
		sk->sk_destruct = its_sock_destruct;
		//ignore psid part, to do .....
		
		sk_refcnt_debug_inc(sk);

		preempt_disable();
		sock_prot_inuse_add(net, &its_proto, 1);
		preempt_enable();
		printk(KERN_NOTICE"ITS_DEBUG: its socket create success\n");

		return 0;

out:
		return err;
}
//export its_create from the mod,and other mod can use it, not sure is it necessary in its?
EXPORT_SYMBOL_GPL(its_create);

