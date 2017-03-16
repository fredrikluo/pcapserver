#ifndef SCRIPT_PARSER_H
#define SCRIPT_PARSER_H

#include "define.h"
#include "expat.h"
#include "http_parser.h"
#include <vector>


/**
 * Build a script as http response.
 */
class script_parser {
public:
    script_parser(class http_stream_manager* http_stream_mgr, const char* filename);
    virtual ~script_parser();

protected:
    virtual void OnstartElementHandler(const XML_Char* name, const XML_Char** atts);
    virtual void OnendElementHandler(const XML_Char* name);
    virtual void OncharacterDataHandler(const XML_Char* s, int len);
    virtual void loadheader();
    virtual void loadbody();
    virtual void loadfile();
    virtual void build_req_response_pair();
    virtual void parse_predefined_vars_header();

private:
    enum {
        BUILD_NONE,
        //status respons pairs
        BUILD_RESPONSE_PAIR = 0x10,
        //status request
        BUILD_REQUEST = 0x30,
        BUILD_REQUEST_URL = 0x31,
        //status response
        BUILD_RESPONSE = 0x50,
        BUILD_RESPONSE_HEADER = 0x51,
        BUILD_RESPONSE_BODY = 0x52,
        BUILD_RESPONSE_BODY_FILE = 0x53

    } m_status;

    static void startElementHandler(void* userData, const XML_Char* name, const XML_Char** atts);
    static void endElementHandler(void* userData, const XML_Char* name);
    static void characterDataHandler(void* userData, const XML_Char* s, int len);

    XML_Parser m_xmlparser;
    http_response_pair m_current_pair;
    http_stream_manager* m_http_stream_mgr;
    stdstring m_header;
    std::vector<u_char> m_body;
    stdstring m_file;
};

#endif
