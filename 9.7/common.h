#ifndef _COMMON_H
#define _COMMON_H

#include <linux/types.h>
#include <linux/skbuff.h>
#include <uapi/linux/if_packet.h>

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
						tc_id:6;		/* traffic class -- |1|1|6| -- |SCF|Channel Offload|ID| -- 4-1 use default, itsGnDeafultTrafficClass*/
		unsigned char	flags;	/* mobile or stationary -- itsGnIsMobile -- */
		uint32_t	pl;			/* length of payload (ie: BTP + CAM) */
		unsigned long	mhl;	/* Max limited hops */
		unsigned long	reserved1;
};

struct geohdr_p
{
		struct	geohdr_basic	*gbhdr;
		struct	geohdr_common	*gchdr;
		void	*extend;
};

/*---------------extend header part------------------*/
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
		/*
		uint32_t	gap_lat;
		uint32_t	gap_long;
		uint16_t	d_a;
		uint16_t	d_b;
		uint16_t	angle;
		*/
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

/*-------------------send param part------------------------*/

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

//get from btp with skb
struct gn_request_param
{
		uint8_t	uphrt:4,
				compro:2,
				secpro:2;//upper protocol | communication profile | security profile
		uint8_t ht:4,
				hst:4;//transport type
		struct gn_addr deaddr;
		uint8_t	mul:6,
				base:2;//max packet lifetime = mulipier | base
		uint16_t repint;//repetition interval
		uint8_t	max_reptime;//max repetition time
		uint8_t	max_hoplimit;
		uint8_t	scf:1,
				chan_offload:1,
				tc_id:6;//traffic class
		uint16_t len;//length of data(paload pf btp, ex:BTP-SDU)
};

// response to gn_request
struct gn_confirm_param
{
		uint8_t result;
		//1 for accept successfully
		//then all are failure
		//2 for sdu > msxsdusize
		//3 for lifetime expered
		//4 for repetition interval < gnconstant
		//5 for unsupported traffic class
		//6 for geo area size exceed gnconstant
		//7 for other reason
};
/*--------------------GN part end------------------------*/


#endif
