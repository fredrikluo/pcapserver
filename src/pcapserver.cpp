#include "pcapserver.h"
#include "define.h"
#include "http_stream_manager.h"
#include "http_server.h"
#include "pcapparser.h"
#include "script_parser.h"
#include "tcp_stream_manager.h"
#include "utility.h"
#include "resource.h"
#include <iostream>

using namespace std;

void pcapserver::start(const char* inputfile, int port) {
    http_stream_manager* p_http_stream_manager = new http_stream_manager;
    string datafile = inputfile;
    p_http_stream_manager->set_datafile(datafile);
    string err_msg;

    // Try to load the file.
    if (string::npos != datafile.find(".pcap") || string::npos != datafile.find(".cap")) {
        INFO_OUT("Start parsing pcap file %s", inputfile);
        pcap_parser _pcap_parser;
        if (!_pcap_parser.parse(p_http_stream_manager, inputfile)) {
            err_msg = _pcap_parser.get_error_msg();
        }
    } else if (string::npos != datafile.find(".xml")) {
        INFO_OUT("Start parsing script file %s", inputfile);
        script_parser _script_parser(p_http_stream_manager, inputfile);
    } else {
        err_msg = STR_UNKNOWN_EXT + datafile;
    }

    p_http_stream_manager->set_error_msg(err_msg);

    // Although we might have failed to load any thing, we still have to
    // start a web server and tell the browser.
    //
    // For this programme might run on a remote server by a script.
    INFO_OUT("Start httpserver at port %d", port);
    http_server* p_http_server = new http_server(p_http_stream_manager);
    p_http_server->start(port);

    delete p_http_server;
    delete p_http_stream_manager;
}
