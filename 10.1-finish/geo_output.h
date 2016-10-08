#ifndef __GEO_OUTPUT_H
#define __GEO_OUTPUT_H

#include "geo.h"
#include "mib.h"
/*
#define pi 3.1415926
#define BEACON 1
#define GUC 2
#define GAC 3
#define GBC 4
#define SHB 5
#define TSB 6
#define LSREQ 7
#define LSREP 8
#define GEO_BASIC_HEADER_LEN 4
#define GEO_COMMON_HEADER_LEN 8


#define itsGnProtocolVersion 0
#define itsGnMAxSduSzie 1398
#define itsGnDeafultTrafficClass 0x00
#define itsGnDefaultHopLimit 10
#define itsGnMinPacketRepetitionInterval 100
#define itsGnMaxPacketLifetime 600
#define itsGnDefaultPacketLifetime 60
#define itsGnMaxGeoAreaSize 10
#define itsGnGeounicastForwardingAlogrithm 1
#define itsGnGeoAreaLineForwarding 1
#define itsGnGeoBroadcastForwardingAlogrithm 4
#define itsGnIsMobile 0//0 for stationary, 1 for Mobile
#define itsGnIfType 0// 0 for unspecified , 1 for its-g5
#define itsGnGnSecurity 0// 0 for unable,1 for able
#define itsGnSnDecapResulthandling 1// 0 for strict 1 for unstrict
*/

int geo_output(struct sk_buff *skb);
int gn_data_request_rcv(struct gn_request_param *params, struct sk_buff *skb);
struct sk_buff *packet_create(int type);

#endif
