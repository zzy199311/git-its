#include <linux/module.h>
#include <linux/kernel.h>

#include "geo.h"
#include "geo_input.h"
#include "geo_output.h"

void geo_init(void)
{
        printk(KERN_INFO "Create a geo module!\n");
/*
        printk(KERN_INFO "-------------------------------input_test_start----------------------------\n");
        //form a skb from mac
        char data[] = "this is a test packet from dev, to test if the function of geo_input could work!";
        int data_len = sizeof(data);
        char btp_hdr[] = "this is btp header";
        int btp_len = sizeof(btp_hdr);
        struct sk_buff *skb = 0;

        struct geohdr_basic *gbhdr = 0;
        int basic_len = sizeof(struct geohdr_basic);
        struct geohdr_common *gchdr = 0;
        int common_len = sizeof(struct geohdr_common);
        struct geohdr_shb *gehdr = 0;
        int extend_len = sizeof(struct geohdr_shb);

        int length = data_len + btp_len + sizeof(struct geohdr_basic) + sizeof(struct geohdr_common) + sizeof(struct geohdr_shb);
        printk(KERN_INFO "Total lenght of packet is %d\n",length);
        printk(KERN_INFO "app_len:%d\n", data_len);
        printk(KERN_INFO "btp_len:%d\n", btp_len);

        printk(KERN_INFO "extend_len:%d\n", extend_len);
        printk(KERN_INFO "common_len:%d\n", common_len);
        printk(KERN_INFO "basic_len:%d\n", basic_len);

        skb = alloc_skb(length, GFP_KERNEL);
        if(!skb)
            printk(KERN_INFO "fail to alloc memeoy for shb_pakcet\n");
        printk(KERN_INFO "succeed to alloc memory\n");
        printk(KERN_INFO "succeed to push, now write data to skb_data\n");
        skb_reserve(skb, length);

        unsigned char *p;
        p = skb_push(skb,data_len);
	    memcpy(p, &data[0], data_len);
        printk(KERN_INFO "App_data: %s\n", p);
        printk(KERN_INFO "App_pointer: %p\n", p);

	    p = skb_push(skb, btp_len);
	    memcpy(p, &btp_hdr[0], btp_len);

        skb_reset_transport_header(skb);
        //printk(KERN_INFO "Btp_data:%p\n", skb->transport_header);
        printk(KERN_INFO "Btp_data:%s\n", p);
        printk(KERN_INFO "Btp_pointer:%p\n", p);
        printk(KERN_INFO "skb->transport_header: %d\n", skb->transport_header);
        printk(KERN_INFO "skb->head:%p\n", skb->head);
        printk(KERN_INFO "skb->end:%p\n", skb->end);
        printk(KERN_INFO "skb_transport_header(=head+trans_header):%p\n",skb_transport_header(skb));
        //gehdr = (struct geohdr_shb*) skb_push(skb, extend_len);

        p = skb_push(skb, extend_len);
        gehdr = (struct geohdr_shb*) p;
        printk(KERN_INFO "extend_pointer:%p\n", p);
        //gchdr = (struct geohdr_common*) skb_push(skb, common_len);

        //set shb header
        unsigned char mac[6] = {0x01,0x02,0x03,0x04,0x05,0x06};
        memcpy(gehdr->so_pv.GN_ADDR.macaddr, mac, sizeof(mac));
        printk(KERN_INFO "The mac addr is :  ");
        int i=0;
        printk(KERN_INFO "%02x:%02x:%02x:%02x:%02x:%02x\n",
                gehdr->so_pv.GN_ADDR.macaddr[i],gehdr->so_pv.GN_ADDR.macaddr[i+1],gehdr->so_pv.GN_ADDR.macaddr[i+2],
                gehdr->so_pv.GN_ADDR.macaddr[i+3],gehdr->so_pv.GN_ADDR.macaddr[i+4],gehdr->so_pv.GN_ADDR.macaddr[i+5]);
        gehdr->so_pv.tst = 12396;
        gehdr->so_pv.latitude = 127623;
        gehdr->so_pv.longitude = 86218;
        gehdr->so_pv.pai = 1;
        gehdr->so_pv.speed = 7000;
        gehdr->so_pv.heading = 2700;
        gehdr->reserved = 0;


        p = skb_push(skb, common_len);
        gchdr = (struct geohdr_common*) p;
        printk(KERN_INFO "common_pointer: %p\n", gchdr);

	//set common header
        gchdr->nexthdr = 1;
        gchdr->reserved = 0;
        gchdr->hdrt = 5;
        gchdr->hdrst= 0;
        gchdr->scf = 1;
        gchdr->chan_offload = 1;
        gchdr->tc_id = 1;
        gchdr->flags = 0;
        gchdr->pl = btp_len + data_len;
        gchdr->mhl = 100;
        gchdr->reserved1 = 0i;
        printk(KERN_INFO "write data of common header successfully\n");

        //gbhdr = (struct geo_basic*) skb_push(skb, basic_len);
	    p = skb_push(skb, basic_len);
        gbhdr = (struct geohdr_basic*) p;
        //set basic header
        gbhdr->version = 0;
        gbhdr->nexthdr = 1;
        gbhdr->reserved = 0;
        gbhdr->mul = 32;
        gbhdr->base = 2;
        gbhdr->rhl = 18;
        printk(KERN_INFO "basic header:%p\n",p);
        printk(KERN_INFO "write data of basic header successfully\n");
        skb_reset_network_header(skb);
        printk(KERN_INFO "skb->network_header: %d\n", skb->network_header);
        printk(KERN_INFO "skb_network_header(=head+net_header):%p\n",skb_network_header(skb));

        //push net header
        //memcpy(skb_push(packet,extend_len), gehdr, extend_len);
        //memcpy(skb_push(packet,common_len), gchdr, common_len);
        //memcpy(skb_push(packet,basic_len), gbhdr, basic_len);
        //skb->mac.raw = skb->data;
        skb->pkt_type = PACKET_HOST;
        //skb->dev = dev;
        //skb->protocol = htons(ETH_P_ITS);
        int  err;
        err = geo_rcv(skb);
        if(err == -1)
            printk(KERN_INFO "packet input processing failed, discard the packet\n");
        if(err == 0)
            printk(KERN_INFO "packet input processing succeed, pass pyaload to BTP\n");
*/

	//--------------------output.c SHB test---------------------------

        printk(KERN_INFO "------------------------output_test_start----------------------------\n");
        char data[] = "this is a test packet from dev, to test if the function of geo_input could work!";
        int data_len = sizeof(data);
        char btp_hdr[] = "this is btp header";
        int btp_len = sizeof(btp_hdr);
        struct sk_buff *skb1 = 0;
        int length = data_len + btp_len + sizeof(struct geohdr_basic) + sizeof(struct geohdr_common) + sizeof(struct geohdr_shb);
        skb1 = alloc_skb(length, GFP_KERNEL);
	if(!skb1)
            printk(KERN_INFO "fail to alloc memeoy for output test pakcet\n");
        printk(KERN_INFO "succeed to alloc memory\n");
        printk(KERN_INFO "succeed to push, now write data to skb_data\n");
        skb_reserve(skb1, length);

	unsigned char *q;
	q = skb_push(skb1, data_len);
	    memcpy(q, &data[0], data_len);
        printk(KERN_INFO "App_data: %s\n", q);
        printk(KERN_INFO "App_pointer: %p\n", q);

	    q = skb_push(skb1, btp_len);
	    memcpy(q, &btp_hdr[0], btp_len);

        skb_reset_transport_header(skb1);
        //printk(KERN_INFO "Btp_data:%p\n", skb->transport_header);
        printk(KERN_INFO "Btp_data:%s\n", q);
        printk(KERN_INFO "Btp_pointer:%p\n", q);
        printk(KERN_INFO "skb->transport_header: %d\n", skb1->transport_header);
        printk(KERN_INFO "skb->head:%p\n", skb1->head);
        printk(KERN_INFO "skb->end:%p\n", skb1->end);
        printk(KERN_INFO "skb_transport_header(=head+trans_header):%p\n",skb_transport_header(skb1));

        struct gn_request_param *params;
        params = (struct gn_request_param*)kmalloc(sizeof(struct gn_request_param), GFP_KERNEL);
        params->uphrt = 0;
        params->secpro = 0;//unsecrued packet
        params->compro = 0;// unspecific
        params->ht = 5;//shb
        params->hst = 0;
        params->mul = 20;
        params->base = 3;
        params->repint= UNSET;
        params->max_reptime = UNSET;
        params->max_hoplimit = UNSET;
        params->scf = 0;
        params->chan_offload = 0;
        params->tc_id = 0;
        params->len = data_len;
        params->data = skb1->data;
        params->deaddr.m = 0;
        params->deaddr.st = 4;
        params->deaddr.scc = 2;
        params->deaddr.macaddr[0] = UNSET;


        params->de_po.gap_lat = 90000;
        params->de_po.gap_long = 156000;
        params->de_po.a = 2000;
        params->de_po.b = 1000;
        params->de_po.angle = 60;
        params->de_po.reserved = 0;

	int err = gn_data_request_rcv(params, skb1);
    if(err != 0)
        printk(KERN_INFO "ERROR: can't finish out put!\n");
    else
        printk(KERN_INFO "Succeed to run output!\n");

	//--------------------output.c TSB test---------------------------

        printk(KERN_INFO "------------------------output_test_start----------------------------\n");

        struct sk_buff *tsb = 0;
        int len_tsb = data_len + btp_len + sizeof(struct geohdr_basic) + sizeof(struct geohdr_common) + sizeof(struct geohdr_tsb);
        tsb = alloc_skb(len_tsb, GFP_KERNEL);
	if(!tsb)
            printk(KERN_INFO "fail to alloc memeoy for output test pakcet\n");
        printk(KERN_INFO "succeed to alloc memory\n");
        printk(KERN_INFO "succeed to push, now write data to skb_data\n");
        skb_reserve(tsb, length);

	unsigned char *p;
	p = skb_push(tsb, data_len);
	    memcpy(p, &data[0], data_len);
        printk(KERN_INFO "App_data: %s\n", p);
        printk(KERN_INFO "App_pointer: %p\n", p);

	    p = skb_push(tsb, btp_len);
	    memcpy(p, &btp_hdr[0], btp_len);

        skb_reset_transport_header(tsb);
        //printk(KERN_INFO "Btp_data:%p\n", skb->transport_header);
        printk(KERN_INFO "Btp_data:%s\n", p);
        printk(KERN_INFO "Btp_pointer:%p\n", p);
        printk(KERN_INFO "skb->transport_header: %d\n", tsb->transport_header);
        printk(KERN_INFO "skb->head:%p\n", tsb->head);
        printk(KERN_INFO "skb->end:%p\n", tsb->end);
        printk(KERN_INFO "skb_transport_header(=head+trans_header):%p\n",skb_transport_header(tsb));

        struct gn_request_param *tparams;
        tparams = (struct gn_request_param*)kmalloc(sizeof(struct gn_request_param), GFP_KERNEL);
        tparams->uphrt = 0;
        tparams->secpro = 0;//unsecrued packet
        tparams->compro = 0;// unspecific
        tparams->ht = 5;//shb
        tparams->hst = 1;
        tparams->mul = 30;
        tparams->base = 2;
        tparams->repint= UNSET;
        tparams->max_reptime = UNSET;
        tparams->max_hoplimit = UNSET;
        tparams->scf = 0;
        tparams->chan_offload = 0;
        tparams->tc_id = 0;
        tparams->len = data_len;
        tparams->data = tsb->data;
        tparams->deaddr.m = 0;
        tparams->deaddr.st = 3;
        tparams->deaddr.scc = 3;
        tparams->deaddr.macaddr[0] = UNSET;


        params->de_po.gap_lat = 90000;
        params->de_po.gap_long = 156000;
        params->de_po.a = 2000;
        params->de_po.b = 1000;
        params->de_po.angle = 60;
        params->de_po.reserved = 0;

	err = gn_data_request_rcv(tparams, tsb);
    if(err != 0)
        printk(KERN_INFO "ERROR: can't finish out put!\n");
    else
        printk(KERN_INFO "Succeed to run output!\n");

}

void geo_exit(void)
{
        printk(KERN_INFO "Exit geo module\n");
}


module_init(geo_init);
module_exit(geo_exit);
MODULE_LICENSE("GPL");
MODULE_ALIAS_NETPROTO(GeoNetworking);
