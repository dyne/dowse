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
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <linux/sockios.h>

int ip2mac(char *ipaddr_type, char*ipaddr_value, char**macaddr) {

    return 0;
}

/**/
char *ethernet_mactoa(struct sockaddr *addr) {
  char*buff;
  buff=(char*)kore_malloc(3*6+1);
  unsigned char *ptr = (unsigned char *) addr->sa_data;

    sprintf(buff, "%02X:%02X:%02X:%02X:%02X:%02X", (ptr[0] & 0377),
            (ptr[1] & 0377), (ptr[2] & 0377), (ptr[3] & 0377), (ptr[4] & 0377),
            (ptr[5] & 0377));

    return (buff);

}

int convert_from_ipv4(char *ipaddr_value, char**mac_addr) {
    int sockef_fd;
    struct arpreq areq;
    struct sockaddr_in *sin;
    struct in_addr ipaddr;
    char buf[256];

    /* Get an internet domain socket. */
    if ((sockef_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        return (1);
    }

    /* Make the ARP request. */
    memset(&areq, 0, sizeof(areq));
    sin = (struct sockaddr_in *) &areq.arp_pa;
    sin->sin_family = AF_INET;

    if (inet_aton(ipaddr_value, &ipaddr) == 0) {
        fprintf(stderr, "-- Error: invalid numbers-and-dots IP address %s.\n",
                ipaddr_value);
        return (1);
    }

    sin->sin_addr = ipaddr;
    sin = (struct sockaddr_in *) &areq.arp_ha;
    sin->sin_family = ARPHRD_ETHER;

    /* Se non c'e' la eth0 ? */
    strncpy(areq.arp_dev, "eth0", 15);

    if (ioctl(sockef_fd, SIOCGARP, (caddr_t) &areq) == -1) {
        perror("-- Error: unable to make ARP request, error");
        return (1);
    }
    struct sockaddr_in * p;
    p=(struct sockaddr_in *) &(areq.arp_pa);

    inet_ntop(AF_INET,&(p->sin_addr),buf,sizeof(buf));

    printf("%s (%s) -> %s\n", ipaddr_value,
            inet_ntoa(p->sin_addr),
            ethernet_mactoa(&areq.arp_ha));
    return 0;
}

int convert_from_ipv6(char *ipaddr_value, char**mac_addr) {
    int s;
    struct arpreq areq;
    struct sockaddr_in6 *sin;
    struct in6_addr ipaddr;

    /* Get an internet domain socket. */
    if ((s = socket(AF_INET6, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        return (1);
    }

    /* Make the ARP request. */
    memset(&areq, 0, sizeof(areq));
    sin = (struct sockaddr_in6 *) &areq.arp_pa;
    sin->sin6_family = AF_INET6;

    if (inet_pton(AF_INET6,ipaddr_value, &ipaddr) == 0) {
        fprintf(stderr, "-- Error: invalid numbers-and-dots IP address %s.\n",
                ipaddr_value);
        return (1);
    }

    sin->sin6_addr= ipaddr;
    sin = (struct sockaddr_in6 *) &areq.arp_ha;
    sin->sin6_family = ARPHRD_ETHER;

    /* TODO definizione di device su cui è attestata webui */
    strncpy(areq.arp_dev, "eth0", 15);

    if (ioctl(s, SIOCGARP, (caddr_t) &areq) == -1) {
        perror("-- Error: unable to make ARP request, error");
        return (1);
    }
    char buf[256];
    struct sockaddr_in6 * p;
    p=(struct sockaddr_in6 *) &(areq.arp_pa);
    inet_ntop(AF_INET6,&(p->sin6_addr),buf,sizeof(buf));

    printf("%s (%s) -> %s\n", ipaddr_value,
               buf,
               ethernet_mactoa(&areq.arp_ha));
    return 0;
}
