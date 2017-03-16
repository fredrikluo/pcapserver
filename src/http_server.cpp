#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "http_stream_manager.h"
#include "http_parser.h"
#include "http_server.h"
#include "utility.h"
#include "resource.h"

using namespace std;

const int TIMEOUT = 10 * 60; // 600s, 10minutes

void http_server::remove_http_responder(int sd_i) {
    for (vector<http_responder>::iterator itr = m_resp_list.begin(); itr != m_resp_list.end(); itr++) {
        if (itr->get_socket() == sd_i) {
            m_resp_list.erase(itr);
            break;
        }
    }
}

http_server::http_server(http_stream_manager* phttp_stream_mgr)
    : m_packet_mgr(phttp_stream_mgr)
    , m_listen_socket(0) {
    FD_ZERO(&m_write_set);
    FD_ZERO(&m_read_set);
}

void http_server::start(int port) {
    int max_sd;
    struct sockaddr_in addr;

    // Create socket
    if ((m_listen_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        ERROR_OUT("Can't create socket");
        exit(-1);
    }

    // Allow socket descriptor to be reuseable
    int on = 1;
    if (setsockopt(m_listen_socket, SOL_SOCKET, SO_REUSEADDR,
            (char*)&on, sizeof(on)) < 0) {
        ERROR_OUT("setsockopt() failed");
        close(m_listen_socket);
        exit(-1);
    }

    // Set socket to be non-blocking.  All of the sockets for
    // the incoming connections will also be non-blocking since
    // they will inherit that state from the listening socket.
    if (ioctl(m_listen_socket, FIONBIO, (char*)&on) < 0) {
        ERROR_OUT("ioctl() failed");
        close(m_listen_socket);
        exit(-1);
    }

    // Bind the socket
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(m_listen_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        ERROR_OUT("bind() failed");
        close(m_listen_socket);
        exit(-1);
    }

    //Start listening
    if (listen(m_listen_socket, 32) < 0) {
        ERROR_OUT("listen() failed");
        close(m_listen_socket);
        exit(-1);
    }

    FD_ZERO(&m_read_set);
    FD_SET(m_listen_socket, &m_read_set);
    max_sd = m_listen_socket;

    int last_session = -1;
    bool end_server = false;

    while (!end_server) {
        fd_set write_set, read_set;
        memcpy(&write_set, &m_write_set, sizeof(m_write_set));
        memcpy(&read_set, &m_read_set, sizeof(m_read_set));

        struct timeval timeout = { TIMEOUT, 0 };
        int rs = select(max_sd + 1, &read_set, &write_set, NULL, &timeout);

        if (rs < 0) {
            ERROR_OUT("  select() failed");
            break;
        }

        if (rs == 0) {
            INFO_OUT("Time out");
            break;
        }

        if (FD_ISSET(m_listen_socket, &read_set)) {
            //Accept new connection
            int new_socket = 0;

            while (new_socket != -1) {
                new_socket = accept(m_listen_socket, NULL, NULL);

                if (new_socket < 0) {
                    if (errno != EWOULDBLOCK) {
                        ERROR_OUT("  accept() failed");
                        end_server = true;
                    }
                    break;
                }

                FD_SET(new_socket, &m_read_set);
                FD_SET(new_socket, &m_write_set);
                max_sd = (new_socket > max_sd) ? new_socket : max_sd;
                http_responder resp(new_socket);
                m_resp_list.push_back(resp);

                INFO_OUT("new connection %d", new_socket);
            }
        }

        //Check for all the http responders
        for (int i = m_resp_list.size() - 1; i >= 0; i--) {
            bool read_close_conn = false;
            bool send_close_conn = false;

            if (FD_ISSET(m_resp_list[i].get_socket(), &read_set)) {
                if (!m_resp_list[i].recv_data()) {
                    read_close_conn = true;
                }

                vector<string> url_list;
                parser_http_header(m_resp_list[i].get_revbuffer(), m_resp_list[i].get_recv_parsed_pos(), url_list);

                for (u_int url_i = 0; url_i < url_list.size(); url_i++) {
                    vector<u_char> response;
                    INFO_OUT("header loaded for %s with socket %d", url_list[url_i].c_str(), m_resp_list[i].get_socket());
                    if (!m_packet_mgr->find_responsebyurl(url_list[url_i], response, last_session)) {
                        for (u_int i = 0; i < strlen(HTTP404); i++)
                            response.push_back(HTTP404[i]);
                    }

                    m_resp_list[i].set_response(response, url_list[url_i]);
                }
            }

            if (FD_ISSET(m_resp_list[i].get_socket(), &write_set)) {
                if (!m_resp_list[i].send_data()) {

                    send_close_conn = true;
                }
            }

            //Close connections
            if (read_close_conn || send_close_conn) {
                INFO_OUT("Close connection %d", m_resp_list[i].get_socket());
                FD_CLR(m_resp_list[i].get_socket(), &m_write_set);
                FD_CLR(m_resp_list[i].get_socket(), &m_read_set);
                close(m_resp_list[i].get_socket());

                if (m_resp_list[i].get_socket() == max_sd) {
                    while ((FD_ISSET(max_sd, &m_read_set)) || (FD_ISSET(max_sd, &m_write_set)))
                        max_sd--;
                }

                DEBUG_TRACE("Maximium %d", max_sd);

                m_resp_list.erase(m_resp_list.begin() + i);
            }
        }

    } // Server loop

    // Cleanup all of the sockets that are open
    for (int i = 0; i <= max_sd; ++i) {
        if (FD_ISSET(i, &m_read_set) || FD_ISSET(i, &m_write_set))
            close(i);
    }
}

void http_server::parser_http_header(const vector<u_char>& header,
                                     int& start_pos,
                                     vector<string>& url_list) {
    if (header.size() < 4)
        return;

    string url;
    for (; start_pos < static_cast<int>(header.size());) {
        if (start_pos + 3 < static_cast<int>(header.size()) &&
            header[start_pos] == '\r' &&
            header[start_pos + 1] == '\n' &&
            header[start_pos + 2] == '\r' &&
            header[start_pos + 3] == '\n') {
            string url;
            //Get new header
            if ((m_header_stack[0] == 'G' && m_header_stack[1] == 'E' && m_header_stack[2] == 'T')
                || (m_header_stack[0] == 'H' && m_header_stack[1] == 'E' && m_header_stack[2] == 'A' && m_header_stack[3] == 'D')) {
                bool start = false;

                for (u_int index = 0; index < m_header_stack.size(); index++) {
                    if (start) {
                        if ((char)m_header_stack[index] == ' ' || (char)m_header_stack[index] == '\r' || (char)m_header_stack[index] == '\n')
                            break;

                        url += (char)m_header_stack[index];
                    }

                    if (!start && (char)m_header_stack[index] == ' ')
                        start = true;
                }

                m_header_stack.clear();
                //convert the  asb url to relative one
                DEBUG_TRACE("found %s", url.c_str());

                if (url[0] == '/') {
                    url_list.push_back(url);
                } else {
                    string protocol = url.substr(0, url.find(":"));

                    if (protocol != "HTTP" && protocol != "http") {
                        url_list.push_back(string(""));
                    } else {
                        int pos = url.find(":") + 3; //skip "//"

                        pos = url.find("/", pos);

                        if (pos != -1) {
                            url_list.push_back(url.substr(pos, url.size()));
                        } else
                            url_list.push_back(string(""));
                    }
                }
            }

            start_pos += 4;
            continue;
        }

        m_header_stack.push_back(header[start_pos]);
        start_pos++;
    }
}
