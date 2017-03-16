#ifndef PCAPSERVER_H
#define PCAPSERVER_H

/**
 * Main class, start to serve as a http server
 * based on input file
 */
class pcapserver {
public:
    pcapserver() {
    }
    /**
     *  @param inputfile must be a .pcap or .xml file.
     *
     *  This function will start a http server on port
     *
     *  use /pcapserver to access index page
     */
    void start(const char* inputfile, int port);

    virtual ~pcapserver() {}
};

#endif // PCAPSERVER_H
