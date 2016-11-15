/*
 * show_error.c
 *
 *  Created on: 14 nov 2016
 *      Author: nop
 */
#include <show_error.h>


inline void show_error(MYSQL *mysql,attributes_set_t *ptr_attrl) {
#define __SIZE (2048)
 char *log_message=kore_malloc(__SIZE);

 snprintf(log_message, __SIZE,
                       "Error(%d) [%s] \"%s\"", mysql_errno(mysql),
                                                mysql_sqlstate(mysql),
                                                mysql_error(mysql));
 kore_log(LOG_ERR, "%s: [%s]\n", log_message, DB_SID);

 webui_add_error_message(ptr_attrl,log_message);

  mysql_close(mysql);
}
