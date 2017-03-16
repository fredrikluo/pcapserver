#include "pcap.h"

#include "http_stream_manager.h"
#include "pcapparser.h"
#include "tcp_stream_manager.h"
#include "utility.h"
#include "resource.h"

#include <vector>

using namespace std;

pcap_parser::pcap_parser() {

}

bool pcap_parser::parse(http_stream_manager* http_stream_mgr, const char* filename) {
    if (!(filename && http_stream_mgr)) {
        ERROR_OUT("NO pcap file  or invalid http_stream_manager");
        return false;
    }

    static char error_txt[255];

    pcap_t* pcap_file = pcap_open_offline(filename, error_txt);

    if (!pcap_file) {
        ERROR_OUT(error_txt);
        m_err_msg = STR_HTML_CANT_PARSE;
        return false;
    }

    DEBUG_TRACE("opened pcap file successfully");

    pcap_pkthdr packet_header;
    const u_char* packet_buffer = NULL;

    int link_layer_size = get_linklayer_length(pcap_datalink(pcap_file));
    DEBUG_TRACE("The link layer size is %d \n", link_layer_size);

    if (link_layer_size == -1) {
        m_err_msg = STR_HTML_LINKLAYER_NOT_SUPPORTED;
        ERROR_OUT("The link layer protocol is not supported\n");
        return false;
    }

    tcp_stream_manager tcp_mgr;
    while (NULL != (packet_buffer = pcap_next(pcap_file, &packet_header))) {
        tcp_mgr.add_packet(packet_buffer + link_layer_size, packet_header.len - link_layer_size);
    }

    tcp_mgr.process();

    const vector<tcp_stream>& tcp_treams = tcp_mgr.get_tcpstreams();

    if (!tcp_treams.size()) {
        m_err_msg = STR_HTML_NO_COMPLETE_TCP_STREAM;
    }

    for (vector<tcp_stream>::const_iterator itr = tcp_treams.begin();
         itr != tcp_treams.end();
         itr++) {

        vector<u_char> request, response;
        for (list<tcp_packet>::const_iterator itr_req = itr->get_tcp_streamP12().begin();
             itr_req != itr->get_tcp_streamP12().end();
             itr_req++) {
            vec_append(request, itr_req->get_data());
        }

        for (list<tcp_packet>::const_iterator itr_resp = itr->get_tcp_streamP21().begin();
             itr_resp != itr->get_tcp_streamP21().end();
             itr_resp++) {
            vec_append(response, itr_resp->get_data());
        }

        http_stream_mgr->add_packet(request, response);
    }

    if (pcap_file) {
        DEBUG_TRACE("close pcap file");
        pcap_close(pcap_file);
    }

    return true;
}

int pcap_parser::get_linklayer_length(int linktype) {
    switch (linktype) {
    case DLT_IEEE802_11:
    case DLT_PRISM_HEADER:
    case DLT_IEEE802_11_RADIO_AVS:
    case DLT_IEEE802_11_RADIO:
        return -1;

    case DLT_RAW:
    case DLT_FRELAY:
    case DLT_LTALK:
    case DLT_MFR:
    case DLT_JUNIPER_ATM2:
    case DLT_ATM_RFC1483:
    case DLT_ATM_CLIP:
    case DLT_SUNATM:
    case DLT_LINUX_IRDA:
    case DLT_DOCSIS:
    case DLT_JUNIPER_SERVICES:
    case DLT_JUNIPER_VP:
    case DLT_MTP2:
    case DLT_MTP2_WITH_PHDR:
    case DLT_LINUX_LAPD:
    case DLT_CAN20B:
    case DLT_JUNIPER_ES:
        return 0;

    case DLT_NULL:
    case DLT_LOOP:
    case DLT_PPP:
    case DLT_PPP_PPPD:
    case DLT_C_HDLC:
    case DLT_PPP_SERIAL:
    case DLT_JUNIPER_MFR:
    case DLT_JUNIPER_MLFR:
    case DLT_JUNIPER_MLPPP:
    case DLT_JUNIPER_PPP:
    case DLT_JUNIPER_CHDLC:
    case DLT_JUNIPER_FRELAY:
    case DLT_JUNIPER_ATM1:
        return 4;

    case DLT_JUNIPER_PPPOE_ATM:
    case DLT_ARCNET:
        return 6;

    case DLT_PPP_ETHER:
    case DLT_ARCNET_LINUX:
        return 8;

    case DLT_ENC:
    case DLT_JUNIPER_GGSN:
    case DLT_JUNIPER_MONITOR:
        return 12;

    case DLT_FDDI:
        return 13; //this might be wrong

    case DLT_EN10MB:
    case DLT_IEEE802:
    case DLT_JUNIPER_PPPOE:
    case DLT_JUNIPER_ETHER:
        return 14;

    case DLT_LINUX_SLL: /* fake header for Linux cooked socket */
    case DLT_SLIP:
    case DLT_IP_OVER_FC:
        return 16;

    case DLT_APPLE_IP_OVER_IEEE1394:
        return 18;

    case DLT_SLIP_BSDOS:
    case DLT_PPP_BSDOS:
        return 24;

    case DLT_SYMANTEC_FIREWALL:
        return 44;
    default:
        break;
    }

    return -1;
}
