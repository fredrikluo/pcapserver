#include "http_stream_manager.h"

#include "resource.h"
#include "utility.h"

#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;


void http_stream_manager::add_packet(vector<u_char> &request_stream,
                                     vector<u_char> &response_stream) {
  // parsing the content
  http_parser parser(request_stream,
                     response_stream,
                     m_request_response_list);
}

void http_stream_manager::add_packet(
    const http_response_pair &http_req_response) {
  m_request_response_list.push_back(http_req_response);
}

bool http_stream_manager::find_responsebyurl(string &url,
                                             vector<u_char> &repsonse,
                                             int &base_request_num) {

  // index page
  if (url == "/pcapserver") {
    DEBUG_TRACE("GET index page");
    index_page(repsonse);
    return true;
  }

  // favicon
  if (url == "/favicon.ico") {
    DEBUG_TRACE("GET Favicon.ico");
    fav_icon(repsonse);
    return true;
  }

  // Load for http sessions
  string::size_type pos = url.find("/p_session?");
  if (string::npos != pos) {
    u_int prefix_size = sizeof("/p_session?") - 1;
    string num = url.substr(pos + prefix_size,
                            url.size() - (prefix_size + pos));

    u_int index = atoi(num.c_str());
    if (index < m_request_response_list.size()) {
      DEBUG_TRACE("Enter session %s,%d,%d", num.c_str(), index, pos);
      repsonse = m_request_response_list[index].m_response;
      base_request_num = index;
      return true;
    }

    DEBUG_TRACE("Session out of bound %s,%d,%d", num.c_str(), index, pos);
    return false;
  }

  // if not loading any particular session, we act as a proxy serve
  // the first one we found.
  for (u_int i = base_request_num + 1 != 0 ? base_request_num + 1 : 0;
       i < m_request_response_list.size(); i++) {
    // find the first one
    if (url == url_abs_to_rel(m_request_response_list[i].m_url)) {
      repsonse = m_request_response_list[i].m_response;
      return true;
    }
  }

  // Found nothing for this url.
  DEBUG_TRACE("Found nothing for %s", url.c_str());
  return false;
}

/**
 * UGLY! UGLY, special function to build up index page in C++
 *
 * Could be done better with a template engine.
 */
void http_stream_manager::index_page(vector<u_char> &repsonse) {
  static char content_size_buffer[10] = {0};

  string index_body("<html><head><style>\
                      table {background-color:blue;}\
                      table td{background-color:white;}\
                      table th{background-color:blue;color:white; text-align:left}\
                      .selected{background-color:#00ff00; color:black;}\
                      </style></head><body><h1>Pcap Server</h1>");

  index_body += "<h3>Using pcap file:" + m_data_filename + " &nbsp;&nbsp;";
  snprintf(content_size_buffer, 10, "%lu", m_request_response_list.size());
  index_body += content_size_buffer;
  index_body += " Sessions in total </h3>";

  if (m_request_response_list.size()) {
    index_body += "<table cellspacing=1px>\
                  <tr style=\"background-color:blue;font-color:white;\" width=100px><th >&nbsp;&nbsp;MIME TYPE</th><th>&nbsp;&nbsp;URL</th></tr>\r\n";

    ostringstream stream_index;
    for (u_int i = 0; i < m_request_response_list.size(); i++) {
      string path = url_abs_to_rel(
              m_request_response_list[i].m_url.substr(0,
                  m_request_response_list[i].m_url.rfind("/") + 1));

      // TODO: fix it by regex.
      if ((m_request_response_list[i].m_mime.find("text/") != string::npos &&
           m_request_response_list[i].m_mime.find("ml") != string::npos) ||
          m_request_response_list[i].m_url.find("ml") != string::npos) {

          stream_index << "<tr><td class=\"selected\" > &nbsp;&nbsp;"
                     << m_request_response_list[i].m_mime.c_str()
                     << "</td><td  class=\"selected\" title=\""
                     << m_request_response_list[i].m_url.c_str()
                     << "\">&nbsp;&nbsp;<a href=\"" << path.c_str()
                     << "p_session?" << i << "\">"
                     << m_request_response_list[i].m_url.c_str()
                     << "</a></td></tr>\r\n";
      } else {

          stream_index << "<tr><td> &nbsp;&nbsp;"
                     << m_request_response_list[i].m_mime.c_str()
                     << "</td><td title=\""
                     << m_request_response_list[i].m_url.c_str()
                     << "\">&nbsp;&nbsp;<a href=\"" << path.c_str()
                     << "p_session?" << i << "\">"
                     << m_request_response_list[i].m_url.c_str()
                     << "</a></td></tr>\r\n";
      }
    }

    index_body += stream_index.str() + "</table>";
  } else {
    index_body += "The pcapserver can't find any useful data in the file you "
                  "just uploaded, here is the possible reason. <BR>";
    if (m_err_msg.length())
      index_body += m_err_msg;
    else
      index_body += "There seems to be no HTTP traffic in your pcap file, if "
                    "there is any , then it's bug of pcapserver, ping "
                    "fredrikl@opera.com";
  }

  index_body += "</body></html>\r\n";
  snprintf(content_size_buffer, 10, "%lu", index_body.size());
  string content_size(content_size_buffer);

  string index_header = "HTTP/1.1 200 OK\r\nServer: "
                        "PCAPSERVER\r\nContent-Type: text/html\r\nConnection: "
                        "close\r\nContent-length:";
  index_header += content_size_buffer;

  index_header += "\r\n\r\n";

  index_header = index_header + index_body;

  for (u_int i = 0; i < index_header.size(); i++)
    repsonse.push_back(index_header[i]);
}

void http_stream_manager::fav_icon(vector<u_char> &repsonse) {

  string fav_icon_header =
      "HTTP/1.1 200 OK\r\nServer: PCAPSERVER\r\nContent-Type: "
      "image/vnd.microsoft.icon\r\nConnection: close\r\nContent-Length:";

  static char content_size_buffer[10] = {0};
  snprintf(content_size_buffer, 10, "%lu", FavIconLen);
  fav_icon_header += content_size_buffer;
  fav_icon_header += "\r\n\r\n";

  for (u_int i = 0; i < fav_icon_header.size(); i++)
    repsonse.push_back(fav_icon_header[i]);

  for (u_int i = 0; i < FavIconLen; i++)
    repsonse.push_back(FavIcon[i]);
}


#ifdef DEBUG
void http_stream_manager::dump_req_response() {
  for (vector<http_response_pair>::iterator itr =
           m_request_response_list.begin();
       itr != m_request_response_list.end(); itr++) {
    itr->dump_data();
  }
}
#endif // DEBUG
