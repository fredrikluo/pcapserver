#include "script_parser.h"
#include "http_stream_manager.h"
#include "utility.h"
#include <assert.h>
#include <fstream>
#include <vector>

using namespace std;

static const int READ_BUFFER_SIZE = 1024;

static const TCHAR HTTP_RESPONSE_PAIR_TAG[] = _T("httpresponsepair");
static const TCHAR HTTP_REQUEST_TAG[] = _T("httprequest");
static const TCHAR HTTP_REQUEST_URL_TAG[] = _T("url");
static const TCHAR HTTP_RESPONSE_TAG[] = _T("httpresponse");
static const TCHAR HTTP_RESPONSE_HEADER_TAG[] = _T("header");
static const TCHAR HTTP_RESPONSE_BODY_TAG[] = _T("body");
static const TCHAR HTTP_RESPONSE_BODY_FILE_TAG[] = _T("file");

script_parser::script_parser(http_stream_manager* http_stream_mgr, const char* filename)
    : m_status(BUILD_NONE)
    , m_http_stream_mgr(http_stream_mgr)
{
    ifstream xmlfile(filename);
    if (!xmlfile) {
        INFO_OUT("Open script file failed");
        return;
    }

    m_xmlparser = XML_ParserCreate(NULL);
    assert(m_xmlparser != NULL && "Find a nice way to deal with OOM:)");

    if (!m_xmlparser) {
        INFO_OUT("Failed to create XML parser");
        return;
    }

    XML_SetUserData(m_xmlparser, (void*)this);
    XML_SetElementHandler(m_xmlparser, startElementHandler, endElementHandler);
    XML_SetCharacterDataHandler(m_xmlparser, characterDataHandler);

    while (!xmlfile.eof()) {
        char readbuffer[READ_BUFFER_SIZE];
        xmlfile.read(readbuffer, READ_BUFFER_SIZE);
        XML_Parse(m_xmlparser, readbuffer, xmlfile.gcount(), xmlfile.eof());
    }
}

script_parser::~script_parser()
{
    XML_ParserFree(m_xmlparser);
}

void script_parser::OnstartElementHandler(const XML_Char* name, const XML_Char** atts)
{
    DEBUG_TRACE("Start element %s", name);
    stdstring start_element(name);

    if (start_element == HTTP_RESPONSE_PAIR_TAG) {
        m_current_pair.clear();
        m_header.clear();
        m_body.clear();
        m_file.clear();
        m_status = BUILD_RESPONSE_PAIR;
    } else if (start_element == HTTP_REQUEST_TAG)
        m_status = BUILD_REQUEST;
    else if (start_element == HTTP_RESPONSE_TAG)
        m_status = BUILD_RESPONSE;
    else if (start_element == HTTP_REQUEST_URL_TAG)
        m_status = BUILD_REQUEST_URL;
    if (start_element == HTTP_RESPONSE_HEADER_TAG)
        m_status = BUILD_RESPONSE_HEADER;
    else if (start_element == HTTP_RESPONSE_BODY_TAG)
        m_status = BUILD_RESPONSE_BODY;
    else if (start_element == HTTP_RESPONSE_BODY_FILE_TAG)
        m_status = BUILD_RESPONSE_BODY_FILE;
}

void script_parser::OnendElementHandler(const XML_Char* name)
{
    DEBUG_TRACE("End element %s", name);
    stdstring end_element(name);

    if (end_element == HTTP_RESPONSE_PAIR_TAG) {
        build_req_response_pair();
        m_http_stream_mgr->add_packet(m_current_pair);
        m_status = BUILD_NONE;
    } else if (end_element == HTTP_REQUEST_TAG)
        m_status = BUILD_RESPONSE_PAIR;
    else if (end_element == HTTP_REQUEST_URL_TAG)
        m_status = BUILD_REQUEST;
    else if (end_element == HTTP_RESPONSE_TAG)
        m_status = BUILD_RESPONSE_PAIR;
    else if (end_element == HTTP_RESPONSE_HEADER_TAG)
        m_status = BUILD_RESPONSE;
    else if (end_element == HTTP_RESPONSE_BODY_TAG)
        m_status = BUILD_RESPONSE;
    else if (end_element == HTTP_RESPONSE_BODY_FILE_TAG) {
        loadfile();
        m_file.clear();
        m_status = BUILD_RESPONSE_BODY;
    } else
        DEBUG_TRACE("State machine goes wrong");

    DEBUG_TRACE("End element fininsed %s", name);
}

void script_parser::OncharacterDataHandler(const XML_Char* s, int len)
{
    if (!len)
        return;

    if (m_status == BUILD_REQUEST_URL) {
        //load the url
        for (int i = 0; i < len; i++)
            m_current_pair.m_url += s[i];
    } else if (m_status == BUILD_RESPONSE_HEADER) {
        //load the head
        const u_char* buffer = (const u_char*)(s);
        for (size_t i = 0; i < len * sizeof(XML_Char); i++)
            m_header += buffer[i];

    } else if (m_status == BUILD_RESPONSE_BODY) {
        //load the body
        const u_char* buffer = (const u_char*)(s);
        for (size_t i = 0; i < len * sizeof(XML_Char); i++)
            m_body.push_back(buffer[i]);
    } else if (m_status == BUILD_RESPONSE_BODY_FILE) {
        //load file into body
        for (int i = 0; i < len; i++)
            m_file += s[i];
    }
}

void script_parser::startElementHandler(void* userData, const XML_Char* name, const XML_Char** atts)
{
    if (!userData)
        return;
    static_cast<script_parser*>(userData)->OnstartElementHandler(name, atts);
}

void script_parser::endElementHandler(void* userData, const XML_Char* name)
{
    if (!userData)
        return;
    static_cast<script_parser*>(userData)->OnendElementHandler(name);
}

void script_parser::characterDataHandler(void* userData, const XML_Char* s, int len)
{
    if (!userData)
        return;
    static_cast<script_parser*>(userData)->OncharacterDataHandler(s, len);
}

void script_parser::loadheader()
{
    for (stdstring::size_type i = 0; i < m_header.size(); i++)
        m_current_pair.m_response.push_back(m_header[i]);

    //Append the end-mark of HTTP header ---------------------------
    if (m_header.size() > 4) {
        if (!((m_header[m_header.size() - 4] == '\r') && (m_header[m_header.size() - 3] == '\n') && (m_header[m_header.size() - 2] == '\r') && (m_header[m_header.size() - 1] == '\n'))) {
            m_current_pair.m_response.push_back('\r');
            m_current_pair.m_response.push_back('\n');
            m_current_pair.m_response.push_back('\r');
            m_current_pair.m_response.push_back('\n');
        }
    }
    //----------------------------------------------------------------
}

void script_parser::loadbody()
{
    m_current_pair.m_response.insert(m_current_pair.m_response.end(), m_body.begin(), m_body.end());
}

void script_parser::loadfile()
{
    ifstream embed_file(m_file.c_str());

    if (!embed_file) {
        INFO_OUT("Failed to read the file %s", m_file.c_str());
        return;
    }

    while (!embed_file.eof()) {
        char readbuffer[READ_BUFFER_SIZE];
        embed_file.read(readbuffer, READ_BUFFER_SIZE);
        for (int i = 0; i < embed_file.gcount(); i++)
            m_body.push_back(readbuffer[i]);
    }
}

void script_parser::build_req_response_pair()
{
    //Parse all the system varible in header
    parse_predefined_vars_header();
    //Assemble the req and response pair
    loadheader();
    loadbody();
}

void script_parser::parse_predefined_vars_header()
{
    //Replace content_length
    stdstring::size_type pos;
    while (stdstring::npos != (pos = m_header.find(_T("$content_length")))) {
        char buffer[20];
        snprintf(buffer, 20, "%lu", m_body.size());
        m_header.replace(pos, pos + sizeof(_T("$content_length")), buffer);
    }
    //---------Add more system variable here -------------------------------------
    //----------------------------------------------------------------------------
}
