#ifndef TCP_STREAM_H
#define TCP_STREAM_H

#include <iostream>
#include <list>
#include "define.h"
#include "tcp_packet.h"

/**
 * This class represents a tcp stream.
 *
 * It loads and track the tcp stream from a binary stream
 * from SYNC to FIN.
 *
 * There are cases that streams are not complete in
 * a binary stream. This class should be able to handle
 * such cases.
*/
class tcp_stream {
public:
    tcp_stream();
    tcp_stream(const tcp_stream& _p);
    void load(const std::list<tcp_packet>::iterator& start_packet,
            const std::list<tcp_packet>& tcp_packet_list);

    virtual ~tcp_stream() {}

    bool add_packet(tcp_packet& _tcp_packet);
    void output_stream();
    bool get_terminated() const { return m_terminated; }
    void set_terminated(bool _terminated);

    u_int get_p1_ip() const { return m_p1_ip; }
    u_int get_p1_port() const { return m_p1_port; }
    u_int get_p2_ip() const { return m_p2_ip; }
    u_int get_p2_port() const { return m_p2_port; }

    const std::list<tcp_packet>& get_tcp_streamP12() const { return m_data_p12; }
    const std::list<tcp_packet>& get_tcp_streamP21() const { return m_data_p21; }

protected:
    enum stream_status {
        TCP_ESTABLISHED,
        TCP_SYN_SENT,
        TCP_SYN_RECV,
        TCP_FIN_WAIT1,
        TCP_FIN_WAIT2,
        TCP_TIME_WAIT,
        TCP_CLOSE,
        TCP_CLOSE_WAIT,
        TCP_LAST_ACK,
        TCP_LISTEN,
        TCP_CLOSING /* now a valid state */
    };

    void set_status(stream_status _status) { m_stream_status = _status; }
    stream_status get_status() const { return m_stream_status; }
    void set_start_p12_seq_number(u_int seq_num) { m_start_p12_seq_number = seq_num; }
    void set_start_p21_seq_number(u_int seq_num) { m_start_p21_seq_number = seq_num; }

    u_int m_p1_ip;
    u_int m_p1_port;
    u_int m_p2_ip;
    u_int m_p2_port;

    u_int m_start_p12_seq_number;
    u_int m_start_p21_seq_number;
    bool m_terminated;
    stream_status m_stream_status;
    std::list<tcp_packet> m_data_p12;
    std::list<tcp_packet> m_data_p21;
};

#endif // TCP_STREAM_H
