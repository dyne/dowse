/*  Dowse - embedded WebUI based on Kore.io
 *
 *  (c) Copyright 2016 Dyne.org foundation, Amsterdam
 *  Written by Nicola Rossi <nicola@dyne.org>
 *
 * This source code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License as published
 * by the Free Software Foundation; either version 3 of the License,
 * or (at your option) any later version.
 *
 * This source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * Please refer to the GNU Public License for more details.
 *
 * You should have received a copy of the GNU Public License along with
 * this source code; if not, write to:
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <webui.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <linux/sockios.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

char *to_upper(char*str){
  int i;
  for (i=0;i<strlen(str);i++) {
    toupper(str[i]);
  }
  return str;
}

int ip2mac(char *ipaddr_type, char*ipaddr_value, char*macaddr,attributes_set_t *ptr_attr) {
    func("converting from %s %s on %s",ipaddr_type,ipaddr_value,getenv("interface"));

    char *address=getenv("address");
      if (((address!=NULL)&&(strcmp(ipaddr_value,address)==0))||(strcmp(ipaddr_value,"127.0.0.1")==0))  {
      sprintf(macaddr,"00:00:00:00:00:00");
      return KORE_RESULT_OK;
    } else {
      int rv;
      if (strcmp(ipaddr_type,"ipv4")==0) {
	if (strcmp(ipaddr_value,"127.0.0.1")==0) {
	  return KORE_RESULT_ERROR;
	}
	rv=convert_from_ipv4(ipaddr_value,macaddr,ptr_attr);
      } else {
	rv=convert_from_ipv6(ipaddr_value,macaddr,ptr_attr);
      }
      to_upper(macaddr);
      return rv;
    }
}

/**/
void ethernet_mactoa(struct sockaddr *addr,char*buff) {

  unsigned char *ptr = (unsigned char *) addr->sa_data;

    sprintf(buff, "%02X:%02X:%02X:%02X:%02X:%02X", (ptr[0] & 0377),
            (ptr[1] & 0377), (ptr[2] & 0377), (ptr[3] & 0377), (ptr[4] & 0377),
            (ptr[5] & 0377));


}

int convert_from_ipv4(char *ipaddr_value, char *mac_addr,attributes_set_t *ptr_attr) {
    int socket_fd;
    struct arpreq areq;
    struct sockaddr_in *sin;
    struct in_addr ipaddr;
    char buf[256];

    /* Get an internet domain socket. */
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        char m[1024];
        snprintf(m,sizeof(m),"Sorry but during IP-ARP conversion I cannot open a socket [%s]",
                strerror(errno));

        return (KORE_RESULT_ERROR);
    }

    /* Make the ARP request. */
    memset(&areq, 0, sizeof(areq));
    sin = (struct sockaddr_in *) &areq.arp_pa;
    sin->sin_family = AF_INET;

    if (inet_aton(ipaddr_value, &ipaddr) == 0) {
        char m[256];
        snprintf(m,sizeof(m),"Sorry but during IP-ARP conversion I cannot execute inet_aton(%s) due to (%s)",
                ipaddr_value,strerror(errno));
        webui_add_error_message(ptr_attr,m);
        close(socket_fd);
        return (KORE_RESULT_ERROR);
    }

    sin->sin_addr = ipaddr;
    sin = (struct sockaddr_in *) &areq.arp_ha;
    sin->sin_family = ARPHRD_ETHER;

    /* TODO Se non c'e' la eth0 ? */
    char *dev=getenv("interface");
    if (dev==NULL) {
        dev="eth0";
    }

    strncpy(areq.arp_dev, dev, 15);

    if (ioctl(socket_fd, SIOCGARP, (caddr_t) &areq) == -1) {
        char m[256];
         snprintf(m,sizeof(m), "-- Error: unable to make ARP request for IP [%s], error on device [%s] due to [%s]",ipaddr_value,dev,strerror(errno));
         webui_add_error_message(ptr_attr,m);
         close(socket_fd);
         return (KORE_RESULT_ERROR);
    }
    struct sockaddr_in * p;
    p=(struct sockaddr_in *) &(areq.arp_pa);

    inet_ntop(AF_INET,&(p->sin_addr),buf,sizeof(buf));

    ethernet_mactoa(&areq.arp_ha,mac_addr);

    func("Conversion from %s (%s) -> %s\n", ipaddr_value,
            inet_ntoa(p->sin_addr),
            mac_addr
            );

    close(socket_fd);
    return KORE_RESULT_OK;
}

int convert_from_ipv6(char *ipaddr_value, char *mac_addr,attributes_set_t *ptr_attr) {
    int socket_fd;
    struct arpreq areq;
    struct sockaddr_in6 *sin;
    struct in6_addr ipaddr;

    /* Get an internet domain socket. */
    if ((socket_fd = socket(AF_INET6, SOCK_DGRAM, 0)) == -1) {
        char m[1024];
        snprintf(m,sizeof(m),"Sorry but during IP-ARP conversion I cannot open a socket [%s]",
                strerror(errno));
        webui_add_error_message(ptr_attr,m);

        return (1);
    }

    /* Make the ARP request. */
    memset(&areq, 0, sizeof(areq));
    sin = (struct sockaddr_in6 *) &areq.arp_pa;
    sin->sin6_family = AF_INET6;

    if (inet_pton(AF_INET6,ipaddr_value, &ipaddr) == 0) {
        char m[256];
        snprintf(m,sizeof(m),"Sorry but during IP-ARP conversion I cannot execute inet_aton(%s) due to (%s)",
                ipaddr_value,strerror(errno));

        webui_add_error_message(ptr_attr,m);
        close(socket_fd);
        return (1);
    }

    sin->sin6_addr= ipaddr;
    sin = (struct sockaddr_in6 *) &areq.arp_ha;
    sin->sin6_family = ARPHRD_ETHER;

    /* TODO definizione di device su cui è attestata webui */
    char *dev=getenv("interface");
    if (dev==NULL) {
        dev="eth0";
    }

    strncpy(areq.arp_dev, dev, 15);

    if (ioctl(socket_fd, SIOCGARP, (caddr_t) &areq) == -1) {
        char m[256];
         webui_add_error_message(ptr_attr,"socket");
         snprintf(m,sizeof(m), "-- Error: unable to make ARP request for IP [%s], error on device [%s] due to [%s]",ipaddr_value,dev,strerror(errno));
         close(socket_fd);
         return (1);
    }
    char buf[256];
    struct sockaddr_in6 * p;
    p=(struct sockaddr_in6 *) &(areq.arp_pa);
    inet_ntop(AF_INET6,&(p->sin6_addr),buf,sizeof(buf));

    ethernet_mactoa(&areq.arp_ha,mac_addr);
    close(socket_fd);

    func("Conversion form %s  -> %s\n", ipaddr_value,
                mac_addr
                );
    return 0;
}
