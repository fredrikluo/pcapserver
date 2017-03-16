#include "http_responder.h"

#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "utility.h"

using namespace std;

bool http_responder::send_data() {
  if (m_response_list.empty())
    return true;

  DEBUG_TRACE("Begin to send data with socket %d", m_sock);

  size_t send_length = m_response_list[m_send_index].size() - m_offset;

  if (!send_length &&
      (++m_send_index) < static_cast<int>(m_response_list.size())) {
    m_offset = 0;
    send_length = m_response_list[m_send_index].size();
    DEBUG_TRACE("Go to next packet with socket %d", m_sock);
  }

  if (send_length) {
    u_char *sendbuffer = new u_char[send_length];
    memset(sendbuffer, 0, send_length);

    for (u_int i = m_offset; i < m_response_list[m_send_index].size(); i++)
      sendbuffer[i - m_offset] = m_response_list[m_send_index][i];

    int send_cout = send(m_sock, sendbuffer, send_length, MSG_NOSIGNAL);

    m_offset += send_cout;
    delete[] sendbuffer;

    if (send_cout < 0) {
      INFO_OUT(
          "Send terminated with socket %d, num %d, percent %d %% , url: %s",
          m_sock, m_send_index,
          m_offset * 100 / m_response_list[m_send_index].size(),
          m_url_list[m_send_index].c_str());
      return false;
    }
  } else {
    INFO_OUT("Send with socket %d finished", m_sock);
    m_response_list.clear();
    m_offset = -1;
    return false;
  }

  INFO_OUT("Send with socket %d, num %d, percent %d %% , url: %s", m_sock,
           m_send_index, m_offset * 100 / m_response_list[m_send_index].size(),
           m_url_list[m_send_index].c_str());

  return true;
}

bool http_responder::recv_data() {
  DEBUG_TRACE("Begin to recv data");
  static char buffer[2048];

  int receive_count = recv(m_sock, buffer, sizeof(buffer), 0);
  if (receive_count < 0) {
    if (errno != EWOULDBLOCK) {
      ERROR_OUT("  recv() failed");
      return true;
    }

    ERROR_OUT("  recv() failed with EWOULDBLOCK");
    return false;
  }

  if (receive_count == 0) {
    return false;
  }

  // find the url in request packet
  for (int i = 0; i < receive_count; i++)
    m_revbuffer.push_back(buffer[i]);

  return true;
}

void http_responder::set_response(std::vector<u_char> &response,
                                  std::string &url) {
  m_response_list.push_back(response);
  m_url_list.push_back(url);
}
