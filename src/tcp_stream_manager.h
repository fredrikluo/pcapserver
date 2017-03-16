#ifndef TCP_STREAM_MANAGER_H
#define TCP_STREAM_MANAGER_H

#include "tcp_stream.h"
#include <list>
#include <vector>


/**
 * This class holds all tcp streams.
 */
class tcp_stream_manager {
public:
    tcp_stream_manager();
    void add_packet(const u_char* buffer, const u_int length);
    void process();
    void output_streams();
    const std::vector<tcp_stream>& get_tcpstreams() const { return m_tcpstreams; }

protected:
    std::vector<tcp_stream> m_tcpstreams;
    std::list<tcp_packet> m_tcp_packet_list;
    u_int m_packet_num;
};

#endif
