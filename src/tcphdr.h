//
// C++: tcphdr
//
//
// Author: luozhiyu <luozhiyu@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TCPHDR_H
# define TCPHDR_H

#include "define.h"

class tcphdr {
public:
    unsigned int get_sport() const { return sport; }
    unsigned int get_dport() const { return dport; }
    unsigned int get_seq() const { return seq; }
    unsigned int get_ack() const { return ack; }
    unsigned int get_th_x2() const { return th_x2; }
    unsigned int get_th_off() const { return th_off; }
    unsigned int get_th_flags() const { return th_flags; }
    unsigned int get_th_win() const { return th_win; }
    unsigned int get_th_sum() const { return th_sum; }
    unsigned int get_th_urp() const { return th_urp; }

    const unsigned char* operator=(const unsigned char* buffer);

protected:
    unsigned int sport; /* source port */
    unsigned int dport; /* destination port */
    unsigned int seq; /* sequence number */
    unsigned int ack; /* acknowledgement number */
    unsigned int th_x2; /* (unused) */
    unsigned int th_off; /* data offset */
    unsigned int th_flags;
    unsigned int th_win; /* window */
    unsigned int th_sum; /* checksum */
    unsigned int th_urp; /* urgent pointer */
};

#endif  // TCPHDR_H
