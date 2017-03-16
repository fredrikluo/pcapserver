//
// C++ class:iphdr
//
//
// Author: luozhiyu <luozhiyu@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef IPHDR_H
# define IPHDR_H

#include "define.h"

class iphdr {
public:
    unsigned int get_header_length() const { return ihl; }
    unsigned int get_version() const { return version; }
    unsigned int get_type_of_service() const { return tos; }
    unsigned int get_totoal_len() const { return tot_len; }
    unsigned int get_id() const { return id; }
    unsigned int get_frag_off() const { return frag_off; }
    unsigned int get_ttl() const { return ttl; }
    unsigned int get_protocol() const { return protocol; }
    unsigned int get_check() const { return check; }
    unsigned int get_saddr() const { return saddr; }
    unsigned int get_daddr() const { return daddr; }

    const unsigned char* operator=(const unsigned char* buffer);
    bool is_valid() { return isvalid; }

protected:
    short comput_checksum(const unsigned char* buffer, int len);

    unsigned int ihl;
    unsigned int version;
    unsigned int tos;
    unsigned int tot_len;
    unsigned int id;
    unsigned int frag_off;
    unsigned int ttl;
    unsigned int protocol;
    unsigned int check;
    unsigned int saddr;
    unsigned int daddr;

    bool isvalid;
};

#endif  // IPHDR_H
