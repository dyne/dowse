/*  Dowse - ip2mac translation function
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
#include "../libdowse/dowse.h"

#define IP2MAC_ERROR (1)
#define IP2MAC_RESULT_OK (0)

char *to_upper(char*str){
  int i;
  for (i=0;i<strlen(str);i++) {
    toupper(str[i]);
  }
  return str;
}

int convert_from_ipv4(char *ipaddr_value, char *mac_addr);
int convert_from_ipv6(char *ipaddr_value, char *mac_addr);

int ip2mac(char *ipaddr_type, char*ipaddr_value, char*macaddr) {
    char loc_ipaddr_type[6];

    if ((ipaddr_type == NULL)||strcmp(ipaddr_type,"")==0) {
        int a, b, c, d;
        func("sscanf from %s on %s", ipaddr_value, getenv("interface"));
        int rv = sscanf(ipaddr_value, "%d.%d.%d.%d", &a, &b, &c, &d);

        if (rv == 4) {
            sprintf(loc_ipaddr_type, "ipv4");
        } else {
            sprintf(loc_ipaddr_type, "ipv6");
        }
    } else {
        snprintf(loc_ipaddr_type, sizeof(loc_ipaddr_type), "%s", ipaddr_type);
    }
    func("converting from %s  %s on %s", loc_ipaddr_type, ipaddr_value,
            getenv("interface"));

    int rv;
    char *address=getenv("address");
    if (
	((address!=NULL)&&(strcmp(ipaddr_value,getenv("address"))==0))
	 || (strcmp(ipaddr_value,"127.0.0.1")==0))  {
      sprintf(macaddr,"00:00:00:00:00:00");
      return IP2MAC_RESULT_OK;
    } else {
      if (strcmp(loc_ipaddr_type, "ipv4") == 0) {
	rv=convert_from_ipv4(ipaddr_value, macaddr);
      } else {
	rv=convert_from_ipv6(ipaddr_value, macaddr);
      }
      if (ipaddr_type!=NULL){
          sprintf(ipaddr_type,loc_ipaddr_type);
      }
      to_upper(macaddr);
      return rv;
    }


}

/**/
void ethernet_mactoa(struct sockaddr *addr, char*buff) {

    unsigned char *ptr = (unsigned char *) addr->sa_data;

    sprintf(buff, "%02X:%02X:%02X:%02X:%02X:%02X", (ptr[0] & 0377),
            (ptr[1] & 0377), (ptr[2] & 0377), (ptr[3] & 0377), (ptr[4] & 0377),
            (ptr[5] & 0377));
}

int convert_from_ipv4(char *ipaddr_value, char *mac_addr) {
    int socket_fd;
    struct arpreq areq;
    struct sockaddr_in *sin;
    struct in_addr ipaddr;
    char buf[256];
    
    /* Get an internet domain socket. */
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        err("Sorry but during IP-ARP conversion I cannot open a socket [%s]",
                strerror(errno));

        return (IP2MAC_ERROR);
    }

    /* Make the ARP request. */
    memset(&areq, 0, sizeof(areq));
    sin = (struct sockaddr_in *) &areq.arp_pa;
    sin->sin_family = AF_INET;

    if (inet_aton(ipaddr_value, &ipaddr) == 0) {
        err(
                "Sorry but during IP-ARP conversion I cannot execute inet_aton(%s) due to (%s)",
                ipaddr_value, strerror(errno));

        close(socket_fd);
        return (IP2MAC_ERROR);
    }

    sin->sin_addr = ipaddr;
    sin = (struct sockaddr_in *) &areq.arp_ha;
    sin->sin_family = ARPHRD_ETHER;

    /* TODO Se non c'e' la eth0 ? */
    char *dev = getenv("interface");
    if (dev == NULL) {
        dev = "eth0";
    }

    strncpy(areq.arp_dev, dev, 15);

    if (ioctl(socket_fd, SIOCGARP, (caddr_t) &areq) == -1) {
        func("-- Error: unable to make ARP request for IP [%s], error on device [%s] due to [%s]",
                ipaddr_value, dev, strerror(errno));
        close(socket_fd);
        return (IP2MAC_ERROR);
    }
    func("%s %d", __FILE__, __LINE__);

    struct sockaddr_in * p;
    p = (struct sockaddr_in *) &(areq.arp_pa);
    func("%s %d", __FILE__, __LINE__);

    inet_ntop(AF_INET, &(p->sin_addr), buf, sizeof(buf));

    ethernet_mactoa(&areq.arp_ha, mac_addr);

    func("Conversion from %s (%s) -> %s\n", ipaddr_value,
            inet_ntoa(p->sin_addr), mac_addr);

    close(socket_fd);
    return IP2MAC_RESULT_OK;
}

int convert_from_ipv6(char *ipaddr_value, char *mac_addr) {
    int socket_fd;
    struct arpreq areq;
    struct sockaddr_in6 *sin;
    struct in6_addr ipaddr;

    /* Get an internet domain socket. */
    if ((socket_fd = socket(AF_INET6, SOCK_DGRAM, 0)) == -1) {
        err("Sorry but during IP-ARP conversion I cannot open a socket [%s]",
                strerror(errno));
        return (1);
    }

    /* Make the ARP request. */
    memset(&areq, 0, sizeof(areq));
    sin = (struct sockaddr_in6 *) &areq.arp_pa;
    sin->sin6_family = AF_INET6;

    if (inet_pton(AF_INET6, ipaddr_value, &ipaddr) == 0) {
        err(
                "Sorry but during IP-ARP conversion I cannot execute inet_aton(%s) due to (%s)",
                ipaddr_value, strerror(errno));

        close(socket_fd);
        return (1);
    }

    sin->sin6_addr = ipaddr;
    sin = (struct sockaddr_in6 *) &areq.arp_ha;
    sin->sin6_family = ARPHRD_ETHER;

    /* TODO definizione di device su cui e' attestata webui */
    char *dev = getenv("interface");
    if (dev == NULL) {
        dev = "lo";
    }

    strncpy(areq.arp_dev, dev, 15);

    if (ioctl(socket_fd, SIOCGARP, (caddr_t) &areq) == -1) {
        err(
                "-- Error: unable to make ARP request for IP [%s], error on device [%s] due to [%s]",
                ipaddr_value, dev, strerror(errno));
        return (1);
    }
    char buf[256];
    struct sockaddr_in6 * p;
    p = (struct sockaddr_in6 *) &(areq.arp_pa);
    inet_ntop(AF_INET6, &(p->sin6_addr), buf, sizeof(buf));

    ethernet_mactoa(&areq.arp_ha, mac_addr);

    func("Conversion form %s  -> %s\n", ipaddr_value, mac_addr);
    close(socket_fd);
    return 0;
}
