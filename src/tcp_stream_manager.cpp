#include <iostream>

#include "tcp_packet.h"
#include "tcp_stream_manager.h"
#include "utility.h"

using namespace std;

tcp_stream_manager::tcp_stream_manager()
    : m_packet_num(0) {
}

void tcp_stream_manager::add_packet(const u_char* buffer, const u_int length) {

    m_packet_num++;
    DEBUG_TRACE("Add packet begins with packet num %d", m_packet_num);
    tcp_packet packet(buffer, length, m_packet_num);

    if (!packet) {
        DEBUG_TRACE("Corrupted packet %d", m_packet_num);
        return;
    }

    m_tcp_packet_list.push_back(packet);
}

void tcp_stream_manager::process() {
    for (list<tcp_packet>::iterator itr = m_tcp_packet_list.begin();
         itr != m_tcp_packet_list.end();
         itr++) {
        //SYN means A new tcp stream starts
        if ((itr->get_TCPHeader().get_th_flags() & TH_SYN) &&
           !(itr->get_TCPHeader().get_th_flags() & TH_ACK)) {
            //Create new stream
            tcp_stream new_stream;
            new_stream.load(itr, m_tcp_packet_list);
            m_tcpstreams.push_back(new_stream);
            DEBUG_TRACE("New stream port1 %d  port2 %d ", new_stream.get_p1_port(), new_stream.get_p2_port());
        }
    }
}

void tcp_stream_manager::output_streams() {
    for (vector<tcp_stream>::iterator itr = m_tcpstreams.begin();
         itr != m_tcpstreams.end();
         itr++) {
        itr->output_stream();
    }
}
