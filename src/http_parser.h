#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include "define.h"
#include <string>
#include <vector>

class http_response_pair {
public:
    std::vector<u_char> m_request;
    std::vector<u_char> m_response;
    std::string m_url;
    std::string m_mime;

    void clear()
    {
        m_request.clear();
        m_response.clear();
        m_url.clear();
        m_mime.clear();
    }

#ifdef DEBUG
    void dump_data();
#endif
};

class http_parser {
private:
    static const char* HTTP_CMD[];
    static const u_int HTTP_CMD_COUNT;

    bool isHTTP_header(const std::string& buffer);
    bool isHTTP_header(const std::vector<u_char>& buffer);
    bool isHTTP_response(std::string& buffer, std::string::size_type pos_start);
    std::string::size_type find_response(std::string& buffer, std::string::size_type pos_start);

public:
    http_parser(const std::vector<u_char>& request,
                const std::vector<u_char>& response,
                std::vector<http_response_pair>& res_req_list);

    static void get_url_from_request(std::vector<u_char>& request, std::string& url);
    static void get_mime_from_response(std::vector<u_char>& response, std::string& mime);
};

#endif // HTTP_PARSER_H
