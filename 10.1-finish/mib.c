#include <linux/jiffies.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/hash.h>
#include <linux/timer.h>
#include <linux/timex.h>
#include <linux/rtc.h>
#include <linux/hash.h>
#include <linux/types.h>
#include <linux/rwlock.h>
#include <linux/spinlock.h>

#include "mib.h"

extern struct timer_list *life_timer[TIMER_NUM];
extern struct itsGnIfTable itsGnTable[MIB_HASH_NUM];
extern struct itsGnMgmt    itsLocalInfo;

static void print_time(void)
{
	struct timex txc;
	struct rtc_time tm;
	do_gettimeofday(&(txc.time));
	rtc_time_to_tm(txc.time.tv_sec,&tm);
	printk("UTC time: %d-%d-%d %d:%d:%d\n",tm.tm_year+1900, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

/*----------------------------Sequence Number---------------------------------*/
uint16_t sn_compute(uint16_t sn_p)
{
    uint16_t sn;
    sn = (sn_p + 1) % SN_MAX;
    return sn;
}

/*---------------------------DPD Function-------------------------------------*/
int dpd_mul(uint16_t sn_p, uint32_t tst_p, struct itsGnIfEntry *entry)
{
    int err;
    uint16_t sn_so = entry->item.sn;
    uint32_t tst_so = entry->item.pv.tst;
    //first find tst and sn of source station
    if(((tst_p > tst_so)&&((tst_p - tst_so) <= TST_MAX/2))||((tst_so > tst_p)&&((tst_so - tst_p) > TST_MAX/2)))
    {
        tst_so = tst_p;
        sn_so = sn_p;
        // should  update the sn and tst of loct?
        err = 0;
    }
    else if(tst_p == tst_so)
    {
        if(((sn_p >sn_so)&&((sn_p - sn_so) <= SN_MAX/2))||((sn_so > sn_p)&&((sn_so - sn_p) > SN_MAX/2)))
        {
            tst_so = tst_p;
            sn_so = sn_p;
            err = 0;
        }
        else
            err = -1;
    }
    else
        err = -1;

    if(err == -1)
        printk(KERN_INFO "This is a duplicate packet, discrad it.\n");

    return err;
}

int dpd_single(uint32_t tst_p, struct itsGnIfEntry *entry)
{
    int err;
    uint32_t tst_so = entry->item.pv.tst;
    if(((tst_p > tst_so)&&((tst_p - tst_so) <= TST_MAX/2))||((tst_so > tst_p)&&((tst_so - tst_p) > TST_MAX/2)))
    {
        tst_so = tst_p;
        err =0;
    }
    else
    {
        printk(KERN_INFO "This is a duplicate packet.\n");
        err = 1;
    }
    return err;
}

int dup_addr_detect(struct longpv so_pv)
{
    if((0 == (memcmp(&(so_pv.GN_ADDR.macaddr), &(itsLocalInfo.LocalConfig.itsGnLPV.GN_ADDR.macaddr), 6)))||
            (0 == (memcmp(&(so_pv.GN_ADDR), &(itsLocalInfo.LocalConfig.itsGnLPV.GN_ADDR), sizeof(struct gn_addr))))){
        return -1;
    }
    else
        return 0;
}
/*------------------------------------Packet Data rate-------------------------------------*/
static uint16_t pdr_compute(struct itsGnIfEntry *entry)
{
    uint32_t pdr;
    unsigned long time;

    time = jiffies - entry->item.create_time;
	printk(KERN_INFO "time(jiffies) = %lu.\n",time);
    time = jiffies_to_msecs(time);
	if(time == 0)
		time = 1000;
	printk(KERN_INFO "time(ms) = %lu.\n",time);

    pdr = itsLocalInfo.PacketForwarding.itsGnMaxPacketDataRateEmaBeta * entry->item.pdr_p +
        (100 - itsLocalInfo.PacketForwarding.itsGnMaxPacketDataRateEmaBeta) * ((entry->item.octet_count)*1000 / time);

    return pdr;
}

int pdr_update(struct itsGnIfEntry *entry, uint32_t len)
{
    entry->item.octet_count += len;
	printk(KERN_INFO "entry->item.octet_count = %d.\n",entry->item.octet_count);
    if(entry->item.create_time == 0){
        entry->item.create_time = jiffies;
    }
	printk(KERN_INFO "entry->item.create_time = %lu.\n",entry->item.create_time);

    entry->item.pdr_p = entry->item.pdr;
	printk(KERN_INFO "entry->item.pdr_p = %d.\n",entry->item.pdr_p);
    entry->item.pdr = pdr_compute(entry);
    return 0;
}

int data_rate_control(struct itsGnIfEntry *entry)
{
	int ret;
	if(entry->item.pdr > itsLocalInfo.PacketForwarding.itsGnMaxPacketDataRate*100*1000)
		ret = -1;
	else
		ret = 0;
	return ret;
}
		
/*---------------------------------Geo Area Size control----------------------------------*/
//这个函数用于对转发包发送前的判定，与另一个函数个geo_area_compute()不同
int geo_area_size_control(struct geohdr_common *gchdr, struct geohdr_gabc *gehdr)
{
    long area;
    area  = -1;
    switch(gchdr->hdrst)
    {
        case 0:
            area = pi * gehdr->gap.a *gehdr->gap.a;
            break;
        case 1:
            area = gehdr->gap.a * gehdr->gap.b;
            break;
        case 2:
            area = pi * gehdr->gap.a *gehdr->gap.b;
            break;
    }
    if(area == -1)
    {
        printk(KERN_INFO "The area size compute wrong!\n");
        return -1;
    }
    if(area > itsLocalInfo.PacketForwarding.itsGnMaxGeoAreaSize*(10^8))
    {
        printk(KERN_INFO "The area size of packet to be sending is too large!\n");
        return -1;
    }
    return 0;
}
/*--------------------------------Position Vector Update---------------------------------*/
//这个函数用packet中的so_pv来更新LocTE中的position vector,附录C.2
static int loct_pv_update(struct itsGnIfEntry *entry, struct longpv pv_rp, uint32_t tst_rp)
{
    uint32_t tst_so = entry->item.pv.tst;
    int ret;
    if(((tst_rp > tst_so)&&((tst_rp - tst_so) <= TST_MAX/2))||
            ((tst_so > tst_rp)&&((tst_so - tst_rp) > TST_MAX/2))){
        printk(KERN_INFO "The packet is new, update LocTE Position Vector\n");
        memcpy(&(entry->item.pv), &(pv_rp), sizeof(struct longpv));
        ret = 0;
    }
    else{
		printk("Entry->item.pv.tst = %lu.\n",entry->item.pv.tst);
		printk("gehdr->so_pv.tst = %lu.\n",tst_so);
        printk(KERN_INFO "The packet is not new, don't update LocTE Vector\n");
        ret = -1;
    }
    return ret;
}

//这个函数用LocTE中的depv来更新转发pcket中的de_pv，附录C.3
void packet_pv_update(struct itsGnIfEntry *entry, struct geohdr_common *gchdr, uint32_t tst_fp, int type)
{
    uint32_t tst_lo = entry->item.pv.tst;
    if(!(((tst_lo > tst_fp)&&((tst_lo -tst_fp) <= TST_MAX/2))||
            ((tst_fp > tst_lo)&&((tst_fp - tst_lo) > TST_MAX/2))))
    {
		printk(KERN_INFO "The de_pv of packet is new, noi update it with LocTE PV\n");
		return;
    }

	struct geohdr_guc *gehdr = 0;
    switch(type)
    {
        case GUC:
                 	 //struct geohdr_guc *gehdr;
                     gehdr = (struct geohdr_guc*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
                     break;
        case GAC:
                     //struct geohdr_gabc *gehdr;
                     gehdr = (struct geohdr_gabc*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
                     break;
        case GBC:
                     //struct geohdr_gabc *gehdr;
                     gehdr = (struct geohdr_gabc*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
                     break;
        case LSREP:
                     //struct geohdr_lsrep *gehdr;
                     gehdr = (struct geohdr_lsrep*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
                     break;
    }

	printk(KERN_INFO "The de_pv of packet is old, update it with LocTE Position Vector\n");
    memcpy(&(gehdr->de_pv.GN_ADDR), &(entry->item.pv.GN_ADDR), sizeof(struct gn_addr));
    gehdr->de_pv.tst = entry->item.pv.tst;
    gehdr->de_pv.latitude = entry->item.pv.latitude;
    gehdr->de_pv.longitude = entry->item.pv.longitude;
}

/* ****************Timer***********************/

static int get_timer_num(void)
{
    unsigned int i;
    unsigned int id;
    for(i = 0; i < TIMER_NUM; i++)
    {
        if(life_timer[i] == NULL)
        {
            id = i;
            break;
        }
        else{
            id = -1;
            printk(KERN_INFO "No more timer to alloc!\n");
        }
    }
    return id;
}


struct itsGnIfEntry *get_entry_by_index(uint64_t pindex, unsigned char mac[6])
{
    struct hlist_head *head;
    struct itsGnIfEntry *entry;

    head = &itsGnTable[pindex].head;
    if(hlist_empty(head)){
        printk(KERN_INFO "ERR: The entry to find is NULL!\n");
        return NULL;
    }

    for(entry = (struct itsGnIfEntry*)(head->first); entry != NULL; entry = (struct itsGnIfEntry*)(entry->node.next))
    {
		/*int i=0;
        printk(KERN_INFO "%02x:%02x:%02x:%02x:%02x:%02x\n",
               entry->item.pv.GN_ADDR.macaddr[i],entry->item.pv.GN_ADDR.macaddr[i+1],entry->item.pv.GN_ADDR.macaddr[i+2],
               entry->item.pv.GN_ADDR.macaddr[i+3],entry->item.pv.GN_ADDR.macaddr[i+4],entry->item.pv.GN_ADDR.macaddr[i+5]);
		printk(KERN_INFO "%02x:%02x:%02x:%02x:%02x:%02x\n",
               mac[i],mac[i+1],mac[i+2],
               mac[i+3],mac[i+4],mac[i+5]);*/
        if((memcmp(mac, entry->item.pv.GN_ADDR.macaddr, 6)) == 0)
            return entry;
    }

    printk(KERN_INFO "ERR: The entry to find doesn't exist!!\n");
    return NULL;
}

void del_mib_entry(struct itsGnIfEntry *entry)
{
    hlist_del(&(entry->node));
    kfree(entry);
	printk(KERN_INFO "Del the entry!\n");
}

static void entry_del(unsigned long pentry)
{
    struct itsGnIfEntry *entry;
    entry = (struct itsGnIfEntry*)(pentry);
    del_timer(life_timer[entry->item.timer_id]);
 	printk(KERN_INFO "Timer ends at ");
	print_time();
    kfree(life_timer[entry->item.timer_id]);
    //entry = get_entry_by_index(info->index, info->mac);
	//printk(KERN_INFO "The address of entry (transed)is %p.\n", entry);
	del_mib_entry(entry);
    printk(KERN_INFO "Del th timer of entry!\n");
}

static int entry_life_timer_create(struct itsGnIfEntry *entry)
{
    int timer_num;
    timer_num = get_timer_num();
	printk(KERN_INFO "Timer_id is %d.\n", timer_num);
    if(timer_num == -1)
        return -1;

    entry->item.timer_id = timer_num;
    entry->item.itsGnIfIndex = 2;
    entry->item.create_time = jiffies;
	printk(KERN_INFO "jiffies = %lu\n",jiffies);
	printk(KERN_INFO "entry->item.create_time = %lu\n", entry->item.create_time);
	printk(KERN_INFO "entry->item.create_time(s) = %u\n", jiffies_to_msecs(entry->item.create_time)/1000);

    life_timer[timer_num] = (struct timer_list*)kmalloc(sizeof(struct timer_list), GFP_KERNEL);
    init_timer(life_timer[timer_num]);
    life_timer[timer_num]->expires = jiffies + msecs_to_jiffies(itsLocalInfo.LocalConfig.itsGnLifetimeLocTE * 1000);
    life_timer[timer_num]->data = (unsigned long)entry;
    life_timer[timer_num]->function = entry_del;

    //add_timer(life_timer[timer_num]);

    return timer_num;
}

static void __add_mib_entry(struct itsGnIfEntry *buf, uint64_t pindex)
{
    if(0 == (hlist_empty(&itsGnTable[pindex].head)))
    {
        printk(KERN_INFO "list head has been taken.\n");
        hlist_add_before(&(buf->node), itsGnTable[pindex].head.first);
    }
    else
    {
        printk(KERN_INFO "list head is empty.\n");
        hlist_add_head(&(buf->node), &(itsGnTable[pindex].head));
    }

}


struct itsGnIfEntry *add_mib_entry(uint64_t index, unsigned char mac[6])
{
    struct itsGnIfEntry *entry;
    uint16_t timer_id;;

    entry = kmalloc(sizeof(struct itsGnIfEntry), GFP_KERNEL);
	memset(entry, 0, sizeof(struct itsGnIfEntry));
	//printk(KERN_INFO "The address of entry (unchanged)is %p.\n", entry);
    if(entry == NULL)
        return NULL;

    memset(entry, 0, sizeof(struct itsGnIfEntry));
    __add_mib_entry(entry, index);

    timer_id = entry_life_timer_create(entry);
    if(timer_id == -1)
        goto err;


    return entry;
err:
    kfree(entry);
    return NULL;

}


int update_mib_entry(struct itsGnIfEntry *entry, struct geohdr_basic *gbhdr, struct geohdr_common *gchdr, int type, int flag)
{
    int ret;

    //set version
    entry->item.version = gbhdr->version;
    //update pdr
    //ret = data_rate_control(entry, len);
    switch(type)
    {
        case BEACON:
            {
                struct geohdr_beacon* gehdr = (struct geohdr_beacon*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
                printk(KERN_INFO "Update entry to Mib for Beacon.\n");
                ret = loct_pv_update(entry, gehdr->so_pv, gehdr->so_pv.tst);
                //memcpy(entry->item.pv, gehdr->so_pv, sizeof(struct longpv));
                entry->item.IS_NEIGHBOUR = TRUE;
                break;
            }
        case SHB:
            {
                struct geohdr_shb* gehdr = (struct geohdr_shb*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
                printk(KERN_INFO "Update entry to Mib for SHB\n");
                //update the pv
                ret = loct_pv_update(entry, gehdr->so_pv, gehdr->so_pv.tst);
                //set flag IS_NEIGHBOUR
                entry->item.IS_NEIGHBOUR = TRUE;
                break;
            }
        case GUC:
            {
                struct geohdr_guc* gehdr = (struct geohdr_guc*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
                printk(KERN_INFO "Update entry to Mib for GUC\n");
                ret = loct_pv_update(entry, gehdr->so_pv, gehdr->so_pv.tst);
                entry->item.IS_NEIGHBOUR = FALSE;
                break;
            }
        case GAC:
            {
                struct geohdr_gabc* gehdr = (struct geohdr_gabc*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
                printk(KERN_INFO "Update entry to Mib for GAC\n");
                ret = loct_pv_update(entry, gehdr->so_pv, gehdr->so_pv.tst);
                entry->item.IS_NEIGHBOUR = FALSE;
                break;
            }
        case GBC:
            {
                struct geohdr_gabc* gehdr = (struct geohdr_gabc*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
                printk(KERN_INFO "Update entry to Mib for GBC\n");
                ret = loct_pv_update(entry, gehdr->so_pv, gehdr->so_pv.tst);
                entry->item.IS_NEIGHBOUR = FALSE;
                break;
            }
        case TSB:
            {
                struct geohdr_tsb* gehdr = (struct geohdr_tsb*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
                printk(KERN_INFO "Update entry to Mib  for TSB\n");
                ret = loct_pv_update(entry, gehdr->so_pv, gehdr->so_pv.tst);
                entry->item.IS_NEIGHBOUR = FALSE;
                break;
            }
        case LSREQ:
            {
                struct geohdr_lsreq* gehdr = (struct geohdr_lsreq*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
                printk(KERN_INFO "Update entry to Mib  for LsRequest\n");
                ret = loct_pv_update(entry, gehdr->so_pv, gehdr->so_pv.tst);
                entry->item.IS_NEIGHBOUR = FALSE;
                break;
            }
        case LSREP:
            {
                struct geohdr_lsrep* gehdr = (struct geohdr_lsrep*)((unsigned char*)gchdr + GEO_COMMON_HEADER_LEN);
                printk(KERN_INFO "Update entry to Mib  for LsReply\n");
                ret = loct_pv_update(entry, gehdr->so_pv, gehdr->so_pv.tst);
                entry->item.IS_NEIGHBOUR = FALSE;
                break;
            }
        default:{
                    printk(KERN_INFO "Unkown packet!\n");
                    ret = -1;
                    break;
                }
    }

    if(flag == 0){
        add_timer(life_timer[entry->item.timer_id]);
 		printk("Timer set at ");
		print_time();
		//printk("jiffies = %lu\n",jiffies);
		printk("time(s) = %u\n",jiffies_to_msecs(jiffies)/1000);
	}
    else if(flag == 1){
		mod_timer(life_timer[entry->item.timer_id], jiffies + msecs_to_jiffies(itsLocalInfo.LocalConfig.itsGnLifetimeLocTE * 1000));
 		printk("Timer reset at ");
		print_time();
		//printk("jiffies = %lu\n",jiffies);
		printk("time(s) = %u\n",jiffies_to_msecs(jiffies)/1000);
		//printk("Lifetime(s) = %d\n",itsLocalInfo.LocalConfig.itsGnLifetimeLocTE);
	}
    else
        ret = -1;

    return ret;
}

uint64_t hash_compute(unsigned char mac[6])
{
    uint64_t index;
    uint64_t pmac;
    int i;

    //计算哈希值,需要源mac地址
    pmac = mac[0];
    for(i =1 ; i < 6; i++)
    {
        pmac = pmac << 8;
        pmac += mac[i];
    }
    index = hash_64(pmac, 7);
    printk(KERN_INFO "Hash number = %lld.\n", index);
    return index;
}

