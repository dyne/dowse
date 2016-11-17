/*
 * scan_proc_net_tcp_file.c
 *
 *  Created on: 15 nov 2016
 *      Author: nop
 */

#include <stdio.h>
#include <string.h>


int _scan_proc_net_tcp_file(char *proc_file,int inode_socket,char *ipaddr_value,int sizeof_ipaddr_value){
    char line[256],_str_inode[256];
    char *field,*to_parse,*_ip_addr;
    FILE *fp;
    fp=fopen(proc_file,"r");
    sprintf(_str_inode,"%d",inode_socket);
    int first_row=1;
    while (fgets(line,sizeof(line),fp)){
        if (first_row){
            first_row=0;
            continue;
        }
        to_parse=line;
        int column=0;
        do {
            field=strtok(to_parse," :");
            to_parse=NULL;
            column++;

            switch(column) {
                case (3) :
                        _ip_addr=field;
                        break;
                case (14):
                   if ((strcmp(_str_inode,field)==0)) {
                       snprintf(ipaddr_value,sizeof_ipaddr_value,"%s",_ip_addr);
                       fclose(fp);
                       return 1;
                   } else {
                       field=NULL;
                   }
            }
        } while (field!=0);
    }

    fclose(fp);
    return 0;

}

void _convert_from_reverse_hex_to_ipv4(char *_str,char*ipaddr_value,int sizeof_ipaddr_value){
    int k;
    int a,b,c,d;

    sscanf(_str,"%2x%2x%2x%2x",&a,&b,&c,&d);
    snprintf(ipaddr_value,sizeof_ipaddr_value,"%d.%d.%d.%d",
            d,c,b,a
    );
}

void _convert_from_reverse_hex_to_ipv6(char *_str,char*ipaddr_value,int sizeof_ipaddr_value){
    int k;
    int a,b,c,d,e,f,g,h;

    //TODO Test
    sscanf(_str,"%4x%4x%4x%4x%4x%4x%4x%4x",&a,&b,&c,&d,&e,&f,&g,&h);
    snprintf(ipaddr_value,sizeof_ipaddr_value,"%d:%d:%d:%d:%d:%d:%d:%d",
            h,g,f,e,d,c,b,a
    );
}


int scan_proc_net_tcp_file(int inode_socket,char *ipaddr_type,int sizeof_ipaddr_type,char*ipaddr_value,int sizeof_ipaddr_value){
    char _str[256];
    int rv=_scan_proc_net_tcp_file("/proc/net/tcp",inode_socket,_str,sizeof(_str));

    if (rv!=0) {
        sprintf(ipaddr_type,"ipv4");
        _convert_from_reverse_hex_to_ipv4(_str,ipaddr_value,sizeof_ipaddr_value);
        return rv;
    }

     rv=_scan_proc_net_tcp_file("/proc/net/tcp6",inode_socket,ipaddr_value,sizeof_ipaddr_value);
    if (rv!=0) {
        sprintf(ipaddr_type,"ipv6");
        _convert_from_reverse_hex_to_ipv6(_str,ipaddr_value,sizeof_ipaddr_value);
        return rv;
    }

    sprintf(ipaddr_type,"wrong");
}
