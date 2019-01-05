/*
 * Copyright (c) 2019 Tsinghua University, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <config.h>

#include "ofp-actions.h"
#include "ofp-msgs.h"
#include "ofp-util.h"
#include "ofproto-provider.h"
#include "openvswitch/vlog.h"

#include "tt.h"

VLOG_DEFINE_THIS_MODULE(tt);

void
onf_tt_table_create(struct ofconn *ofconn)
{
    struct ofproto *ofproto = ofconn_get_ofproto(ofconn);

    ovs_assert(!ofproto->tt_table);
    ofproto->tt_table = xmalloc(sizeof *ofproto->tt_table);

    ofproto->tt_table->expect_flows = 0;
    ofproto->tt_table->recv_flows = 0;
    ofproto->tt_table->max_flows = 255;
    list_init(&ofproto->tt_table->entry_list);
}

enum ofperr 
onf_tt_flow_receive_start(struct ofconn *ofconn, unsigned int flow_cnt)
{
    struct ofproto *ofproto = ofconn_get_ofproto(ofconn);
    struct onf_tt_table *tt_table = ofproto->tt_table;
    enum ofperr error;

    if (flow_cnt > tt_table->max_flows) {
        /* TODO(chenweihang): add error type. */
    }
    
    tt_table->expect_flows = flow_cnt;
    tt_table->state = TTS_MUTABLE;

    return error;
}

enum ofperr 
onf_tt_flow_receive_end(struct ofconn *ofconn)
{
    struct ofproto *ofproto = ofconn_get_ofproto(ofconn);
    struct onf_tt_table *tt_table = ofproto->tt_table;
    enum ofperr error;

    if (!tt_table) {
        /* TODO(chenweihang): add error type. */
    } else if (tt_table->state != TTS_MUTABLE) {
        /* TODO(chenweihang): add error type. */
    } else if (tt_table->expect_flows != tt_table->recv_flows) {
        /* TODO(chenweihang): add error type. */
    }

    tt_table->state = TTS_CONST;

    /* tt table print test */
    /*
    struct ovs_list *cur = tt_table->entry_list.next;
    while (cur != &(tt_table->entry_list)) {
        VLOG_INFO("%d %d %d %d %d %d %d %d\n",
                  *(uint32_t *)(cur + 16),
                  *(uint32_t *)(cur + 20),
                  *(uint32_t *)(cur + 24),
                  *(uint64_t *)(cur + 28),
                  *(uint64_t *)(cur + 36),
                  *(uint32_t *)(cur + 44),
                  *(uint32_t *)(cur + 48),
                  *(uint64_t *)(cur + 54));
        cur = cur->next;
    } */
    VLOG_INFO("TT Control process end!\n");

    return error;
}

enum ofperr 
onf_tt_table_add_entry(struct ofconn *ofconn,
                       struct ofputil_tt_flow_mod_msg *ttm)
{
    
    struct ofproto *ofproto = ofconn_get_ofproto(ofconn);
    struct onf_tt_table *tt_table = ofproto->tt_table;
    struct onf_tt_entry *tte;
    enum ofperr error;

    if (!tt_table) {
        /* TODO(chenweihang): add error type. */
    } else if (tt_table->state != TTS_MUTABLE) {
        /* TODO(chenweihang): add error type. */
    }

    tt_table->recv_flows++;
    tte = onf_tt_entry_alloc(ttm);
    list_push_back(&tt_table->entry_list, &tte->node);

    return error;
}
