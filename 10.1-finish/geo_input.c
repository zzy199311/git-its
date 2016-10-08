#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/slab.h>

#include <linux/net.h>
#include <linux/socket.h>
#include <linux/sockios.h>
#include <linux/in.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#include <net/ip.h>
#include <net/protocol.h>
#include <net/route.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#include <net/raw.h>
#include <net/checksum.h>
#include <linux/netfilter_ipv4.h>
#include <net/xfrm.h>
#include <linux/mroute.h>
#include <linux/netlink.h>

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


#include "mib.h"
#include "geo_input.h"
#include "geo_output.h"
/*
//#define GEO_SECURED_HEADER 2
#define GEO_BASIC_HEADER_LEN 4
#define GEO_COMMON_HEADER_LEN 8
*/

static int skb_len(struct sk_buff *skb)
{
    int len;

    //printk(KERN_INFO "Pointer skb->data = %p.\n", skb->data);
    //printk(KERN_INFO "Pointer skb->tail = %p.\n", skb->tail);
    len = (skb->tail - skb->data);
    printk(KERN_INFO "The octet of packet is %d.\n", len);
    return len;
}

int btp_rcv(struct gn_indication_param *gn_indication_params, struct sk_buff *skb)
{
    printk(KERN_INFO "btp_rcv\n");
    return 0;
}

/*
static (struct geohdr_common)* get_common_header(struct geohdr_basic *gbhdr)
{
    struct geohdr_common* gchdr = (struct geohdr_common*)((unsigned char*)gbhdr + GEO_BASIC_HEADER_LEN);
    return gchdr;
}
*/

int gn_data_indication(struct sk_buff *skb, struct geohdr_basic *gbhdr, struct geohdr_common *gchdr, int type)
{
    printk(KERN_INFO "---------------------Service gn_indication---------------------------\n");
    int err;
    int len;
    struct gn_indication_param *gn_indication_params;
    gn_indication_params =(struct gn_indication_param *)kmalloc(sizeof(struct gn_indication_param), GFP_KERNEL);
    switch(type){
        case SHB:
            {
                struct geohdr_shb* gehdr = (struct geohdr_shb*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
                //set source position vector
                printk(KERN_INFO "GN_indication for SHB\n");
                gn_indication_params->so_pv = gehdr->so_pv;
                len =sizeof(struct geohdr_shb);
                break;
            }
        case GUC:
            {
                struct geohdr_guc* gehdr = (struct geohdr_guc*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
                printk(KERN_INFO "GN_indication for GUC\n");
                //deaddr means guc's address(MID only,other set 0) or gac/gbc's geoarea
                strcpy(gn_indication_params->deaddr.macaddr, gehdr->de_pv.GN_ADDR.macaddr);
                gn_indication_params->deaddr.m = 0;
                gn_indication_params->deaddr.scc = 0;
                gn_indication_params->deaddr.st = 0;
                //set source position vector
                gn_indication_params->so_pv = gehdr->so_pv;
                len =sizeof(struct geohdr_guc);
                break;
            }
        case GAC:
            {
                struct geohdr_gabc* gehdr = (struct geohdr_gabc*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
                printk(KERN_INFO "GN_indication for GAC\n");
                gn_indication_params->de_po = gehdr->gap;
                //set source position vector
                gn_indication_params->so_pv = gehdr->so_pv;
                len =sizeof(struct geohdr_gabc);
                break;
            }
        case GBC:
            {
                struct geohdr_gabc* gehdr = (struct geohdr_gabc*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
                printk(KERN_INFO "GN_indication for GBC\n");
                gn_indication_params->de_po = gehdr->gap;
                //set source position vector
                gn_indication_params->so_pv = gehdr->so_pv;
                len =sizeof(struct geohdr_gabc);
                break;
            }
        case TSB:
            {
                struct geohdr_tsb* gehdr = (struct geohdr_tsb*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
                printk(KERN_INFO "GN_indication for TSB\n");
                //set source position vector
                gn_indication_params->so_pv = gehdr->so_pv;
                len =sizeof(struct geohdr_tsb);
                break;
            }
        default:
            goto out;
            break;
    }


    //set upper protocol entity
    if((gchdr->nexthdr == 1) || (gchdr->nexthdr = 2))
        gn_indication_params->uphrt = 0;// upper protocol entity 0 for btp; 1 for ipv6
    else if(gchdr->nexthdr == 3)
        gn_indication_params->uphrt = 1;

    //set gn_indication transporttype which is idenical to the hdrtypr in common_header
    gn_indication_params->ht = gchdr->hdrt;
    gn_indication_params->hst = gchdr->hdrst;

    //set traffic class
    gn_indication_params->scf = gchdr->scf;
    gn_indication_params->chan_offload = gchdr->chan_offload;
    gn_indication_params->tc_id = gchdr->tc_id;

    //set data pointer
    gn_indication_params->data = skb->data;

    //set security report
    //set certificate id
    //set permission
    //set remaining packet lifetime
    //set remaining hop limit
    //set length

    printk(KERN_INFO "gn_indication_params.ht = %d\n", gn_indication_params->ht);
    printk(KERN_INFO "gn_indication_params.hst = %d\n", gn_indication_params->hst);
    printk(KERN_INFO "gn_indication_params.scf = %d\n", gn_indication_params->scf);
    printk(KERN_INFO "gn_indication_params.chan_offload = %d\n", gn_indication_params->chan_offload);
    printk(KERN_INFO "gn_indication_params.tc_id = %d\n", gn_indication_params->tc_id);
    printk(KERN_INFO "gn_indication_params.so_pv->speed = %d\n", gn_indication_params->so_pv.speed);
    len = len + GEO_BASIC_HEADER_LEN + GEO_COMMON_HEADER_LEN;
    printk(KERN_INFO "length of network header to be pulled = %d\n", len);
    skb_orphan(skb);
    printk(KERN_INFO "skb->data before pull = %p\n", skb->data);
    __skb_pull(skb, len);
    printk(KERN_INFO "skb_pull is ok\n");
    printk(KERN_INFO "skb->data after pull = %p\n", skb->data);
    err = btp_rcv(gn_indication_params, skb);
    if(err != 0)
        goto out;
    return err;
out:
    return -1;
}


int geo_guc_rcv_handling(struct sk_buff *skb, struct geohdr_basic *gbhdr, struct geohdr_common *gchdr)
{
    printk(KERN_INFO "--------------------GEO_GUC_HEADER PROCESS--------------------\n");
    int err;
    struct geohdr_guc* gehdr = (struct geohdr_guc*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
    printk(KERN_INFO "gehdr->sn = %d\n", gehdr->sn);
    printk(KERN_INFO "gehdr->lati = %d\n", gehdr->so_pv.latitude);
    printk(KERN_INFO "gehdr->longti = %d\n", gehdr->so_pv.longitude);
    printk(KERN_INFO "gehdr->speed = %d\n", gehdr->so_pv.speed);
    printk(KERN_INFO "gehdr->heading = %d\n", gehdr->so_pv.heading);
    //check gn addr in de_pv and match local gn addr
    /*
    if(gehdr->de_pv.GN_ADDR != local.GN_ADDR)
        err = geo_guc_forward();
*/
    //execute DPD
    //execute DAD
    //check LocTE(SO)
    //flush buffer
    //
    err = gn_data_indication(skb, gbhdr, gchdr, GUC);
    if(err  != 0)
        printk(KERN_INFO "ERROR: gn_indication service fail!");
    return err;

out:
    return err;
}

int geo_shb_rcv_handling(struct sk_buff *skb, struct geohdr_basic *gbhdr, struct geohdr_common *gchdr)
{
    printk(KERN_INFO "--------------------GEO_SHB_HEADER PROCESS--------------------\n");
    struct geohdr_shb* gehdr = (struct geohdr_shb*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
    printk(KERN_INFO "gehdr_shb: %p\n", gehdr);
    printk(KERN_INFO "gehdr_shb->tst = %d\n", gehdr->so_pv.tst);
    printk(KERN_INFO "gehdr_shb->lati = %d\n", gehdr->so_pv.latitude);
    printk(KERN_INFO "gehdr_shb->longti = %d\n", gehdr->so_pv.longitude);
    printk(KERN_INFO "gehdr_shb->speed = %d\n", gehdr->so_pv.speed);
    printk(KERN_INFO "gehdr_shb->pai = %d\n", gehdr->so_pv.pai);
	printk(KERN_INFO "gehdr_shb->heading = %d\n", gehdr->so_pv.heading);

    int err;
    int len;
    int flag;
    uint64_t index;
    struct itsGnIfEntry *entry;

    //执行DAD操作
    err = dup_addr_detect(gehdr->so_pv);
    if(err != 0){
        printk(KERN_INFO "The packet sender's address is same with local address!\n");
        //return err;
    }
    len = skb_len(skb);

    //计算哈希值
    index = hash_compute(gehdr->so_pv.GN_ADDR.macaddr);
    //查找是否存在与MAC对应的路由表项
    entry = get_entry_by_index(index, gehdr->so_pv.GN_ADDR.macaddr);
    if(entry == NULL){
        printk(KERN_INFO "Mib doesn't include corresponding entry about the sta's mac address!\n");
        printk(KERN_INFO "Create an entry for MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
                gehdr->so_pv.GN_ADDR.macaddr[0],gehdr->so_pv.GN_ADDR.macaddr[1],gehdr->so_pv.GN_ADDR.macaddr[2],
                gehdr->so_pv.GN_ADDR.macaddr[3],gehdr->so_pv.GN_ADDR.macaddr[4],gehdr->so_pv.GN_ADDR.macaddr[5]);
        //创建相应的路由表
        entry = add_mib_entry(index, gehdr->so_pv.GN_ADDR.macaddr);
        flag = 0;
    }
    else{
        printk(KERN_INFO "Mib already includes corresponding entry of MAC = %02x:%02x:%02x:%02x:%02x:%02x\n",
                gehdr->so_pv.GN_ADDR.macaddr[0],gehdr->so_pv.GN_ADDR.macaddr[1],gehdr->so_pv.GN_ADDR.macaddr[2],
                gehdr->so_pv.GN_ADDR.macaddr[3],gehdr->so_pv.GN_ADDR.macaddr[4],gehdr->so_pv.GN_ADDR.macaddr[5]);

		//执行DPD检测
    	err = dpd_single(gehdr->so_pv.tst, entry);
    	if(err != 0){
        	printk(KERN_INFO "The packet is a duplicate packet\n");
        	return err;
    	}
        flag = 1;
    }
    //执行表项的update操作,包括更新pdr，设置neighbour标记
    err = update_mib_entry(entry, gbhdr, gchdr, SHB, flag);
    if(err != 0)
        return err;
    //update PDR;
    err = pdr_update(entry, len);

    //pass the payload with GN-DATA.indication with parameter setting
    err = gn_data_indication(skb, gbhdr, gchdr, SHB);
    if(err != 0)
        printk(KERN_INFO "ERROR: gn_indication service fail!");
    return err;
}

int geo_beacon_rcv_handling(struct sk_buff *skb, struct geohdr_basic *gbhdr, struct geohdr_common *gchdr)
{
    //identical to SHB packet, except step 8
    int err;
    err = geo_shb_rcv_handling(skb, gbhdr, gchdr);
    return err;
}

int geo_tsb_rcv_handling(struct sk_buff *skb, struct geohdr_basic *gbhdr, struct geohdr_common *gchdr)
{
		int err;
        //execute DPD
        //execute DAD
        //check LocTE(SO)
        //

		err = gn_data_indication(skb, gbhdr, gchdr, TSB);
		return err;

}

int geo_secured_handling(struct sk_buff *skb, struct geohdr_basic *gbhdr)
{
		//to do .....
        printk(KERN_INFO "This packet is a secrued packet!\n");
		return 0;
}

int geo_common_handling(struct sk_buff *skb, struct geohdr_basic *gbhdr)
{
    printk(KERN_INFO "--------------------GEO_COMMON_HEADER PROCESS--------------------\n");
    int err;
    struct geohdr_common* gchdr = (struct geohdr_common*)((unsigned char*)gbhdr + GEO_BASIC_HEADER_LEN);
    //gchdr = gbhdr + GEO_COMMON_HEADER_LEN;
    printk(KERN_INFO "gchdr: %p\n", gchdr);

    if(gchdr->mhl < gbhdr->rhl){
        printk(KERN_INFO "geo_input: geo_common_handling: hop limit exceed\n");
        return -1;
    }
    // bc forwarding buffer is not empty, flush it -- to do....

    switch(gchdr->nexthdr)
    {
        case 0:
            printk(KERN_INFO "geo_input: geo_common_handling: next header is unspecified\n");
            err = -1;
            break;
        case 1:
            printk(KERN_INFO "geo_input: geo_common_handling: next header is BTP-A\n");
            err =0;
            break;
        case 2:
            printk(KERN_INFO "geo_input: geo_common_handling: next header is BTP-B\n");
            err =0;
            break;
        case 3:
            printk(KERN_INFO "geo_input: geo_common_handling: next header is IPV6\n");
            //goto ipv6;
			break;
    }

    switch(gchdr->hdrt)
    {
        case 0:
            printk(KERN_INFO "geo_input: geo_common_handling: next header is ANY\n");
            err = -1;
            break;
        case 1:
            {
                printk(KERN_INFO "geo_input: geo_common_handling: next header is BEACON\n");
                struct geohdr_beacon* gehdr = (struct geohdr_beacon*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
                err = geo_beacon_rcv_handling(skb, gbhdr, gchdr);
                break;
            }
/*	        case 2:
						err = geo_guc_rcv_handling(skb, gbhdr, gchdr);
						break;
				case 3:
						err = geo_gac_rcv_handling(skb, gbhdr, gchdr);
						break;
				case 4:
						err = geo_gbc_rcv_handling(skb, gbhbr, gchdr);
						break;
						*/
        case 5:
            switch(gchdr->hdrst){
                case 0:
                    printk(KERN_INFO "geo_input: geo_common_handling: next header is SHB\n");
                    err = geo_shb_rcv_handling(skb, gbhdr, gchdr);
                    break;
                case 1:
                    printk(KERN_INFO "geo_input: geo_common_handling: next header is TSB\n");
                    err = geo_tsb_rcv_handling(skb, gbhdr, gchdr);
                    break;
                default:
                    printk(KERN_INFO "geo_input: geo_common_handling: next header is OTHER\n");
                    err = -1;
                    break;
            }
            break;
/*				case 6:
						err = geo_ls_rcv_handling(skb, gbhdr, gchdr);
						break;
						*/
        default:
            err = -1;
            break;
    }
    return err;

//ipv6:
//		return ipv6_handling();
}

int geo_basic_handling(struct sk_buff *skb)
{
    printk(KERN_INFO "--------------------GEO_BASIC_HEADER PROCESS--------------------\n");
    int ret;
    struct geohdr_basic* gbhdr = (struct geohdr_basic*)skb_network_header(skb);//geohdr point to geo basic header
    printk(KERN_INFO "ghbdr: %p\n", gbhdr);

    if(gbhdr->version != itsLocalInfo.LocalConfig.itsGnProtocolVersion){
        printk(KERN_INFO "geo version is wrong! drop the packet.");
        return -1;
    }
    printk(KERN_INFO "geo_input:  geo_basic_handling : remaining hop limit is %d.\n", gbhdr->rhl);
    printk(KERN_INFO "geo_input:  geo_basic_handling : Lifetime mul is %d.\n", gbhdr->mul);
    printk(KERN_INFO "geo_input:  geo_basic_handling : Lifetime base is %d.\n", gbhdr->base);

    switch(gbhdr->nexthdr)
    {
        case 0:
            printk(KERN_INFO "geo_input:  geo_basic_handling : next header is any.\n");
            ret = geo_common_handling(skb, gbhdr);
            break;
        case 1:
            printk(KERN_INFO "geo_input:  geo_basic_handling : next header is common header.\n");
            ret = geo_common_handling(skb, gbhdr);
            break;
        case 2:
            printk(KERN_INFO "geo_input:  geo_basic_handling : next header is secured header.\n");
            ret = geo_secured_handling(skb, gbhdr);
            break;
        default:
            printk(KERN_INFO "geo_input:  geo_basic_handling : next header is wrong!\n");
            ret = -1;
            break;
    }
    printk(KERN_INFO "geo_common_handling! err = %d.\n", ret);

    return ret;
}

/*
int geo_rcv_finish(struct sk_buff *skb, struct geohdr_p *ghdr, int geo_hdrlen)
{
    printk(KERN_INFO "now execute geo_rcv_finsih\n");
		int err;
		printk("GEO_DEBUG: geo_rcv_finish\n");
		err = geo_packet_handling(skb, ghdr);

		if(!err == 1)
				goto drop;

		// now skb->data point to geo data, header is skiped
		__skb_pull(skb, geo_hdrlen);

		return err;
drop:
		kfree_skb(skb);
        return -1;
}
*/

int geo_rcv(struct sk_buff *skb/*, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev*/)
{
    printk(KERN_INFO "--------------------GEO_RCV--------------------\n");
    int err;

    if(skb->pkt_type == PACKET_OTHERHOST){
        printk(KERN_NOTICE"GEO_DEBUG: geo_rcv: packet is to other host\n");
        goto drop;
    }
    else
        printk(KERN_INFO "geo_rcv: packect is to local host\n");

    if((skb = skb_share_check(skb, GFP_ATOMIC)) == NULL)
        goto out;
    if(!pskb_may_pull(skb, GEO_BASIC_HEADER_LEN)){
        printk(KERN_NOTICE"GEO_DEBUG: geo_rcv: packet is too short\n");
        goto drop;
    }


    //skb_orphan(skb);

    err = geo_basic_handling(skb);
    if(err != 0){
        printk(KERN_INFO "Geo_rcv wrong! err = %d.\n", err);
    	goto drop;
	}
	else
		return 0;

drop:
    //kfree_skb(skb);
    return -1;
out:
    return NET_RX_DROP;
}



