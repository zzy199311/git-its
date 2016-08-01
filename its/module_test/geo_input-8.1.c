/*
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
*/
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



#include "geo.h"
#define GEO_BASIC_HEADER_LEN 32
#define GEO_COMMON_HEADER_LEN 64
#define itsGnProtocolVersion 1

static int geo_rcv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev)
{
//		unsigned char * ghdr;

		int len;
		int found;

		//printk("GEO_DEBUG: geo_rcv before skb -> %d\n", skb->len);
		if(skb->pkt_type == PACKET_OTHERHOST){
				printk(KERN_NOTICE"GEO_DEBUG: geo_rcv: packet is to other host\n"); 
				goto drop;
		}


		if((skb = skb_share_check(skb, GFP_ATOMIC)) == NULL)
				goto out;

/*		if(!pskb_may_pull(skb, sizeof(struct ghdr))){
				printk(KERN_NOTICE"GEO_DEBUG: geo_rcv: packet is too short\n");
				goto drop;
		}
*/
		skb_reset_network_header(skb);//set pointer to header

		struct geohdr_p* ghdr = (struct geohdr_p*)skb_network_header(skb);//geohdr point to geohdr


		if(ghdr->gbhdr->version != itsGnProtocolVersion){
				printk(KERN_NOTICE"GEO_DEBUG: geo_rcv: wrong geo version\n");
				goto drop;
		}
		
		found = get_extend_header(ghdr) ;
		if(found == -1){
				printk(KERN_NOTICE"GEO_DEBUG: geo_rcv: fail to get extend hdr\n");
				goto out;
		}

		len = ntohs(sizeof(struct geohdr_basic) + sizeof(struct geohdr_common) + found);
		printk("GEO_DEBUG: geo_rcv geo header length = %d\n", len);

		if(skb->len < len)
				goto drop;
		//total len of geo->packet, pl = payload
		skb_reset_transport_header(skb);

		skb_orphan(skb);
		//return geo_rcv_finish(skb, &ghdr, len);

drop:
		kfree_skb(skb);
out:
		return NET_RX_DROP;
}



static int geo_rcv_finish(struct sk_buff *skb, struct geohdr_p *ghdr, int geo_hdrlen)
{
		printk("GEO_DEBUG: geo_rcv_finish\n");

		// now skb->data point to geo data, header is skiped 
		__skb_pull(skb, geo_hdrlen);

		int err = geo_packet_handling(skb, ghdr);

		if(!dst(skb))
		{
				// routing....to do....
				int err = geo_route_input(skb, )



		}
}
/*
static int geo_packet_handling(struct sk_buff *skb, struct geohdr_p *ghdr)
{
		int err;
		if(ghdr->gbhdr->version != isGnProtocolVersion)
				goto drop;
		switch(ghdr->gbhdr->nexthdr)
		{
				case 0:
						err = geo_common_handling(skb, ghdr);
						break;
				case 1:
						err = geo_common_handling(skb, ghdr);
						break;
				case 2:
						err = geo_secured_handling(skb, ghdr);
						break;
				default:
						err = -1;
						break;
		}
		if(err != 1)
				goto drop;
		return 1;
drop:
		kfree(skb);
}

static int geo_common_handling(struct sk_buff *skb, struct geohdr_p *ghdr)
{
		int err;
		if(ghdr->gchdr->mhl < ghdr->gbhdr->rhl)
				goto drop;
		// bc forwarding buffer is not empty, flush it -- to do....

		switch(ghdr->gchdr->hdrt)
		{
				case 0:	
						err = -1;
						break;
				case 1:
						err = geo_beacon_rcv_handling(skb, ghdr);
						break;
				case 2:
						err = geo_guc_rcv_handling(skb, ghdr);
						break;
				case 3:
						err = geo_gac_rcv_handling(skb, ghdr);
*/
