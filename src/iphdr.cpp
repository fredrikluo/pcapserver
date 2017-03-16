#include "iphdr.h"
#include "utility.h"
#include <iostream>

const unsigned char* iphdr::operator=(const unsigned char* buffer)
{
    version = ((buffer[0] & 0xf0) >> 4) & 0x0f;
    ihl = (buffer[0] & 0x0f) * 4;
    tos = buffer[1];
    tot_len = reverse_endian(*((unsigned short int*)&buffer[2]));
    id = reverse_endian(*((unsigned short int*)&buffer[4]));
    frag_off = (buffer[6] >> 5) & 0x07;
    ttl = buffer[8];
    protocol = buffer[9];
    check = reverse_endian(*((unsigned short int*)&buffer[10]));
    saddr = reverse_endian(*((unsigned int*)&buffer[12]));
    daddr = reverse_endian(*((unsigned int*)&buffer[16]));
    isvalid = ((short)check) == comput_checksum(buffer, ihl);
    return buffer;
}

short iphdr::comput_checksum(const unsigned char* buffer, int len)
{
    unsigned long csum = 0;
    assert((len & 0x1) == 0);
    for (int i = 0; i < len; i += 2) {
        if (i == 11 || i == 10)
            continue;

        csum += (unsigned short)(buffer[i] << 8) + buffer[i + 1];
    }

    csum = (csum >> 16) + (csum & 0xffff);
    csum += csum >> 16;
    csum = ~csum;
    return csum;
}
