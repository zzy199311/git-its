
#ifndef _GEO_H
#define _GEO_H

#include <linux/types.h>
#include <linux/skbuff.h>
#include <uapi/linux/if_packet.h>

//static int geo_rcv_finish(struct sk_buff *skb, struct geohdr_p *ghdr, int geo_hdrlen);

//#include "geo_input.h"
struct gn_addr
{
		uint16_t	m:1,
					st:5,
					scc:10;
		unsigned char	macaddr[6];
};

struct geo_area_position
{
		uint32_t	gap_lat;
		uint32_t	gap_long;
		uint16_t	a;
		uint16_t	b;
		uint16_t	angle;
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
		unsigned char	mul:6,
                        base:2;
		unsigned char	rhl;
};

struct geohdr_common
{
		unsigned char	nexthdr:4,
						reserved:4;
		unsigned char	hdrt:4,
						hdrst:4;
		unsigned char	scf:1,
						chan_offload:1,
						tc_id:6;		//traffic class -- |1|1|6| -- |SCF|Channel Offload|ID| -- 4-1 use default, itsGnDeafultTrafficClass
		unsigned char	flags;	// mobile or stationary -- itsGnIsMobile --
		uint32_t	pl;			// length of payload (ie: BTP + CAM)
		unsigned long	mhl;	//Max limited hops
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
		struct  geo_area_position gap;

		//uint32_t	gap_lat;
		//uint32_t	gap_long;
		//uint16_t	d_a;
		//uint16_t	d_b;
		//uint16_t	angle;

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



struct gn_indication_param
{
		uint8_t uphrt:4,
				reserved:4;
		uint8_t ht:4,
				hst:4;//transport type
		struct gn_addr deaddr;
		struct geo_area_position de_po;
		struct longpv so_pv;
		uint8_t scf:1,
				chan_offload:1,
				tc_id:6;//traffic class
		uint8_t rhl;//remain hop limit
		uint16_t rlt;//remain pakcet lifetime,to do...
		// todo..all from sn-decap.confirm
		//uint8_t report:4,
		//id = ?
		//permissions = ?
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



#endif
