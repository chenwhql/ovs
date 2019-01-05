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

#ifndef TT_H
#define TT_H 1

#include <sys/types.h>

#include "connmgr.h"
#include "ofp-util.h"
#include "ofproto-provider.h"

#ifdef  __cplusplus
extern "C" {
#endif

struct onf_tt_entry {
    /* List Pointer. */
    struct ovs_list node;
    /* Entry attributes in mod msg. */
    struct ofputil_tt_flow_mod_msg attrs;
};

enum tt_table_state {
    TTS_MUTABLE,
    TTS_CONST
};

struct onf_tt_table {
    /* The flow count in tt flow ctrl msg. */
    unsigned int expect_flows;
    /* The flow count received actually. */
    unsigned int recv_flows;
    /* Maxinum number of flows. */
    unsigned int max_flows;
    /* Whether it can be changed. */
    enum tt_table_state state;

    /* List of the tt flow entry. */
    struct ovs_list entry_list;
};

static inline struct onf_tt_entry *
onf_tt_entry_alloc(struct ofputil_tt_flow_mod_msg *);
static inline void onf_tt_entry_free(struct onf_tt_entry *);

void onf_tt_table_create(struct ofconn *);
enum ofperr onf_tt_flow_receive_start(struct ofconn *, unsigned int flow_cnt);
enum ofperr onf_tt_flow_receive_end(struct ofconn *);

static inline struct onf_tt_entry *
onf_tt_entry_alloc(struct ofputil_tt_flow_mod_msg *ttm)
{
    struct onf_tt_entry *entry = xmalloc(sizeof *entry);

    memcpy(&entry->attrs, ttm, sizeof entry->attrs);

    return entry;
}

static inline void
onf_tt_entry_free(struct onf_tt_entry *entry)
{
    if (entry) {
        free(entry);
    }
}

#ifdef  __cplusplus
}
#endif

#endif
