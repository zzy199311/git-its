#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/delay.h>

#include "mib.h"
#include "geo.h"
#include "geo_input.h"
#include "geo_output.h"

#define AUTO        0
#define MANAGED     1
#define ANONYMOUS   2

#define CBF         2
#define GREEDY      1

#define SIMPLE      1
#define ADVANCED    3

#define DISABLED    0
#define ENABLED     1

#define Stationary  0
#define Mobile      1

#define UNSPECIFIED 0
#define ITS_G5      1

#define Unknown     0
#define Pedestrian  1
#define Cyclist     2
#define Moped       3
#define Motorcycle  4
#define PassengerCar    5
#define Bus         6
#define LightTruck  7
#define HeavyTruck  8
#define Trailer     9
#define SpecialVehicles 10
#define Tram        11
#define RoadSideUnit    15

#define STRICT      0
#define NON_Strict  1

struct timer_list *life_timer[TIMER_NUM];
struct itsGnIfTable itsGnTable[MIB_HASH_NUM];
struct itsGnMgmt itsLocalInfo;



static void mib_packet_forwadring_init(struct itsGnMgmt *itsLocalInfo)
{
    itsLocalInfo->PacketForwarding.itsGnDefaultHopLimit = 10;
    itsLocalInfo->PacketForwarding.itsGnMaxPacketLifetime = 600;
    itsLocalInfo->PacketForwarding.itsGnDefaultPacketLifetime = 60;
    itsLocalInfo->PacketForwarding.itsGnMaxPacketDataRate = 100;
    itsLocalInfo->PacketForwarding.itsGnMaxPacketDataRateEmaBeta = 90;
    itsLocalInfo->PacketForwarding.itsGnMaxGeoAreaSize = 10;
    itsLocalInfo->PacketForwarding.itsGnMinPacketRepetitionInterval = 100;
    itsLocalInfo->PacketForwarding.itsGnGeoUnicastForwardingAlgorithm = GREEDY;
    itsLocalInfo->PacketForwarding.itsGnGeoBroadcastForwardingAlgorithm = ADVANCED;
    itsLocalInfo->PacketForwarding.itsGnGeoUnicastCbfMinTime = 1;
    itsLocalInfo->PacketForwarding.itsGnGeoUnicastCbfMaxTime = 100;
    itsLocalInfo->PacketForwarding.itsGnGeoBroadcastCbfMinTime = 1;
    itsLocalInfo->PacketForwarding.itsGnGeoBroadcastCbfMaxTime = 100;
    itsLocalInfo->PacketForwarding.itsGnDefaultMaxCommunicationRange = 1000;
    itsLocalInfo->PacketForwarding.itsGnGeoAreaLineForwarding = ENABLED;
    itsLocalInfo->PacketForwarding.itsGnUcForwardingPacketBufferSize = 256;
    itsLocalInfo->PacketForwarding.itsGnBcForwardingPacketBufferSzie = 1024;
    itsLocalInfo->PacketForwarding.itsGnCbfPacketBufferSize = 256;
    itsLocalInfo->PacketForwarding.itsGnDefaultTrafficClass = 0x00;
}
static void mib_beacon_service_init(struct itsGnMgmt *itsLocalInfo)
{
    itsLocalInfo->BeaconService.itsGnBeaconServiceRetransmitTimer = 3000;
    itsLocalInfo->BeaconService.itsGnBeaconServiceMaxJitter = 750;// = RetransmitTimer/4
}
static void mib_location_service_init(struct itsGnMgmt *itsLocalInfo)
{
    itsLocalInfo->LocationService.itsGnLocationServiceMaxRetrans = 10;
    itsLocalInfo->LocationService.itsGnLocationServiceRetransmitTimer = 1000;
    itsLocalInfo->LocationService.itsGnLocationServicePacketBufferSize = 1024;
}
static void mib_local_config_init(struct itsGnMgmt *itsLocalInfo, int method, int32_t station, int ismobile, int safe, int sndecap)
{
    memset(&itsLocalInfo->LocalConfig.itsGnLPV.GN_ADDR, 0, sizeof(struct gn_addr));
    itsLocalInfo->LocalConfig.itsGnLPV.GN_ADDR.macaddr[5] = 1;//get_mac_from device()
    itsLocalInfo->LocalConfig.itsGnLocalAddrConfMethod = method;
    itsLocalInfo->LocalConfig.itsGnProtocolVersion = 0;// 0 for EN 302 636-4-1 (V1.2.1)
    itsLocalInfo->LocalConfig.itsGnStationType = station;
    itsLocalInfo->LocalConfig.itsGnIsMobile = ismobile;
    itsLocalInfo->LocalConfig.itsGnIfType = UNSPECIFIED;
    itsLocalInfo->LocalConfig.itsGnMinUpdateFrequencyLPV = 1000;
    itsLocalInfo->LocalConfig.itsGnPaiInterval = 80;
    itsLocalInfo->LocalConfig.itsGnMaxSduSize = 1398;
    itsLocalInfo->LocalConfig.itsGnMaxGeoNetworkingHeaderSize = 88;
    itsLocalInfo->LocalConfig.itsGnLifetimeLocTE = 20;
    itsLocalInfo->LocalConfig.itsGnSecurity = safe;
    itsLocalInfo->LocalConfig.itsGnSnDecapeResultHandling = sndecap;
}


static void __mib_init_hash_table(struct itsGnIfTable *entry)
{
    INIT_HLIST_HEAD(&entry->head);
}

static void mib_init_hash_table(struct itsGnIfTable itsGnTable[MIB_HASH_NUM])
{
    unsigned int i;
    for(i = 0; i < MIB_HASH_NUM; i++)
        __mib_init_hash_table(&itsGnTable[i]);
	printk(KERN_INFO "Init hash table!\n");
}



void sta_init(void)
{
	
        printk(KERN_INFO "Create a station's geo module!\n");
        int err;
		uint64_t hash;
        memset(life_timer, 0, sizeof(struct timer_list*) * TIMER_NUM);
	    mib_local_config_init(&itsLocalInfo, 0, 6, Mobile, DISABLED, STRICT);
	    mib_location_service_init(&itsLocalInfo);
	    mib_beacon_service_init(&itsLocalInfo);
	    mib_packet_forwadring_init(&itsLocalInfo);
        mib_init_hash_table(&itsGnTable);
        printk(KERN_INFO "local station 's mac = %02x:%02x:%02x:%02x:%02x:%02x\n",
               itsLocalInfo.LocalConfig.itsGnLPV.GN_ADDR.macaddr[0],itsLocalInfo.LocalConfig.itsGnLPV.GN_ADDR.macaddr[1],itsLocalInfo.LocalConfig.itsGnLPV.GN_ADDR.macaddr[2],
               itsLocalInfo.LocalConfig.itsGnLPV.GN_ADDR.macaddr[3],itsLocalInfo.LocalConfig.itsGnLPV.GN_ADDR.macaddr[4],itsLocalInfo.LocalConfig.itsGnLPV.GN_ADDR.macaddr[5]);

/*------------------create a shb packet-----------------

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

        skb = alloc_skb(length, GFP_KERNEL);
        if(!skb)
            printk(KERN_INFO "fail to alloc memeoy for shb_pakcet\n");
        printk(KERN_INFO "succeed to alloc memory\n");
        printk(KERN_INFO "succeed to push, now write data to skb_data\n");
        skb_reserve(skb, length);

        unsigned char *p;
        p = skb_push(skb,data_len);
	    memcpy(p, &data[0], data_len);
        //printk(KERN_INFO "App_data: %s\n", p);
        //printk(KERN_INFO "App_pointer: %p\n", p);

	    p = skb_push(skb, btp_len);
	    memcpy(p, &btp_hdr[0], btp_len);

        skb_reset_transport_header(skb);
        //printk(KERN_INFO "Btp_data:%p\n", skb->transport_header);
        //printk(KERN_INFO "Btp_data:%s\n", p);
        //printk(KERN_INFO "Btp_pointer:%p\n", p);
        //printk(KERN_INFO "skb->transport_header: %d\n", skb->transport_header);
        //printk(KERN_INFO "skb->head:%p\n", skb->head);
        //printk(KERN_INFO "skb->end:%p\n", skb->end);
        //printk(KERN_INFO "skb_transport_header(=head+trans_header):%p\n",skb_transport_header(skb));

        p = skb_push(skb, extend_len);
        gehdr = (struct geohdr_shb*) p;
        //printk(KERN_INFO "extend_pointer:%p\n", p);

        //set shb header
        unsigned char mac[6] = {0x01,0x02,0x03,0x04,0x05,0x06};
        memcpy(gehdr->so_pv.GN_ADDR.macaddr, mac, sizeof(mac));
        printk(KERN_INFO "SHB packet's source mac = %02x:%02x:%02x:%02x:%02x:%02x\n",
                gehdr->so_pv.GN_ADDR.macaddr[0],gehdr->so_pv.GN_ADDR.macaddr[1],gehdr->so_pv.GN_ADDR.macaddr[2],
                gehdr->so_pv.GN_ADDR.macaddr[3],gehdr->so_pv.GN_ADDR.macaddr[4],gehdr->so_pv.GN_ADDR.macaddr[5]);
        gehdr->so_pv.tst = 12396;
        gehdr->so_pv.latitude = 127623;
        gehdr->so_pv.longitude = 86218;
        gehdr->so_pv.pai = 1;
        gehdr->so_pv.speed = 7000;
        gehdr->so_pv.heading = 2700;
        gehdr->reserved = 0;


        p = skb_push(skb, common_len);
        gchdr = (struct geohdr_common*) p;
        //printk(KERN_INFO "common_pointer: %p\n", gchdr);

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
        //printk(KERN_INFO "basic header:%p\n",p);
        printk(KERN_INFO "write data of basic header successfully\n");
        skb_reset_network_header(skb);

		int flag = 0;
		hash = hash_compute(gehdr->so_pv.GN_ADDR.macaddr);
        //执行DAD操作
        err = dup_addr_detect(gehdr->so_pv);
        if(err != 0){
            printk(KERN_INFO "The packet sender's address is same with local address!\n");
        }

		struct itsGnIfEntry *entry;
		entry = get_entry_by_index(hash, gehdr->so_pv.GN_ADDR.macaddr);

		if(entry == NULL)
		{
			printk(KERN_INFO "No such entry!\n");
			entry = add_mib_entry(hash,gehdr->so_pv.GN_ADDR.macaddr);
			flag = 0;
		}
		else{
			printk(KERN_INFO "Already have such entry!\n");
 			err = dpd_single(gehdr->so_pv.tst, entry);
       	 	if(err != 0){
            	printk(KERN_INFO "The packet is a duplicate packet\n");
            	//return err;
        }

        	flag =1;
		}

		err= update_mib_entry(entry, gbhdr, gchdr, SHB, flag);
		if(err == 0)
			printk(KERN_INFO "Mib entry update sucesseed.\n");
		else
			printk(KERN_INFO "Mib entry update failed.\n");
		printk(KERN_INFO "entry->item.itsGnIfIndex = %lld.\n",entry->item.itsGnIfIndex);
		printk(KERN_INFO "entry->item.version = %d.\n",entry->item.version);
		printk(KERN_INFO "entry->item.IS_NEIGHBOUR = %d.\n",entry->item.IS_NEIGHBOUR);
		printk(KERN_INFO "entry->item.pv.tst = %d.\n",entry->item.pv.tst);
		printk(KERN_INFO "entry->item.pv.latitude = %d.\n",entry->item.pv.latitude);
		printk(KERN_INFO "entry->item.pv.longitude = %d.\n",entry->item.pv.longitude);
    	err = pdr_update(entry, 150);
		printk(KERN_INFO "entry->item.pdr = %d.\n",entry->item.pdr);

		//第二轮测试，8s后
		mdelay(1000*8);
        gehdr->so_pv.tst = 12900;
        gehdr->so_pv.latitude = 130000;
        gehdr->so_pv.longitude = 88000;
        gehdr->so_pv.pai = 1;
        gehdr->so_pv.speed = 5600;
        gehdr->so_pv.heading = 2813;
		entry = get_entry_by_index(hash, gehdr->so_pv.GN_ADDR.macaddr);
		if(entry == NULL)
		{
			printk(KERN_INFO "No such entry!\n");
			entry = add_mib_entry(hash,gehdr->so_pv.GN_ADDR.macaddr);
			flag = 0;
		}
		else{
			printk(KERN_INFO "Already have such entry!\n");
 			err = dpd_single(gehdr->so_pv.tst, entry);
       	 	if(err != 0){
            	printk(KERN_INFO "The packet is a duplicate packet\n");
            	//return err;
        	}

        	//执行DAD操作
        	err = dup_addr_detect(gehdr->so_pv);
        	if(err != 0){
            	printk(KERN_INFO "The packet sender's address is same with local address!\n");
            	//return err;
        	}
        	flag =1;
		}
		
		err= update_mib_entry(entry, gbhdr, gchdr, SHB, flag);
		if(err == 0)
			printk(KERN_INFO "Mib entry update sucesseed.\n");
		else
			printk(KERN_INFO "Mib entry update failed.\n");
		printk(KERN_INFO "entry->item.itsGnIfIndex = %lld.\n",entry->item.itsGnIfIndex);
		printk(KERN_INFO "entry->item.version = %d.\n",entry->item.version);
		printk(KERN_INFO "entry->item.IS_NEIGHBOUR = %d.\n",entry->item.IS_NEIGHBOUR);
		printk(KERN_INFO "entry->item.pv.tst = %d.\n",entry->item.pv.tst);
		printk(KERN_INFO "entry->item.pv.latitude = %d.\n",entry->item.pv.latitude);
		printk(KERN_INFO "entry->item.pv.longitude = %d.\n",entry->item.pv.longitude);
    	err = pdr_update(entry, 150);
		printk(KERN_INFO "entry->item.pdr = %d.\n",entry->item.pdr);
		
*/

	//--------------------output.c SHB test---------------------------

        printk(KERN_INFO "------------------------output_test_start----------------------------\n");
        char data[] = "this is a test packet from dev, to test if the function of geo_input could work!";
        int data_len = sizeof(data);
        printk(KERN_INFO "The length of data is %d.\n", data_len);
        char btp_hdr[] = "this is btp header";
        int btp_len = sizeof(btp_hdr);
        printk(KERN_INFO "The length of btp is %d.\n", btp_len);
        struct sk_buff *skb1 = NULL;
        int length = data_len + btp_len + sizeof(struct geohdr_basic) + sizeof(struct geohdr_common) + sizeof(struct geohdr_shb);
        printk(KERN_INFO "The length of packet is %d.\n", length);
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

    err = gn_data_request_rcv(params, skb1);
    if(err != 0)
        printk(KERN_INFO "ERROR: can't finish out put!\n");
    else
        printk(KERN_INFO "Succeed to run output!\n");


}


void sta_exit(void)
{
	printk(KERN_INFO "Exit geo module\n");
	mdelay(1000*10);
}


module_init(sta_init);
module_exit(sta_exit);
MODULE_LICENSE("GPL");
//MODULE_ALIAS_NETPROTO(GeoNetworking);
