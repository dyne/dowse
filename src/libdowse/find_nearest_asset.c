/*  Dowse - find nearest asset
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
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <string.h>

#include <dirent.h>
#include "find_nearest_asset.h"

/********/


int print_file_and_link(char*prefix,struct dirent *entry,void*data) {
    if (entry->d_type==DT_REG) {
        fprintf(stderr,"[%s/%s]\n",prefix,entry->d_name);
        return 0;
    }
    if (entry->d_type==DT_LNK) {
        fprintf(stderr,"[%s/%s]->\n",prefix,entry->d_name);
        return 0;
    }
    return 0;
}

char filtered(char a){
    if (a=='.') return '_';
    if (a=='/') return '_';
    return a;
}
/*
int asset_name_distance(char* a,char*b) {
    int rv=0;
    int i;

    for ( i=0;(i<(strlen(a))&&(i<(strlen(b))));) {
       if (filtered(a[i])==filtered(b[i])) {
           rv++;
           i++;
           continue;
       }
       break;
    }

    int ia=strlen(a)-1;
    int ib=strlen(b)-1;
    for ( ; ia>=0 && ib >= 0 ; ) {
       if (filtered(a[ia])==filtered(b[ib])) {
           rv++;
           ia--;
           ib--;
           continue;
       }
       break;
    }

    return rv;
}

int print_nearest_name(char*prefix,struct dirent *entry,void*data) {
    char entry_file_name[256];
    nearest_filename* p;

    if ((entry->d_type==DT_REG) || (entry->d_type==DT_LNK)) {
        snprintf(entry_file_name,sizeof(entry_file_name),"%s/%s",prefix,entry->d_name);
        p=(nearest_filename*) data;

        int nv=p->utility_function(entry_file_name,p->name_to_search);
        if (nv > p->max_point) {
            snprintf(p->nearest,sizeof(p->nearest),"%s",entry_file_name);
            p->max_point=nv;
        }
    }
    return 0;
}

*/
/**/
int span_directory(char*dir_path,char*prefix,int (*to_execute)(char*,struct dirent*,void*),void*data) {
    DIR*dirp=opendir(dir_path);
    if (dirp==NULL) {
        return 0;
    }
    struct dirent entry;
    struct dirent *result=&entry;

    while (1) {
        readdir_r(dirp, &entry, &result);

        if (result!=NULL) {
            if (strcmp(entry.d_name,".")==0) {
             //   fprintf(stderr,"DIR [%s] \n",entry.d_name);
                continue;
            }
            if (strcmp(entry.d_name,"..")==0) {
            //    fprintf(stderr,"DIR [%s] \n",entry.d_name);
                continue;
            }
            if (entry.d_type==DT_DIR) {
                char use_prefix[256];
                sprintf(use_prefix,"%s/%s",prefix,entry.d_name);
                span_directory(entry.d_name,use_prefix,to_execute,data);
                continue;
            }
            if ((entry.d_type==DT_REG) ||(entry.d_type==DT_LNK)) {
                to_execute(prefix,&entry,data);
                continue;
            }
        } else {
            break;
        }
    }

    closedir(dirp);
    return 0;
}

void init_data_to_search(nearest_filename*p,char*val,int (*util_function)(char*,char*)){
    snprintf(p->name_to_search,
            sizeof(p->name_to_search),
            "%s",val);
    (p->nearest)[0]=0;
    p->max_point=-1;
    p->utility_function=util_function;
}


int example_to_use() {
    return span_directory(".","./",print_file_and_link,NULL);
    /* another example is located in the WebUI project into src/template.c */
}

/*
int find_nearest_asset_and_load_template(char *template_name, int len, struct template_t *tmpl) {


    struct nearest_filename data_to_search;

    init_data_to_search(&data_to_search,template_name,asset_name_distance);

    span_directory(".","./",print_nearest_name,&data_to_search);

    func("Nearest of \n[%s]\n[%s]\n %d\n",
            data_to_search.name_to_search,
            data_to_search.nearest,
            data_to_search.max_point);



    tmpl->data = str;
    tmpl->len = len;
    tmpl->fmtlist = TMPL_add_fmt(0, ENTITY_ESCAPE, TMPL_encode_entity);
    tmpl->fmtlist = TMPL_add_fmt(tmpl->fmtlist, URL_ESCAPE, TMPL_encode_url);


}*/
