#ifndef TT_H
#define TT_H 1

#include <linux/types.h>
#include <linux/skbuff.h>

#define TT_FLOW_ID_MAX  1024    /* max flow id */
#define TT_PORT 63000   /* tt flow dest port */
#define ETH_P_TT 0x88D7 /* tt flow ethernet type */

#define TT_HLEN 4   /* tt header length */
#define TT_TABLE_SIZE_MIN 16    /* minimum size of tt_table */
#define TT_BUFFER_SIZE 4096 /* one flow one buffer */
#define MAX_JITTER 100000
#define NSEC_PER_SECOND 1000000000
#define NSEC_PER_MSECOND 1000000
#define NSEC_PER_USECOND 1000

#define TIMESPEC_TO_NSEC(time_spec) \
    (time_spec.tv_sec * (__u64)NSEC_PER_SECOND + time_spec.tv_nsec)

#define SWAP(x, y) x = x^y; y = x^y; x = x^y;

// TT报文头
/**
struct eth_tthdr {
    unsigned char h_dest[ETH_ALEN]; //目的MAC地址
    unsigned char h_source[ETH_ALEN]; //源MAC地址
    __u16 h_proto ; //网络层所使用的协议类型
    __u16 flow_id; //tt的flow_id
    __u16 len; //报文的整体长度（包括mac、ip、udp头），不包括CRC
};
**/

struct tt_header {
    __u16 flow_id; //tt的flow_id
    __u16 len; //报文的整体长度
};

/**
	note:
	假设控制器给的tt报文的flow_id都是从1开始依次增长，所以下面的实现过程直接用flow_id-1作为数组的索引
	则在这种情况下，最大的flow_id即为tt_table->count
	当对表项无论进行删除还是修改，都应该保证表中的flow_id让仍然是从1开始
	之后开发的过程中，这部分可能需要进行修改
**/
//tt调度表
struct tt_table_item {
	__u16 flow_id;	//TT流标识
	__u16 buffer_id;	//共享缓存的buffer id
	__u32 circle;	 // 发送周期范围：0.5, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024(ms), 但是在此单位是ns
	__u64 time;	//该端口接收或发送此flow的时刻（ns）
	struct rcu_head rcu;
    __u16 len;	 // 报文长度
};

struct tt_table {
	struct rcu_head rcu;
	__u32 count, max;
	struct tt_table_item* __rcu tt_items[];
};

struct tt_send_cache {
    __u64 *send_times;
    __u16 *flow_ids;
    __u16 size;
};

struct tt_send_info {
    __u64 macro_period;
    __u32 advance_time;
    struct tt_send_cache send_cache; 
};

/* tt operation */
bool udp_port_is_tt(__be16 port);
bool eth_p_tt(__be16 eth_type);
unsigned char *skb_tt_header(struct sk_buff *skb);
bool is_trdp_packet(struct sk_buff *skb);
bool is_tt_packet(struct sk_buff *skb);
int trdp_to_tt(struct sk_buff *skb);
int tt_to_trdp(struct sk_buff *skb);

/* tt_table operation */
struct tt_table_item *tt_table_item_alloc(void);
void rcu_free_tt_table(struct rcu_head *rcu);
struct tt_table *tt_table_alloc(int size);
struct tt_table_item* tt_table_lookup(const struct tt_table* cur_tt_table, const __be16 flow_id);
int tt_table_num_items(const struct tt_table* cur_tt_table);
struct tt_table* tt_table_delete_item(struct tt_table* cur_tt_table, __be16 flow_id);
struct tt_table* tt_table_item_insert(struct tt_table *cur_tt_table, const struct tt_table_item *new);

/* tt send info */
__u64 global_time_read(void);
int dispatch(struct vport* vport);
void get_next_time(struct vport *vport, __u64 cur_time, __u64 *wait_time, __u16 *flow_id, __u64 *send_time);
#endif
