#include "geo_output.h"

static int gn_data_request(struct gn_request_param gen_params, struct sk_buff *skb)
{
	struct sock *sk = skb->sk;
	struct geo_sock *geo = geo_sk(sk);//to do what ?
}

static int basic_header_create()
{

}

static int common_header_create()
{

}

static int shb_header_create()
{

}
