/*  Dowse - IP neigh parser
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

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

char line[1024];
char*field;
char *fields[128];
char ip4[128],ip6[256];
char device[128];
char macaddr[128];
char ref_value[128];
char used_str[128];
char probes_str[128];

#define DELIM " \n"
int n_fields,i;

int is_deleted,is_missing,is_router,is_proxy;

MYSQL *db ;



void parse_constant(char**fields,char*value,int*ptr_idx,int*out_value){
  if (strcmp(fields[*ptr_idx],value)==0) {
    *out_value=1;
    (*ptr_idx)++;
  } else {
    out_value=0;
  }
}

void parse_ip4(char**fields,int*ptr_idx,char*value){
  struct in_addr result;
  if (inet_pton(AF_INET,fields[*ptr_idx],&result)==1) {
    sprintf(value,"%s",fields[*ptr_idx]);
    (*ptr_idx)++;
  } else {
    value[0]=0;
  }
}


void parse_ip6(char**fields,int*ptr_idx,char*value){
  struct in6_addr result;

  if (inet_pton(AF_INET6,fields[*ptr_idx],&result)==1) {
    sprintf(value,"%s",fields[*ptr_idx]);
    (*ptr_idx)++;
  } else {
    value[0]=0;
  }
}

void parse_parameter(char**fields,int*ptr_idx,char*parameter,char*value){
  if (strcmp(fields[*ptr_idx],parameter)==0) {
    sprintf(value,"%s",fields[1+*ptr_idx]);
    (*ptr_idx)+=2;
  } else {
     value[0]=0;
  }
}

#define parse_state(S) \
int IS_##S ;\
IS_##S = (strstr(fields[n_fields-1], #S )!=0);\


void show_mysql_error(MYSQL *mysql){
  char log_message[256];
  snprintf(log_message, sizeof(log_message),
	   "Error(%d) [%s] \"%s\"",
	   mysql_errno(mysql),
	   mysql_sqlstate(mysql),
	   mysql_error(mysql));
  fprintf(stderr,"Error \n%s\n",log_message);
  exit(-1);
}

int main() {
 /* */
    db=mysql_init(NULL);
    //     Constant parameted created at compile time
    if (!mysql_real_connect(db, DB_HOST, DB_USER,DB_PASSWORD,
			    DB_SID, 0, DB_SOCK_DIRECTORY , 0))  {
      show_mysql_error(db);
    }

  for (;!feof(stdin);fgets(line,sizeof(line),stdin)) {
    #ifdef _DEBUG
    fprintf(stderr,"\n--------\nNew line\n");
    #endif
    field=strtok(line,DELIM);
    for (n_fields=0;field;n_fields++) {
      fields[n_fields]=(char*)malloc(strlen(field)+1);
      sprintf(fields[n_fields],"%s",field);
      field=strtok(NULL,DELIM);
    }
    fields[n_fields]=field;

    #ifdef _DEBUG
    for (i=0;i<n_fields;i++) {
      fprintf(stderr,"%d\t[%s]\n",i,fields[i]);
    }
    #endif
    if (n_fields<=0) continue;
    i=0;
    parse_constant(fields,"Deleted",&i,&is_deleted);
    parse_constant(fields,"miss",&i,&is_missing);

    parse_ip4(fields,&i,ip4);
    parse_ip6(fields,&i,ip6);
    

    parse_parameter(fields,&i,"dev",device);
    parse_parameter(fields,&i,"lladdr",macaddr);

    parse_constant(fields,"router",&i,&is_router);
    parse_constant(fields,"proxy",&i,&is_proxy);
    
    parse_parameter(fields,&i,"ref",ref_value);
    parse_parameter(fields,&i,"used",used_str);
    parse_parameter(fields,&i,"probes",probes_str);

    /* */
    parse_state(INCOMPLETE);
    parse_state(REACHABLE);
    parse_state(STALE);
    parse_state(DELAY);
    parse_state(PROBE);
    parse_state(FAILED);
    parse_state(NOARP);
    parse_state(PERMANENT);

    /*  Generate the SQL code */
    if (!IS_REACHABLE){
      // free all allocated memory
      for(i=0;i<n_fields;i++) free(fields[i]);
      continue;
    }
    char command[256];
    snprintf(command,sizeof(command),
	    "INSERT INTO found(macaddr,ip4,ip6) "
	    " VALUES ('%s','%s','%s') ON DUPLICATE KEY UPDATE ip4='%s',ip6='%s'",
            macaddr,ip4,ip6,ip4,ip6);
   
    // Execute the statement
    if (mysql_query(db, command)) {
      show_mysql_error(db);
    }
    
    // free all allocated memory
    for(i=0;i<n_fields;i++) free( fields[i] );
  }

  mysql_close(db);
  return 0;
}
		    
