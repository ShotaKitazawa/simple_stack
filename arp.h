char *my_arp_ip_ntoa_r(u_int8_t ip[4],char *buf);
void print_ether_arp(struct ether_arp *ether_arp);
int ArpAddTable(u_int8_t mac[6],struct in_addr *ipaddr);
int ArpDelTable(struct in_addr *ipaddr);
int ArpSearchTable(struct in_addr *ipaddr,u_int8_t mac[6]);
int ArpShowTable();
int GetTargetMac(device_t *device,struct in_addr *daddr,u_int8_t dmac[6],int gratuitous);
int ArpSend(device_t *device,u_int16_t op,u_int8_t e_smac[6],u_int8_t e_dmac[6],u_int8_t smac[6],u_int8_t dmac[6],u_int8_t saddr[4],u_int8_t daddr[4]);
int ArpSendRequestGratuitous(device_t *device,struct in_addr *targetIp);
int ArpSendRequest(device_t *device,struct in_addr *targetIp);
int ArpCheckGArp(device_t *device);
int ArpRecv(device_t *device,struct ether_header *eh,u_int8_t *data,int len);
