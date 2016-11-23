/*
 * sqlexecute.c
 *
 *  Created on: 14 nov 2016
 *      Author: nop
 */

#include <webui_debug.h>
#include <mysql.h>
#include <webui.h>


int sqlexecute(char*command,attributes_set_t *ptr_attrl){
  MYSQL *db ;

  WEBUI_DEBUG;
  // open db connection
   db=mysql_init(NULL);
//     Constant parameted created at compile time
      if (!mysql_real_connect(db, DB_HOST, DB_USER,DB_PASSWORD,
                  DB_SID, 0, DB_SOCK_DIRECTORY , 0))  {
          show_mysql_error(db,ptr_attrl);
          db=NULL;
          return(KORE_RESULT_ERROR);
  }

  WEBUI_DEBUG;
  // Execute the statement
  if (mysql_query(db, command)) {
    show_mysql_error(db,ptr_attrl);
    return KORE_RESULT_ERROR;
  }

  WEBUI_DEBUG;
  mysql_close(db);

  return 0;
}
