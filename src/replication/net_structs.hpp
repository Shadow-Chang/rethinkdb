#ifndef __REPLICATION_NET_STRUCTS_HPP__
#define __REPLICATION_NET_STRUCTS_HPP__

#include <stdint.h>

#include "btree/value.hpp"
#include "serializer/translator.hpp"

namespace replication {

enum multipart_aspect { SMALL = 0x81, FIRST = 0x82, MIDDLE = 0x83, LAST = 0x84 };

enum message_code { MSGCODE_NIL = 0, INTRODUCE,
                    BACKFILL, BACKFILL_COMPLETE, BACKFILL_DELETE_EVERYTHING, BACKFILL_SET, BACKFILL_DELETE,
                    // TODO: Explicitly number these
                    GET_CAS, SARC, INCR, DECR, APPEND, PREPEND, DELETE, NOP };

struct net_castime_t {
    cas_t proposed_cas;
    repli_timestamp timestamp;
} __attribute__((__packed__));

struct net_hello_t {
    char hello_magic[16];
    uint32_t replication_protocol_version;
} __attribute__((__packed__));

struct net_introduce_t {
    uint32_t database_creation_timestamp;
    // When master sends net_introduce_t, this is ID of last slave seen. When slave sends
    // net_introduce_t, this is unused.
    uint32_t other_id;
} __attribute__((__packed__));

struct net_header_t {
    uint8_t message_multipart_aspect;
    uint8_t msgcode;
    uint16_t msgsize;
} __attribute__((__packed__));

struct net_multipart_header_t {
    uint8_t message_multipart_aspect;
    uint8_t msgcode;
    uint16_t msgsize;
    uint32_t ident;
} __attribute__((__packed__));

struct net_backfill_t {
    repli_timestamp timestamp;
} __attribute__((__packed__));

struct net_backfill_complete_t {
    repli_timestamp time_barrier_timestamp;
} __attribute__((__packed__));

struct net_backfill_delete_everything_t {
    // We need at least 4 bytes so that we do not get a msgsize
    // smaller than sizeof(net_multipart_header_t).
    uint32_t padding;
} __attribute__((__packed__));

// TODO: Get rid of one of these, and maybe just make the protocol
// "layer" completely symmetric.
struct net_nop_t {
    repli_timestamp timestamp;
} __attribute__((__packed__));

struct net_get_cas_t {
    cas_t proposed_cas;
    repli_timestamp timestamp;
    uint16_t key_size;
    char key[];
} __attribute__((__packed__));

// TODO: Make this structure more efficient, use optional fields for
// flags, exptime, add_policy, old_cas.
struct net_sarc_t {
    repli_timestamp timestamp;
    cas_t proposed_cas;
    mcflags_t flags;
    exptime_t exptime;
    uint16_t key_size;
    uint32_t value_size;
    uint8_t add_policy;
    uint8_t replace_policy;
    cas_t old_cas;
    char keyvalue[];
} __attribute__((__packed__));

struct net_backfill_set_t {
    repli_timestamp timestamp;
    mcflags_t flags;
    exptime_t exptime;
    cas_t cas_or_zero;
    uint16_t key_size;
    uint32_t value_size;
    char keyvalue[];
} __attribute__((__packed__));

struct net_incr_t {
    repli_timestamp timestamp;
    cas_t proposed_cas;
    uint64_t amount;
    uint16_t key_size;
    char key[];
} __attribute__((__packed__));

struct net_decr_t {
    repli_timestamp timestamp;
    cas_t proposed_cas;
    uint64_t amount;
    uint16_t key_size;
    char key[];
} __attribute__((__packed__));

struct net_append_t {
    repli_timestamp timestamp;
    cas_t proposed_cas;
    uint16_t key_size;
    uint32_t value_size;

    // The first key_size bytes are for the key, the next value_size
    // bytes (possibly spanning multiple messages) are for the value.
    char keyvalue[];
} __attribute__((__packed__));

struct net_prepend_t {
    repli_timestamp timestamp;
    cas_t proposed_cas;
    uint16_t key_size;
    uint32_t value_size;

    // The first key_size bytes are for the key, the next value_size
    // bytes (possibly spanning multiple messages) are for the value.
    char keyvalue[];
} __attribute__((__packed__));

struct net_delete_t {
    repli_timestamp timestamp;
    uint16_t key_size;
    char key[];
} __attribute__((__packed__));

struct net_backfill_delete_t {
    // We need at least 4 bytes so that we do not get a msgsize
    // smaller than sizeof(net_multipart_header_t).
    uint16_t padding;
    uint16_t key_size;
    char key[];
} __attribute__((__packed__));

}  // namespace replication

#endif  // __REPLICATION_NET_STRUCTS_HPP__
