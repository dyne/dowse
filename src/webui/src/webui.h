#ifndef __WEBUI_H_
#define __WEBUI_H_
#include <webui_debug.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <math.h>
#include <mysql.h>
#include <time.h>

#include <kore.h>
#include <http.h>
#include <stdio.h>
#include <attributes_set.h>
#include <template.h>
#include <redis.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libdowse/dowse.h>

#define mb (1024*500)
#define ml (1024*3)
#define RESET_ADMIN_FILE "./reset_admin_file"


#define _IP_IS_ADMIN_ (1)
#define _ADMIN_NOT_CONFIGURED_ (0)
#define _IP_IS_NOT_ADMIN_ (-1)
#define _SQL_ERROR_ (-2)

#ifndef DB_HOST /* if it's not redefined at runtime */
#define DB_HOST "127.0.0.1"
#define DB_USER "root"
#define DB_PASSWORD "p4ssw0rd"
#define DB_SID "things"
#define DB_SOCK_DIRECTORY "$R/mysqld/mysqld.sock"
#endif


#include <webui_common_var.h>
#include <webui_prototype.h>

#endif
