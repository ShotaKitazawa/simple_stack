int print_icmp(struct icmp *icmp);
int IcmpSendEchoReply(device_t *device,struct ip *r_ip,struct icmp *r_icmp,u_int8_t *data,int len,int ip_ttl);
int IcmpSendEcho(device_t *device,struct in_addr *daddr,int seqNo,int size);
int IcmpSendDestinationUnreachable(device_t *device,struct in_addr *daddr,struct ip *ip,u_int8_t *data,int len);
int PingSend(device_t *device,struct in_addr *daddr,int size);
int IcmpRecv(device_t *device,u_int8_t *raw,int raw_len,struct ether_header *eh,struct ip *ip,u_int8_t *data,int len);
int PingCheckReply(struct ip *ip,struct icmp *icmp);
