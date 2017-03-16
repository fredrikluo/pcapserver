#include "tcp_stream.h"
#include "tcp_packet.h"
#include "utility.h"
#include <iostream>

using namespace std;

tcp_stream::tcp_stream()
    : m_p1_ip(0)
    , m_p1_port(0)
    , m_p2_ip(0)
    , m_p2_port(0)
    , m_start_p12_seq_number(0)
    , m_start_p21_seq_number(0)
    , m_terminated(false)
    , m_stream_status(TCP_CLOSE)
{
}


tcp_stream::tcp_stream(const tcp_stream& _p)
    : m_p1_ip(_p.m_p1_ip)
    , m_p1_port(_p.m_p1_port)
    , m_p2_ip(_p.m_p2_ip)
    , m_p2_port(_p.m_p2_port)
    , m_start_p12_seq_number(_p.m_start_p12_seq_number)
    , m_start_p21_seq_number(_p.m_start_p21_seq_number)
    , m_terminated(_p.m_terminated)
    , m_stream_status(_p.m_stream_status)
    , m_data_p12(_p.m_data_p12)
    , m_data_p21(_p.m_data_p21)
{
}

#define OP_SADDR(x) ((x)->get_IPHeader().get_saddr())
#define OP_DADDR(x) ((x)->get_IPHeader().get_daddr())
#define OP_SPORT(x) ((x)->get_TCPHeader().get_sport())
#define OP_DPORT(x) ((x)->get_TCPHeader().get_dport())

static bool is_same_stream(tcp_packet& pack_a, tcp_packet& pack_b) {
    tcp_packet * a = &pack_a;
    tcp_packet * b = &pack_b;
    return (OP_SADDR(a) == OP_SADDR(b) && OP_SPORT(a) == OP_SPORT(b) &&
            OP_DADDR(a) == OP_DADDR(b) && OP_DPORT(a) == OP_DPORT(b)) ||
           (OP_SADDR(a) == OP_DADDR(b) && OP_SPORT(a) == OP_DPORT(b) &&
            OP_DADDR(a) == OP_SADDR(b) && OP_DPORT(a) == OP_SPORT(b));
}

void tcp_stream::load(const list<tcp_packet>::iterator& start_packet, const list<tcp_packet>& tcp_packet_list) {
    m_p1_ip = OP_SADDR(start_packet);
    m_p1_port = OP_SPORT(start_packet);
    m_p2_ip = OP_DADDR(start_packet);
    m_p2_port = OP_DPORT(start_packet);

    m_start_p12_seq_number =start_packet->get_TCPHeader().get_seq();
    m_stream_status = tcp_stream::TCP_SYN_SENT;

    DEBUG_TRACE("Create tcp stream:");
    list<tcp_packet> all_packet_list;

    //Grap all the packets with same port number and ip
    for (list<tcp_packet>::iterator itr = start_packet;
         itr != tcp_packet_list.end();
         itr++) {

        if (is_same_stream(*itr, *start_packet)) {
            all_packet_list.push_back(*itr);
        }
    }

    //Hand shake
    for (list<tcp_packet>::iterator itr = all_packet_list.begin();
         itr != all_packet_list.end();) {
        if (get_status() == tcp_stream::TCP_SYN_SENT) {
            DEBUG_TRACE("enter TCP_SYN_SENT: port1 %d  port2 %d ", this->get_p1_port(), this->get_p2_port());

            if ((itr->get_TCPHeader().get_th_flags() & TH_SYN) &&
                (itr->get_TCPHeader().get_th_flags() & TH_ACK)) {
                set_status(tcp_stream::TCP_SYN_RECV);
                m_start_p21_seq_number = itr->get_TCPHeader().get_seq();
            } else if (itr->get_TCPHeader().get_th_flags() & TH_RST) {
                set_status(tcp_stream::TCP_CLOSE);
                set_terminated(true);
            }

            itr = all_packet_list.erase(itr);
        } else if (get_status() == tcp_stream::TCP_SYN_RECV) {
            DEBUG_TRACE("enter TCP_SYN_RECV", this->get_p1_port(), this->get_p2_port());

            if (!(itr->get_TCPHeader().get_th_flags() & TH_SYN) &&
                 (itr->get_TCPHeader().get_th_flags() & TH_ACK)) {
                set_status(tcp_stream::TCP_ESTABLISHED);
            } else if (itr->get_TCPHeader().get_th_flags() & TH_RST) {
                set_status(tcp_stream::TCP_CLOSE);
                set_terminated(true);
            } else if (itr->get_TCPHeader().get_th_flags() & TH_FIN) {
                set_status(tcp_stream::TCP_FIN_WAIT1);
            }

            itr = all_packet_list.erase(itr);
            break;
        } else
            itr++;
    }

    // Track it to the end.
    m_start_p12_seq_number++;
    for (list<tcp_packet>::iterator itr_inner = all_packet_list.begin();
            itr_inner != all_packet_list.end();) {
        if (m_start_p12_seq_number == itr_inner->get_TCPHeader().get_seq()) {
            m_start_p12_seq_number = itr_inner->get_TCPHeader().get_seq() +
                itr_inner->get_data_length() +
                (itr_inner->get_TCPHeader().get_th_flags() & 0x07 ? 1 : 0);

            add_packet(*itr_inner);
            all_packet_list.erase(itr_inner);
            itr_inner = all_packet_list.begin();
        } else {
            itr_inner++;
        }
    }

    m_start_p21_seq_number++;
    for (list<tcp_packet>::iterator itr_inner = all_packet_list.begin();
            itr_inner != all_packet_list.end();) {
        if (m_start_p21_seq_number == itr_inner->get_TCPHeader().get_seq()) {
            m_start_p21_seq_number = itr_inner->get_TCPHeader().get_seq() +
                itr_inner->get_data_length() +
                (itr_inner->get_TCPHeader().get_th_flags() & 0x07 ? 1 : 0);
            add_packet(*itr_inner);
            all_packet_list.erase(itr_inner);
            itr_inner = all_packet_list.begin();
        } else {
            itr_inner++;
        }
    }

    DEBUG_TRACE("Create done");
}

bool tcp_stream::add_packet(tcp_packet& _tcp_packet) {
    if (OP_SADDR(&_tcp_packet) == m_p1_ip &&
        OP_DADDR(&_tcp_packet) == m_p2_ip) {
        DEBUG_TRACE("Add packet 12 %d", _tcp_packet.get_packet_num());
        m_data_p12.push_back(_tcp_packet);
        return true;
    }

    if (OP_DADDR(&_tcp_packet) == m_p1_ip &&
        OP_SADDR(&_tcp_packet) == m_p2_ip) {
        DEBUG_TRACE("Add packet 21 %d", _tcp_packet.get_packet_num());
        m_data_p21.push_back(_tcp_packet);
        return true;
    }

    DEBUG_TRACE("Ingore packet %d", _tcp_packet.get_packet_num());
    return false;
}

void tcp_stream::output_stream() {
    DEBUG_TRACE("Stream begins: ip1 %d,  port1 %d, ip2 %d, port2 %d", m_p1_ip, m_p1_port, m_p2_ip, m_p2_port);
    list<tcp_packet>::iterator itr12 = m_data_p12.begin();
    list<tcp_packet>::iterator itr21 = m_data_p21.begin();

    while (true) {
        if (itr12 != m_data_p12.end() &&
            itr21 != m_data_p21.end()) {
            if (itr12->get_packet_num() < itr21->get_packet_num()) {
                itr12->output_data();
                itr12++;
            } else {
                itr21->output_data();
                itr21++;
            }
        } else if (itr12 != m_data_p12.end()) {
            itr12->output_data();
            itr12++;
        } else if (itr21 != m_data_p21.end()) {
            itr21->output_data();
            itr21++;
        } else
            break;
    }

    DEBUG_TRACE("Stream ends ip1 %d,  port1 %d, ip2 %d, port2 %d", m_p1_ip, m_p1_port, m_p2_ip, m_p2_port);
}

void tcp_stream::set_terminated(bool _terminated) {
    m_terminated = _terminated;
}
