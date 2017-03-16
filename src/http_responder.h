#ifndef HTTP_RESPONDER
# define HTTP_RESPONDER

#include "define.h"
#include <stdio.h>
#include <string>
#include <sys/select.h>
#include <vector>

/**
 * Object of this class hold a http stream
 */
class http_responder {
public:
    http_responder(int sock)
        : m_sock(sock)
        , m_offset(0)
        , m_send_index(0)
        , m_recv_parsed_pos(0)
    {
    }

    bool send_data();
    bool recv_data();
    void set_response(std::vector<u_char>& response, std::string& url);
    int get_socket() const { return m_sock; }
    bool operator!() { return m_offset == -1; }
    bool is_empty() const { return m_response_list.size() ? false : true; }
    const std::vector<u_char>& get_revbuffer() const { return m_revbuffer; }
    int& get_recv_parsed_pos() { return m_recv_parsed_pos; }

private:
    int m_sock;
    std::vector<std::vector<u_char> > m_response_list;
    std::vector<u_char> m_revbuffer;
    int m_offset;
    int m_send_index;
    int m_recv_parsed_pos;
    std::vector<std::string> m_url_list;
};

#endif // HTTP_RESPONDER
