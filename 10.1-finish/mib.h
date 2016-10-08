#ifndef __MIB_H
#define __MIB_H

#include "geo.h"

#define TRUE 0
#define FALSE 1
#define MIB_HASH_NUM 128
#define SN_MAX 65535
#define TST_MAX 4294967295LL
#define TIMER_NUM 4096


struct locationentry{
    uint64_t    itsGnIfIndex;
    uint32_t    itsGnIfPriority;       //priority configured to interface;
    uint32_t    itdGnIfDescription;    //type:snmpAdminString
    uint32_t    version;
    struct      longpv   pv;
    struct      geo_area_position   gap;
    uint8_t     LS_PENDING;
    uint8_t     IS_NEIGHBOUR;
    uint16_t    sn;
    uint16_t    pdr;
    uint16_t    pdr_p;
    unsigned long   create_time;
    uint32_t    octet_count;
    int         timer_id;
};

struct itsGnIfEntry{
    struct      hlist_node      node;
    struct      locationentry   item;
};

struct itsGnIfTable{
    struct      hlist_head      head;
};

struct itsGnConfig{
    //struct      gn_addr itsGnLocalGnAddr;           //GN addr of router
    struct      longpv     itsGnLPV;
    int         itsGnLocalAddrConfMethod;           //0 for auto, 1 for managed, 2 for anonyous; deafult is 2
    uint32_t    itsGnProtocolVersion;               //GeoNetworking Protocol Version
    int32_t     itsGnStationType;                   // 0~11, 15;
    int         itsGnIsMobile;                      // TRUEVALUE
    int32_t     itsGnIfType;                        // 0 for unspecified, 1 for its-g5
    int32_t     itsGnMinUpdateFrequencyLPV;         // ms
    int32_t     itsGnPaiInterval;                   // 0~100; unit :meters; teh distanced related to Longitude and latitude
    int32_t     itsGnMaxSduSize;                    //
    int32_t     itsGnMaxGeoNetworkingHeaderSize;
    int32_t     itsGnLifetimeLocTE;                 // unit: seconds
    int         itsGnSecurity;                      //0 for disvaled; 1 for enabled
    int         itsGnSnDecapeResultHandling;        // 0 for strict ; 1 for non-strict

};

struct itsGnLocationService{
    int32_t     itsGnLocationServiceMaxRetrans;         //0~255 for lsrequest
    int32_t     itsGnLocationServiceRetransmitTimer;    //0~65535 ms
    int32_t     itsGnLocationServicePacketBufferSize;   // size of ls packet
};

struct itsGnBeaconService{
    int32_t     itsGnBeaconServiceRetransmitTimer;      //ms
    int32_t     itsGnBeaconServiceMaxJitter;            //ms
};

struct itsGnPacketForwarding{
    int32_t     itsGnDefaultHopLimit;               //
    int32_t     itsGnMaxPacketLifetime;             //0~6300 seconds; Upper limit of the max Lifetime  of a packet
    int32_t     itsGnDefaultPacketLifetime;         //0~6300 seconds;
    int32_t     itsGnMaxPacketDataRate;             //ko/s ; if EMA > this , stop forwarding;
    int32_t     itsGnMaxPacketDataRateEmaBeta;      //units: %; weight factoer for EMA
    int32_t     itsGnMaxGeoAreaSize;                 //km^2;
    int32_t     itsGnMinPacketRepetitionInterval;   //0~1000; lower limit of the packet repetition interval
    int         itsGnGeoUnicastForwardingAlgorithm; //0 for unspecified; 1 for greedy; 2 for cbf;
    int         itsGnGeoBroadcastForwardingAlgorithm;   //0 for unspecified; 1 for simple; 2 for cbf;
    int32_t     itsGnGeoUnicastCbfMinTime;          //0~65535  the time be buffered of cbf packet;
    int32_t     itsGnGeoUnicastCbfMaxTime;
    int32_t     itsGnGeoBroadcastCbfMinTime;        //0~65535
    int32_t     itsGnGeoBroadcastCbfMaxTime;        //
    int32_t     itsGnDefaultMaxCommunicationRange;  //0~65535 m
    int         itsGnGeoAreaLineForwarding;         //0 for disbaled ; 1 for enabled;
    int32_t     itsGnUcForwardingPacketBufferSize;  //Size of UC forwarding packet buffer in ko;
    int32_t     itsGnBcForwardingPacketBufferSzie;  //
    int32_t     itsGnCbfPacketBufferSize;           //0~65535 Size of CBF packet buffer;
    int32_t     itsGnDefaultTrafficClass;                  // Default traffic class;

};

struct itsGnMgmt
{
    struct  itsGnConfig              LocalConfig;
    struct  itsGnLocationService     LocationService;
    struct  itsGnBeaconService       BeaconService;
    struct  itsGnPacketForwarding    PacketForwarding;
};



/*
struct timer_info
{
	uint64_t index;
	int 	 timer_id;
	unsigned char mac[6];
};
*/

extern struct timer_list *life_timer[TIMER_NUM];
extern struct itsGnIfTable itsGnTable[MIB_HASH_NUM];
extern struct itsGnMgmt    itsLocalInfo;

uint16_t sn_compute(uint16_t sn_p);
int dpd_mul(uint16_t sn_p, uint32_t tst_p, struct itsGnIfEntry *entry);
int dpd_single(uint32_t tst_p, struct itsGnIfEntry *entry);
int data_rate_control(struct itsGnIfEntry *entry);
int pdr_update(struct itsGnIfEntry *entry, uint32_t len);
int dup_addr_detect(struct longpv so_pv);
int geo_area_size_control(struct geohdr_common *gchdr, struct geohdr_gabc *gehdr);
int update_mib_entry(struct itsGnIfEntry *entry, struct geohdr_basic *gbhdr, struct geohdr_common *gchdr, int type, int flag);
void packet_pv_update(struct itsGnIfEntry *entry, struct geohdr_common* gchdr, uint32_t tst_fp, int type);
struct itsGnIfEntry *add_mib_entry(uint64_t index, unsigned char mac[6]);
uint64_t hash_compute(unsigned char mac[6]);
struct itsGnIfEntry *get_entry_by_index(uint64_t pindex, unsigned char mac[6]);
void del_mib_entry(struct itsGnIfEntry *entry);
#endif
