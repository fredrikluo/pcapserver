#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "define.h"
#include <stdio.h>
#include <string>
#include <sys/select.h>
#include <vector>

#include "http_responder.h"

/**
 * A simple http server, which hold a set of
 * http_responders, which serve http content
 */
class http_server {
public:
    http_server(class http_stream_manager* phttp_stream_mgr);
    void start(int port);

protected:
    void remove_http_responder(int sd_i);
    void parser_http_header(const std::vector<u_char>& header, int& start_pos, std::vector<std::string>& url_list);

    std::vector<http_responder> m_resp_list;
    class http_stream_manager* m_packet_mgr;
    fd_set m_write_set;
    fd_set m_read_set;
    int m_listen_socket;
    std::vector<u_char> m_header_stack;
};

#endif // HTTP_SERVER_H
