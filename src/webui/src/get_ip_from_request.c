/*
 * get_ip_from_request.c
 *
 *  Created on: 15 nov 2016
 *      Author: nop
 */

#include <webui.h>

int get_ip_from_request(struct http_request *req,
        char* ipaddr_type,int sizeof_ipaddr_type,
        char *ipaddr_value,int sizeof_ipaddr_value
        ) {

    char socket_link_path[256];
    sprintf(socket_link_path,"/proc/%d/fd/%d",getpid(),req->owner->fd);

    char socket_path[256];
    readlink(socket_link_path, socket_path, sizeof(socket_path));

    int inode_socket;
    sscanf(socket_path,"socket:[%d]",&inode_socket);

    int rv;
    rv=scan_proc_net_tcp_file(inode_socket,ipaddr_type,sizeof_ipaddr_type,ipaddr_value,sizeof_ipaddr_value);

    kore_log(LOG_DEBUG,"%s Hello your IP is [%s] and your address is [%s]",
            __where_i_am__,ipaddr_type,ipaddr_value);



    return (KORE_RESULT_OK);
}
