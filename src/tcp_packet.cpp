//
// C++ Implementation: tcp_packet
//
// Description:
//
//
// Author: luozhiyu <zhiyul@localhost.localdomain>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "tcp_packet.h"
#include "utility.h"
#include <iostream>

tcp_packet::tcp_packet(const u_char* buffer, const u_int _length, const int packet_number)
    : m_is_loaded_correct(false)
    , m_packet_num(packet_number)
{
    //Fill the ip header
    m_iphdr = buffer;

    if (m_iphdr.get_protocol() != 0x06 || !m_iphdr.is_valid()) {
        DEBUG_TRACE("Not tcp packet");
        return;
    }

    //Fill the tcp header
    m_tcphdr = (buffer + m_iphdr.get_header_length());

    //Loading the data
    u_int data_length = (m_iphdr.get_totoal_len() - m_tcphdr.get_th_off() - m_iphdr.get_header_length());
    const u_char* data_start = buffer;
    data_start += m_iphdr.get_header_length() + m_tcphdr.get_th_off();

    for (u_int index = 0; index < data_length; index++)
        m_data.push_back(data_start[index]);

    m_is_loaded_correct = true;

    m_data_length = data_length;
}

tcp_packet::tcp_packet(const tcp_packet& packet)
    : m_iphdr(packet.m_iphdr)
    , m_tcphdr(packet.m_tcphdr)
    , m_data_length(packet.m_data_length)
    , m_is_loaded_correct(packet.m_is_loaded_correct)
    , m_packet_num(packet.m_packet_num)
{
    m_data = packet.m_data;
}

const tcp_packet& tcp_packet::operator=(const tcp_packet& packet)
{
    m_iphdr = packet.m_iphdr;
    m_tcphdr = packet.m_tcphdr;
    m_data_length = packet.m_data_length;
    m_is_loaded_correct = packet.m_is_loaded_correct;
    m_packet_num = packet.m_packet_num;
    m_data = packet.m_data;
    return *this;
}

tcp_packet::~tcp_packet()
{
}

void tcp_packet::output_data()
{
    for (std::vector<u_char>::iterator itr = m_data.begin(); itr != m_data.end(); itr++) {
        std::cout << *itr;
    }
}
