#include "http_parser.h"
#include "tcp_stream.h"
#include "utility.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

const char* http_parser::HTTP_CMD[] = {
    "GET",
    "POST",
    "HTTP"
};

const u_int http_parser::HTTP_CMD_COUNT = sizeof(HTTP_CMD) / sizeof(char*);

string::size_type http_parser::find_response(string& buffer,
                                             string::size_type pos_start) {
    while (true) {
        pos_start = buffer.find("HTTP/1.", pos_start);
        if (pos_start == string::npos)
            return string::npos;

        if (isHTTP_response(buffer, pos_start))
            return pos_start;

        pos_start += 7;
    }
}

http_parser::http_parser(const vector<u_char>& request,
                         const vector<u_char>& response,
                         std::vector<http_response_pair>& res_req_list) {
    // Extract the requests
    string data12;
    for (vector<u_char>::const_iterator itr = request.begin();
            itr != request.end(); itr++) {
        data12 += *itr;
    }

    string data21;
    for (vector<u_char>::const_iterator itr = response.begin();
            itr != response.end(); itr++) {
        data21 += *itr;
    }

    if (!(isHTTP_header(data21) &&
          isHTTP_header(data12))) {
        return;
    }

    DEBUG_TRACE("Found a HTTP stream");

    vector<vector<u_char> > request_list;
    string::size_type pos = 0;
    string::size_type pos_start = 0;

    while (string::npos != (pos = data12.find("\r\n\r\n", pos_start))) {
        string header = data12.substr(pos + 4, 10);
        if ((pos + 4) == data12.size() || isHTTP_header(header)) {
            vector<u_char> request;
            for (string::size_type i = pos_start; i < pos + 4; i++)
                request.push_back(data12[i]);

            request_list.push_back(request);
        }

        pos_start = pos + 4;
    }

    // Strictly speaking, here a HTTP stack should be written to parse all the reponses
    // However, we are not writing a http server, most of the response could be handled
    // rather by text parsing.
    vector<vector<u_char> > response_list;
    pos_start = 0;
    string::size_type pos_end = 0;
    while (pos_end != data21.size()) {
        pos_start = find_response(data21, pos_start);

        if (pos_start != string::npos) {
            pos_end = find_response(data21, pos_start + 7);
        }

        if (pos_start == string::npos &&
            pos_end == string::npos) {
            DEBUG_TRACE("No response found");
            break;
        }

        if (pos_start != string::npos &&
            pos_end == string::npos) {
            pos_end = data21.size();
        }

        vector<u_char> response;
        for (string::size_type i = pos_start; i < pos_end; i++) {
            response.push_back(data21[i]);
        }

        response_list.push_back(response);
        pos_start = pos_end;
    }

    if (response_list.size() != request_list.size()) {
        DEBUG_TRACE("unmatched request & response");
    }

    // if we have unmatched request and response, we only push in the matched part
    for (size_t i = 0; i < min(request_list.size(), response_list.size()); i++) {
        http_response_pair res_pair;

        std::vector<u_char> & req_list  = res_pair.m_request;
        std::vector<u_char> & resp_list = res_pair.m_response;

        req_list.insert(req_list.end(), request_list[i].begin(), request_list[i].end());
        resp_list.insert(resp_list.end(), response_list[i].begin(), response_list[i].end());

        get_url_from_request(req_list, res_pair.m_url);
        get_mime_from_response(resp_list, res_pair.m_mime);

        res_req_list.push_back(res_pair);
#ifdef DEBUG
        res_pair.dump_data();
#endif
    }
}

static bool is_valid_white(char c) {
    return (c == ' ' || c == '\r'|| c == '\n');
}

void http_parser::get_url_from_request(vector<u_char>& request, string& url) {
    bool start = false;

    for (u_int index = 0; index < request.size(); index++) {
        if (start) {
            if (is_valid_white((char)request[index])) {
                break;
            }

            url += (char)request[index];
        }

        if (!start && (char)request[index] == ' ')
            start = true;
    }
}

void http_parser::get_mime_from_response(vector<u_char>& response, string& mime) {
    string header;
    for (u_int index = 0; index < response.size(); index++) {
        if ('\r' == response[index] && index + 3 < response.size()) {
            if ('\n' == response[index + 1] &&
                '\r' == response[index + 2] &&
                '\n' == response[index + 3]){
                break;
            }
        }

        header += response[index];
    }

    string::size_type pos = header.find("Content-Type:");
    if (string::npos != pos) {
        for (u_int i = pos + sizeof("Content-Type:"); i < header.size(); i++) {
            if (is_valid_white((char)response[i])) {
                break;
            }
            mime += header[i];
        }
    }
}

/**
   This function is to check if  the *buffer* start with a HTTP header,
   the algorithm here is quite primitive.
*/

bool http_parser::isHTTP_header(const std::string& buffer) {
    const u_int MAX_BUFFER_COUNT = 10;

    if (buffer.size() < MAX_BUFFER_COUNT)
        return false;

    static char pstrbuffer[MAX_BUFFER_COUNT + 1];
    memset(pstrbuffer, 0, MAX_BUFFER_COUNT + 1);

    for (u_int index = 0; index < MAX_BUFFER_COUNT; index++)
        pstrbuffer[index] = (char)buffer[index];

    // Look for any HTTP command
    for (u_int index = 0; index < HTTP_CMD_COUNT; index++) {
        if (!strncmp(pstrbuffer, HTTP_CMD[index], strlen(HTTP_CMD[index]))) {
            return true;
        }
    }

    return false;
}

bool http_parser::isHTTP_header(const std::vector<u_char>& buffer) {
    string sbuffer;
    for (size_t i = 0; i < buffer.size(); i++)
        sbuffer += buffer[i];

    return isHTTP_header(sbuffer);
}

bool http_parser::isHTTP_response(string& buffer, string::size_type pos_start) {
    string response = buffer.substr(pos_start, 20);
    if (strncmp(buffer.c_str(), "HTTP/1.", 7))
        return false;

    string::size_type i = sizeof("HTTP/1.1");
    if (i >= response.size())
        return false;

    response = response.substr(i, 13);
    int status_code = atoi(response.c_str());
    if (status_code > 99 && status_code < 506)
        return true;

    return false;
}

#ifdef DEBUG
#include <iostream>
using namespace std;
void http_response_pair::dump_data() {
    cout << ("-dumping begins") << endl;
    cout << ("-request") << endl;

    for (u_int index = 0; index < m_request.size(); index++) {
        cout << (m_request[index]);
    }

    cout << ("-response") << endl;
    for (u_int index = 0; index < m_response.size(); index++) {
        cout << (m_response[index]);
    }

    cout << ("-dumping ends") << endl;
}
#endif // DEBUG
