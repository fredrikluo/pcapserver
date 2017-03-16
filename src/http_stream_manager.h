#ifndef HTTP_PACKETS_MANAGER_H
#define HTTP_PACKETS_MANAGER_H

#include "define.h"
#include "http_parser.h"
#include "tcp_stream_manager.h"
#include <string>
#include <vector>

/**
 * This class is to hold all the http streams found in
 * pcap file or created by a script.
 */
class http_stream_manager {
public:
    http_stream_manager() {
    };

    virtual ~http_stream_manager() {
    };

    void add_packet(std::vector<u_char>& request_stream,
            std::vector<u_char>& response_stream);

    void add_packet(const http_response_pair& http_req_response);

    bool find_responsebyurl(std::string& url,
            std::vector<u_char>& repsonse,
            int& base_request_num);

    void set_datafile(std::string& datafile) {
        m_data_filename = datafile;
    }

    void set_error_msg(std::string& err_msg) {
        m_err_msg = err_msg;
    }

#ifdef DEBUG
    void dump_req_response();
#endif

protected:
    void index_page(std::vector<u_char>& repsonse);
    void fav_icon(std::vector<u_char>& repsonse);

    std::vector<http_response_pair> m_request_response_list;
    std::string m_data_filename;
    std::string m_err_msg;
};

#endif // HTTP_PACKETS_MANAGER_H
