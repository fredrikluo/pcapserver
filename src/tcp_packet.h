//
// C++ class: tcp_packet
//
//
// Author: luozhiyu <luozhiyu@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TCP_PACKET_H
#define TCP_PACKET_H

#include "define.h"
#include "iphdr.h"
#include "tcphdr.h"
#include <vector>

const u_int TH_FIN = 0x01;
const u_int TH_SYN = 0x02;
const u_int TH_RST = 0x04;
const u_int TH_PUSH = 0x08;
const u_int TH_ACK = 0x10;
const u_int TH_URG = 0x20;

/**
 * This class holds and parse a tcp packet
 */
class tcp_packet {
public:
    tcp_packet(){};
    tcp_packet(const u_char* buffer, const u_int length, const int packet_number);
    tcp_packet(const tcp_packet& packet);

    bool operator!() { return !m_is_loaded_correct; }
    const tcp_packet& operator=(const tcp_packet& packet);
    virtual ~tcp_packet();
    const iphdr& get_IPHeader() const { return m_iphdr; }
    const tcphdr& get_TCPHeader() const { return m_tcphdr; }
    const u_int get_data_length() const { return m_data_length; }
    const u_int get_packet_num() const { return m_packet_num; }
    void output_data();
    const std::vector<u_char>& get_data() const { return m_data; };

protected:
    iphdr m_iphdr;
    tcphdr m_tcphdr;
    u_int m_data_length;
    bool m_is_loaded_correct;
    std::vector<u_char> m_data;
    u_int m_packet_num;
};

#endif  // TCP_PACKET_H
