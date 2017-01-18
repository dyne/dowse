/*
 * get_ip_from_request.c
 *
 *  Created on: 15 nov 2016
 *      Author: nop
 */

#include <webui.h>

int get_ip_from_request(struct http_request *req,
        char ** ipaddr_type,
        char **ipaddr_value
        ) {

#ifndef PROC_FILE_FOR_NETWORK
    char _ip[64];
    memset(_ip,0,sizeof(_ip));
    const char *s;
#define _SIZE 16
    (*ipaddr_type)=kore_malloc(_SIZE);

    if (req->owner->addrtype==AF_INET) {
        s=inet_ntop(req->owner->addrtype,
                &(req->owner->addr.ipv4.sin_addr),
                _ip,sizeof(_ip)
        );
        snprintf(*ipaddr_type,_SIZE,"ipv4");
    } else {
        s=inet_ntop(req->owner->addrtype,
                &(req->owner->addr.ipv6.sin6_addr),
                _ip,sizeof(_ip)
        );
        snprintf(*ipaddr_type,_SIZE,"ipv6");
    }
    (*ipaddr_value)=kore_malloc(strlen(_ip)+1);
    sprintf( *ipaddr_value,"%s",_ip);

    func("%s connection from %s",__where_i_am__,s);

#else
    char socket_link_path[256];
    sprintf(socket_link_path,"/proc/%d/fd/%d",getpid(),req->owner->fd);

    char socket_path[256];
    readlink(socket_link_path, socket_path, sizeof(socket_path));

    int inode_socket;
    sscanf(socket_path,"socket:[%d]",&inode_socket);

    int rv;
    rv=scan_proc_net_tcp_file(inode_socket,ipaddr_type,sizeof_ipaddr_type,ipaddr_value,sizeof_ipaddr_value);

    func("%s Hello your IP is [%s] and your address is [%s]",
            __where_i_am__,ipaddr_type,ipaddr_value);
#endif


    return (KORE_RESULT_OK);
}
