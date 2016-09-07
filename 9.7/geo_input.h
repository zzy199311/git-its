#ifndef __GEO_INPUT_H
#define __GEO_INPUT_H

#include <linux/types.h>
#include <linux/skbuff.h>
#include "geo.h"
/*
#define BEACON 1
#define GUC 2
#define GAC 3
#define GBC 4
#define SHB 5
#define TSB 6
#define LSREQ 7
#define LSREP 8

#define itsGnProtocolVersion 0
*/
int gn_data_indication(struct sk_buff *skb, struct geohdr_basic *gbhdr, struct geohdr_common *gchdr, int type);

int geo_rcv(struct sk_buff *skb/*, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev*/);

//int get_extend_header(struct geohdr_p *ghdr);

//int geo_rcv_finish(struct sk_buff *skb, struct geohdr_p *ghdr, int geo_hdrlen);

int geo_basic_handling(struct sk_buff *skb);

int geo_common_handling(struct sk_buff *skb, struct geohdr_basic *gbhdr);

int geo_secured_handling(struct sk_buff *skb, struct geohdr_basic *gbhdr);

int geo_beacon_rcv_handling(struct sk_buff *skb, struct geohdr_basic *gbhdr, struct geohdr_common *gchdr);

int geo_shb_rcv_handling(struct sk_buff *skb, struct geohdr_basic *gbhdr, struct geohdr_common *gchdr);

int geo_tsb_rcv_handling(struct sk_buff *skb, struct geohdr_basic *gbhdr, struct geohdr_common *gchdr);

int btp_rcv(struct gn_indication_param *gn_indication_params, struct sk_buff *skb);



#endif
