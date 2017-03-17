# pcapserver

pcapserver is a tool to extract HTTP stream from a pcap file (tcpdump) and replay it.

It's particularly useful when you debug an embedded device, where you can't monitor the network traffic in real time,
or there is a rare bug that doesn't happen every time you run the programme - you can then record all the network activity by running a tcpdump on the device, and then grab the pcap and replay it on PC.

- To make it  
make debug/release

- To run it  
./pcapserver [pcapfile] 8080

- Example:  
./pcapserver ./testcases/mtv.mcu.pcap 8080

Then access  
http://localhost:8080/pcapserver

