
#ifndef _GEO_H
#define _GEO_H

#include <linux/types.h>
#include <linux/skbuff.h>


struct gn_addr
{
		uint16_t	m:1,
					st:5,
					scc:10;
		unsigned char	macaddr[6];
};

struct longpv
{
		struct gn_addr GN_ADDR;
		uint32_t	tst;
		int32_t		latitude;
		int32_t		longitude;
		int16_t		pai:1,
					speed:15;
		uint16_t	heading;
};

struct shortpv
{
		struct gn_addr	GN_ADDR;
		uint32_t	tst;
		int32_t 	latitude;
		int32_t		longitude;	
};

struct geohdr_basic{
		unsigned char	version:4,
						nexthdr:4;
		unsigned char	reserved;
		unsigned char	it;
		unsigned char	rhl;
};

struct geohdr_common
{
		unsigned char	nexthdr:4,
						reserved:4;
		unsigned char	hdrt:4,
						hdrst:4;
		unsigned char	tc;		/* traffic class -- |1|1|6| -- |SCF|Channel Offload|ID| -- 4-1 use default, itsGnDeafultTrafficClass*/
		unsigned char	flags;	/* mobile or stationary -- itsGnIsMobile -- */
		uint32_t	pl;			/* length of payload (ie: BTP + CAM) */
		unsigned long	mhl;	/* Max limited hops */
		unsigned long	reserved1;
};


struct geohdr_tsb
{
		uint16_t	sn;
		uint16_t	reserved;
		struct	longpv	so_pv;
};

struct geohdr_shb
{
		struct	longpv	so_pv;
		uint32_t	reserved;
};

struct geohdr_gabc
{
		uint16_t	sn;
		uint16_t	reserved;
		struct	longpv	so_pv;
		struct	shortpv	de_pv;
};

struct geohdr_guc
{
		uint16_t	sn;
		uint16_t	reserved;
		struct	longpv	so_pv;
		struct	shortpv	de_pv;
};

struct geohdr_beacon
{
		struct	longpv	so_pv;
};

struct geohdr_lsreq
{
		uint16_t	sn;
		uint16_t	reserved;
		struct	longpv	so_pv;
		struct	gn_addr	rgnaddr;
};

struct geohdr_lsrep
{
		uint16_t	sn;
		uint16_t	reserved;
		struct	longpv	so_pv;
		struct	shortpv	de_pv;
};

struct geo_send_param
{
		//unsigned char psid[PSID_LEN];
		unsigned char channel_num;
		unsigned char data_rate;
		unsigned char tx_power_level;
		unsigned char priority;
		uint64_t	  expire_time;
		unsigned char ext_flag; //don't know 
		unsigned char element_id; //dont' know
};

struct geohdr_p
{
		struct	geohdr_basic	*gbhdr;
		struct	geohdr_common	*gchdr;
		void	*extend;
};

/*
static inline struct geohdr *geo_hdr(const struct sk_buff *skb)
{
		return (struct geohdr *)skb_network_header(skb);
}*/

static int  get_extend_header(struct geohdr_p *ghdr)
{
		int found;
		struct geohdr_p *tmp;
		tmp = ghdr;
		switch(ghdr->gchdr->hdrt){
				case 0:
						printk(KERN_NOTICE"GEO_DEBUG: Extend header is ANY\n");
						found = -1;
						break;
				case 1:
						printk(KERN_NOTICE"GEO_DEBUG: Extend header is BEACON\n");
						ghdr->extend = (struct geohdr_beacon*)(tmp->extend);
						found = sizeof(struct geohdr_beacon);
						break;
				case 2:
						printk(KERN_NOTICE"GEO_DEBUG: Extend header is GUC\n");
						ghdr->extend = (struct geohdr_guc*)(tmp->extend);
						found = sizeof(struct geohdr_guc);
						break;
				case 3:
						printk(KERN_NOTICE"GEO_DEBUG: Extend header is GAC\n");
						ghdr->extend = (struct geohdr_gabc*)(tmp->extend);
						found = sizeof(struct geohdr_gabc);
						break;
				case 4:
						printk(KERN_NOTICE"GEO_DEBUG: Extend header is GBC\n");
						ghdr->extend = (struct geohdr_gabc*)(tmp->extend);
						found = sizeof(struct geohdr_gabc);
						break;
				case 5:
						printk(KERN_NOTICE"GEO_DEBUG: Extend header is TSB\n");
						ghdr->extend = (struct geohdr_tsb*)(tmp->extend);
						found = sizeof(struct geohdr_tsb);
						break;
				case 6:
						switch(ghdr->gchdr->hdrst){
								case 0:
										printk(KERN_NOTICE"GEO_DEBUG: Extend header is LSrequest\n");
										ghdr->extend = (struct geohdr_lsreq*)(tmp->extend);
										found = sizeof(struct geohdr_lsreq);
										break;
								case 1:
										printk(KERN_NOTICE"GEO_DEBUG: Extend header is LSreply\n");
										ghdr->extend = (struct geohdr_lsrep*)(tmp->extend);
										found = sizeof(struct geohdr_lsrep);
										break;
								default:
										found = -1;
										break;
						}
				default:
						found = -1;
						break;
		}
		return found;
}

//static int isdst(struct sk_buff *skb, 


#endif
