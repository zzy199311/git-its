
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "geo_output.h"

int test(struct sk_buff *skb, struct gn_request_param *params)
{
    if(params->secpro ==0)
    {
        printk(KERN_INFO "This shb packet requests SN_ENCAP!\n");
	return 0;
        //skb_cp = gn_sec_prepare(skb, params);

        //err = basic_header_create(skb_cp, params, type);
        //if(err != 0)
        //    goto drop;
    }
}
