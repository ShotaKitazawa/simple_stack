#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <netinet/ip_icmp.h>
#include <netinet/if_ether.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <linux/if.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <pthread.h>
#include "sock.h"
#include "ether.h"
#include "arp.h"
#include "ip.h"
#include "icmp.h"
#include "udp.h"
#include "tcp.h"
#include "dhcp.h"
#include "param.h"
#include "cmd.h"

int EndFlag = 0;

device_t *device;

PARAM Param;

void *MyEthThread(void *arg) {
  int nready;
  struct pollfd targets[1];
  u_int8_t buf[2048];
  int len;

  targets[0].fd = device->sock;
  targets[0].events = POLLIN | POLLERR;

  while (EndFlag == 0) {
    switch ((nready = poll(targets, 1, 1000))) {
      case -1:
        if (errno != EINTR) {
          perror("poll");
        }
        break;
      case 0:
        break;
      default:
        if (targets[0].revents & (POLLIN | POLLERR)) {
          if ((len = read(device->sock, buf, sizeof(buf))) <= 0) {
            perror("read");
          } else {
            EtherRecv(device, buf, len);
          }
        }
        break;
    }
  }

  return (NULL);
}

void *StdInThread(void *arg) {
  int nready;
  struct pollfd targets[1];
  char buf[2048];

  targets[0].fd = fileno(stdin);
  targets[0].events = POLLIN | POLLERR;

  while (EndFlag == 0) {
    switch ((nready = poll(targets, 1, 1000))) {
      case -1:
        if (errno != EINTR) {
          perror("poll");
        }
        break;
      case 0:
        break;
      default:
        if (targets[0].revents & (POLLIN | POLLERR)) {
          fgets(buf, sizeof(buf), stdin);
          DoCmd(buf);
        }
        break;
    }
  }

  return (NULL);
}

void sig_term(int sig) { EndFlag = 1; }

int ending() {
  struct ifreq if_req;

  printf("ending\n");

  TcpAllSocketClose(device);

  if (Param.DhcpServer.s_addr != 0) {
    DhcpSendRelease(device);
  }

  if (device->sock != -1) {
    strcpy(if_req.ifr_name, Param.device);
    if (ioctl(device->sock, SIOCGIFFLAGS, &if_req) < 0) {
      perror("ioctl");
    }

    if_req.ifr_flags = if_req.ifr_flags & ~IFF_PROMISC;
    if (ioctl(device->sock, SIOCSIFFLAGS, &if_req) < 0) {
      perror("ioctl");
    }

    close(device->sock);
    device->sock = -1;
  }

  return (0);
}

int main(int argc, char *argv[]) {
  char buf1[80];
  int i, paramFlag;
  pthread_attr_t attr;
  pthread_t thread_id;

  SetDefaultParam();

  paramFlag = 0;
  for (i = 1; i < argc; i++) {
    if (ReadParam(argv[1]) == -1) {
      exit(-1);
    }
    paramFlag = 1;
  }
  if (paramFlag == 0) {
    if (ReadParam("./MyEth.ini") == -1) {
      exit(-1);
    }
  }

  printf("IP-TTL=%d\n", Param.IpTTL);
  printf("MTU=%d\n", Param.MTU);
  printf("MSS=%d\n", Param.MSS);

  srandom(time(NULL));

  IpRecvBufInit();
  if ((device = malloc(sizeof(device_t))) == NULL) {
    perror("malloc");
    exit(-1);
  }

  if ((device->sock = init_socket(Param.device)) == -1) {
    exit(-1);
  }

  printf("device=%s\n", Param.device);
  printf("++++++++++++++++++++++++++++++++++++++++\n");
  show_ifreq(Param.device);
  if (GetMacAddress(Param.device, Param.mymac) == -1) {
    printf("GetMacAddress:error");
  } else {
    printf("mymac=%s\n", my_ether_ntoa_r(Param.mymac, buf1));
  }
  printf("++++++++++++++++++++++++++++++++++++++++\n");

  printf("vmac=%s\n", my_ether_ntoa_r(Param.vmac, buf1));
  printf("vip=%s\n", inet_ntop(AF_INET, &Param.vip, buf1, sizeof(buf1)));
  printf("vmask=%s\n", inet_ntop(AF_INET, &Param.vmask, buf1, sizeof(buf1)));
  printf("gateway=%s\n",
         inet_ntop(AF_INET, &Param.gateway, buf1, sizeof(buf1)));
  printf("DHCP request lease time=%d\n", Param.DhcpRequestLeaseTime);

  signal(SIGINT, sig_term);
  signal(SIGTERM, sig_term);
  signal(SIGQUIT, sig_term);

  signal(SIGPIPE, SIG_IGN);

  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, 102400);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if (pthread_create(&thread_id, &attr, MyEthThread, NULL) != 0) {
    printf("pthread_create:error\n");
  }
  if (pthread_create(&thread_id, &attr, StdInThread, NULL) != 0) {
    printf("pthread_create:error\n");
  }

  if (Param.vip.s_addr == 0) {
    int count = 0;
    do {
      count++;
      if (count > 5) {
        printf("DHCP fail\n");
        return (-1);
      }
      DhcpSendDiscover(device);
      sleep(1);
    } while (Param.vip.s_addr == 0);
  }

  if (ArpCheckGArp(device) == 0) {
    printf("GArp check fail\n");
    return (-1);
  }

  while (EndFlag == 0) {
    sleep(1);
    if (Param.DhcpStartTime != 0) {
      DhcpCheck(device);
    }
  }

  ending();

  return (0);
}
