#include "tcphdr.h"
#include "utility.h"
#include <iostream>

const unsigned char* tcphdr::operator=(const unsigned char* buffer)
{
    sport = reverse_endian(*((unsigned short*)&buffer[0])); /* source port */
    dport = reverse_endian(*((unsigned short*)&buffer[2])); /* destination port */
    seq = reverse_endian(*((unsigned int*)&buffer[4])); /* sequence number */
    ack = reverse_endian(*((unsigned int*)&buffer[8])); /* acknowledgement number */
    th_x2 = 0; /* (unused) */
    th_off = ((buffer[12] & 0xf0) >> 4) * 4; /* data offset */
    th_flags = buffer[13];
    th_win = reverse_endian(*((unsigned short*)&buffer[14])); /* window */
    th_sum = reverse_endian(*((unsigned short*)&buffer[16])); /* checksum */
    th_urp = 0; /* urgent pointer */

    return buffer;
}
