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

#include "geo_output.h"


int geo_output(struct sk_buff *skb)
{
    printk(KERN_INFO "geo output!\n");
    return 0;
}

static int extend_header_len(int type)
{
    int len = -1;
    switch(type)
    {
        case BEACON:
            len = sizeof(struct geohdr_beacon);
            break;
        case GUC:
            len = sizeof(struct geohdr_guc);
            break;
        case GAC:
            len = sizeof(struct geohdr_gabc);
            break;
        case GBC:
            len = sizeof(struct geohdr_gabc);
            break;
        case SHB:
            len = sizeof(struct geohdr_shb);
            break;
        case TSB:
            len = sizeof(struct geohdr_tsb);
            break;
        case LSREQ:
            len = sizeof(struct geohdr_lsreq);
            break;
        case LSREP:
            len = sizeof(struct geohdr_lsrep);
            break;
        default:
            break;
    }
    return len;
}

int gn_basic_header_create(struct sk_buff *skb, struct gn_request_param *params, int type)
{
    struct geohdr_basic *gbhdr;
    gbhdr = (struct geohdr_basic*)skb_push(skb, GEO_BASIC_HEADER_LEN);
    if(gbhdr == NULL){
        printk(KERN_INFO "ERROR: Can't push basic data into skb!\n");
        goto out;
    }
    skb_reset_network_header(skb);
    //set version
    gbhdr->version = itsGnProtocolVersion;
    //set next header
    if(params->secpro == 1)
        gbhdr->nexthdr = 2;//Secured
    else if(params->secpro ==0)
        gbhdr->nexthdr = 1;//Common
    //set reserved
    gbhdr->reserved = 0;

    //set lifetime
    if((params->mul == UNSET)||(params->base == UNSET))
    {
        gbhdr->mul = 60;//0~63
        else
            gbhdr->rhl = params->max_hoplimit;
    }
    return 0;
out:
    return -1;
}


int gn_common_header_create(struct sk_buff *skb, struct gn_request_param *params, int type)
{
    struct geohdr_common *gchdr;
    gchdr = (struct geohdr_common*)skb_push(skb, GEO_COMMON_HEADER_LEN);
    if(gchdr == NULL){
        printk(KERN_INFO "ERROR: Can't push common data into skb!\n");
        goto out;
    }
    skb_reset_network_header(skb);

    //set max hop limit
    if(type == SHB)
        gchdr->mhl = 1;
    else{
        if(params->max_hoplimit == UNSET){
            printk(KERN_INFO "Use default Max Hop Limit\n");
            gchdr->mhl = itsGnDefaultHopLimit;
        }
        else
            gchdr->mhl = params->max_hoplimit;
    }

    //set traffic class
    if(params->tc_id == UNSET){
        printk(KERN_INFO "Use default Traffic Class!\n");
        gchdr->scf = 0;
        gchdr->chan_offload = 0;
        gchdr->tc_id = 0;
    }
    else{
        gchdr->scf = params->scf;
        gchdr->chan_offload = params->chan_offload;
        gchdr->tc_id = params->tc_id;
    }
    gchdr->nexthdr = params->uphrt;
    gchdr->flags = itsGnIsMobile;
    gchdr->flags_reserved = 0;
    gchdr->reserved = 0;
    gchdr->reserved1 = 0;
    gchdr->hdrt = params->ht;
    gchdr->hdrst = params->hst;
    gchdr->pl = params->len;
    return 0;
out:
    return -1;
}



int shb_header_create(struct sk_buff *skb, struct gn_request_param *params)
{
    int err;
    struct geohdr_shb *gehdr;
    gehdr = (struct geohdr_shb*)skb_push(skb, sizeof(struct geohdr_shb));
    if(gehdr == NULL){
        printk(KERN_INFO "ERROR: Can't push shb data into skb!\n");
        goto out;
    }
    skb_reset_network_header(skb);

    //set so_pv from mib
    //p->so_pv = ;

    return 0;

out:
    return -1;
}


static int unsec_packet_create(struct sk_buff *skb, struct gn_request_param *params, int type)
{
    int err = -1;

    switch(type)
    {
        case GUC:
            {
                printk(KERN_INFO "Create an unsecrued GUC packet!\n");
                //err = guc_header_create(skb, params);
                break;
            }
        case GAC:
            {
                printk(KERN_INFO "Create an unsecrued GAC packet!\n");
                //err = gabc_header_create(skb, params, type);
                break;
            }
        case GBC:
            {
                printk(KERN_INFO "Create an unsecrued GBC packet!\n");
                //err = gabc_header_create(skb, params, type);
                break;
            }
        case SHB:
            {
                printk(KERN_INFO "Create an unsecrued SHB packet!\n");
                err = shb_header_create(skb, params);
                break;
            }
        case TSB:
            {
                printk(KERN_INFO "Create an unsecrued TSB packet!\n");
                //err = tsb_header_create(skb, params);
                break;
            }
    }

    if(err == -1)
        goto out;

    if(gn_common_header_create(skb, params, type) == -1)
        goto out;

    if(gn_basic_header_create(skb, params, type) == -1)
        goto out;

    return 0;
out:
    return -1;
}


int geo_shb_src_handling(struct sk_buff *skb, struct gn_request_param *params)
{
    int err;
    int type = SHB;
    int len = params->len + GEO_BASIC_HEADER_LEN + GEO_COMMON_HEADER_LEN + sizeof(struct geohdr_shb);

    if(params->secpro ==1)
    {
        printk(KERN_INFO "This shb packet requests SN_ENCAP!\n");
        //skb_cp = gn_sec_prepare(skb, params);

        //err = basic_header_create(skb_cp, params, type);
        //if(err != 0)
        //    goto drop;
    }
    else if(params->secpro == 0)
    {
        err = unsec_packet_create(skb, params, type);
        if(err != 0)
            goto out;
    }
    else{
        printk(KERN_INFO "Wrong security profile!\n");
        goto out;
    }

    //find neighor
    //check scf
    if((params->max_reptime != UNSET)||(params->max_reptime != 0)){
	printk(KERN_INFO "This TSB packet requests retransmition!\n");
    }

    if(params->compro == 1){
        printk(KERN_INFO "Appilication request ITS-G5!\n");
        goto out;
    }

    err = geo_output(skb);
    if(err != 0)
        goto out;
    return 0;
out:
    return -1;

}


long geo_area_compute(struct gn_request_param *params)
{
    long area = -1;
    if((params->ht == 3)||(params->ht == 4))
    {
        if(params->hst == 0)
            area = pi *(int)(params->de_po.a) * (int)(params->de_po.a);
        if(params->hst == 1)
            area = (int)(params->de_po.a) * (int)(params->de_po.b);
        if(params->hst == 2)
            area = (int)(params->de_po.a) * (int)(params->de_po.b) * pi;
    }
    else
        area = -1;
    return area;
}

int gn_data_request_rcv(struct gn_request_param *params, struct sk_buff *skb)
{
	//struct sock *sk = skb->sk;
	//struct geo_sock *geo = geo_sk(sk);//to do .. create a new struct geo_sk
    struct gn_confirm_param *iparams;
    iparams = (struct gn_confirm_param*)kmalloc(sizeof(struct gn_confirm_param), GFP_KERNEL);
    //int copy;
    int err = 0;
    long area_size;
    printk(KERN_INFO "-------------------GN REQUEST--------------------\n");

    if(params->uphrt == 0)
        printk(KERN_INFO "This request from ITS Transport protocol BTP\n");
    else if(params->uphrt == 1){
        printk(KERN_INFO "This request from GeoNetworking to IPv6 Adaptation Layer GN6ASL\n");
        //err = GN6ASL()
    }
    else{
        printk(KERN_INFO "Wrong upper header type!\n");
        iparams->resultcode = 7;
        goto confirm;
    }

    if(params->len > itsGnMaxSduSize){
        printk(KERN_INFO "ERROR: The length of SDU is larger than itsGnMaxSduSize!\n");
        iparams->resultcode = 2;
        goto confirm;
    }

    if((params->repint != UNSET)&&(params->repint < itsGnMinPacketRepetitionInterval)){
        printk(KERN_INFO "ERROR: The repetition interval is smaller than itsGnMinPacketReptitionInterval!\n");
        iparams->resultcode = 4;
        goto confirm;
    }

    if(((params->base == 3)&&(params->mul > 60))||((params->base == 4)&&(params->mul > 6))){
        printk(KERN_INFO "ERROR: The Lifetime of packet is larger than itsGnMaxPacketLifetime!\n");
        iparams->resultcode = 3;
        goto confirm;
    }

    area_size = geo_area_compute(params);
    if((area_size != -1)&&(area_size > 10^8*itsGnMaxGeoAreaSize)){
        printk(KERN_INFO "ERROR: The area of destination position is larger than itsGnMaxGeoAreaSize!\n");
        iparams->resultcode = 6;
        goto confirm;
    }

    if(params->tc_id != 0){
        printk(KERN_INFO "ERROR: The Traffic Class is not supported!\n");
        iparams->resultcode = 5;
        goto confirm;
    }
    switch(params->ht)
    {
        case 2:
            printk(KERN_INFO "The packet is GUC\n");
            //err = geo_guc_src_handling(skb, params);
            break;
        case 3:
            printk(KERN_INFO "The packet is GAC\n");
            //err = geo_gabc_src_handling(skb, params, GAC);
            break;
        case 4:
            printk(KERN_INFO "The packet is GBC\n");
            //err = geo_gabc_src_handling(skb, params, GBC);
            break;
        case 5:{
            switch(params->hst){
                case 0:
                    printk(KERN_INFO "The packet is SHB\n");
                    //err = geo_shb_src_handling(skb, params);
                    break;
                case 1:
                    printk(KERN_INFO "The packet is TSB\n");
                    //err = geo_tsb_src_handling(skb, params);
                    break;
                default:
                    printk(KERN_INFO "ERROR: Wrong packet transport type! ht = %d, hst =%d.\n", params->ht, params->hst);
                    err = -1;
                    break;
            }
            break;
               }
    }
    if(err != 0){
        iparams->resultcode = 7;
        goto confirm;
    }

    //create basic header


confirm:
    printk(KERN_INFO "Gn_Confirm result code = %d.\n", iparams->resultcode);
    //gn_confirm(iparams);
    if(iparams->resultcode == 1)
        return 0;
    else
        goto drop;

drop:
    kfree_skb(skb);
    kfree(iparams);
    return -1;

}
/*
static int extend_header_create(struct sk_buff *skb, int type)
{
    switch(type)
    {
        case BEACON:{
                        struct geohdr_beacon *gehdr;
                        gehdr = (struct geohdr_beacon*)skb_push(skb, extend_header_len(type));
                        skb_reset_network_header(skb);
                        //set so_pv
                        break;
                    }
        case LSREQ:{
                       struct geohdr_lsreq *gehdr;
                       gehdr = (struct geohdr_lsreq*)skb_push(skb, extend_header_len(type));
                       skb_reset_transport_header(skb);
                       // set sn
                       gehdr->reserved = 0;
                       //set so_pv;
                       //set reuest GN_ADDR
                       break;
                   }
        case LSREP:{
                       struct geohdr_lsrep *gehdr;
                       gehdr = (struct geohdr_lsrep*)skb_push(skb, extend_header_len(type));
                       skb_reset_transport_header(skb);
                       //set sn
                       gehdr->reserved = 0;
                       //set so_pv;
                       //set de_pv;
                       break;
                   }
        default:
                   goto out;
                       //set sn
                       gehdr->reserved = 0;
                       //set so_pv;
                       //set de_pv;
                       break;
                   }
        default:
                   goto out;
    }
    return 0;
out:
    return -1;
}

static int common_header_create(struct sk_buff *skb, int type)
{
    int err;
    struct geohdr_common *gchdr;
    gchdr = (struct geohdr_common*)skb_push(skb, GEO_COMMON_HEADER_LEN);
    if(gchdr == NULL)
        goto out;
    skb_reset_network_header(skb);
    gchdr->nexthdr = 0;
    gchdr->reserved = 0;
    gchdr->reserved1 = 0;
    gchdr->pl = 0;
    gchdr->flags = itsGnIsMobile;
    gchdr->flags_reserved = 0;
    gchdr->scf = 0;
    gchdr->chan_offload = 0;
    gchdr->tc_id = 0;
    switch(type)
    {
        case BEACON:
            gchdr->hdrt = 1;
            gchdr->hdrst = 0;
            gchdr->mhl = 1;
            break;
        case LSREQ:
            gchdr->hdrt = 6;
            gchdr->hdrst = 0;
            gchdr->mhl = itsGnDefaultHopLimit;
            break;
        case LSREP:
            gchdr->hdrt = 6;
            gchdr->hdrst = 1;
            gchdr->mhl = itsGnDefaultHopLimit;
            break;
    }
    return 0;
out:
    return -1;
}

static int basic_header_create(struct sk_buff *skb, int type)
{
    struct geohdr_basic *gbhdr;
    gbhdr = (struct geohdr_basic*)skb_push(skb, GEO_BASIC_HEADER_LEN);
    if(gbhdr == NULL)
        goto out;
    skb_reset_network_header(skb);
    gbhdr->version = itsGnProtocolVersion;
    if(itsGnSecurity == 0)
        gbhdr->nexthdr = 1;
    else
        gbhdr->nexthdr = 2;
    gbhdr->reserved = 0;
    gbhdr->mul = 60;
    gbhdr->base = 1;
    if(type == BEACON)
        gbhdr->rhl = 1;
    else
        gbhdr->rhl = itsGnDefaultHopLimit;

    return 0;
out:
    return -1;
}

struct sk_buff *packet_create(int type)
{
    int len;
    struct sk_buff *skb;

    len = GEO_BASIC_HEADER_LEN + GEO_COMMON_HEADER_LEN + extend_header_len(type);
    skb = alloc_skb(len, GFP_KERNEL);
    if(!skb)
    {
        printk(KERN_INFO "Fail to alloc memory for skb\n");
        goto drop;
    }
    skb_reserve(skb, len);
    if(extend_header_create(skb, type) != 0)
        goto drop;
    if(common_header_create(skb, type) != 0)
        goto drop;
    if(basic_header_create(skb, type) != 0)
        goto drop;

    return skb;

drop:
    kfree_skb(skb);
    return NULL;


}
*/
