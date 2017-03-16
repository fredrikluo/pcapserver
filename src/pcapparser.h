#ifndef PCAP_PARSER_H
#define PCAP_PARSER_H

#include "tcp_stream_manager.h"
#include <string>


/**
 * Parse the pcap file and save it to http
 * stream manager
 */
class pcap_parser {
public:
    pcap_parser();
    virtual ~pcap_parser() {};

    bool parse(class http_stream_manager* mgr, const char* filename);
    std::string& get_error_msg() { return m_err_msg; }

protected:
    int get_linklayer_length(int linktype);
    std::string m_err_msg;
};

#endif
