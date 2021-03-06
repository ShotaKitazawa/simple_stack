int print_tcp(struct tcphdr *tcp);
int print_tcp_optpad(unsigned char *data,int size);
char *TcpStatusStr(int status);
u_int16_t TcpChecksum(struct in_addr *saddr,struct in_addr *daddr,u_int8_t proto,u_int8_t *data,int len);
int TcpAddTable(u_int16_t port);
int TcpSearchTable(u_int16_t port);
int TcpShowTable();
u_int16_t TcpSearchFreePort();
int TcpSocketListen(u_int16_t port);
int TcpSocketClose(u_int16_t port);
int TcpSendSyn(device_t *device,int no,int ackFlag);
int TcpSendFin(device_t *device,int no);
int TcpSendRst(device_t *device,int no);
int TcpSendAck(device_t *device,int no);
int TcpSendRstDirect(device_t *device,struct ether_header *r_eh,struct ip *r_ip,struct tcphdr *r_tcp);
int TcpConnect(device_t *device,u_int16_t sport,struct in_addr *daddr,u_int16_t dport);
int TcpClose(device_t *device,u_int16_t sport);
int TcpReset(device_t *device,u_int16_t sport);
int TcpAllSocketClose(device_t *device);
int TcpSendData(device_t *device,u_int16_t sport,u_int8_t *data,int len);
int TcpSend(device_t *device,u_int16_t sport,u_int8_t *data,int len);
int TcpRecv(device_t *device,struct ether_header *eh,struct ip *ip,u_int8_t *data,int len);
