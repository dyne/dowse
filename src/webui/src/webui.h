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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <libdowse/dowse.h>
#include <../database.h>
#include <show_error.h>

#define mb (1024*500)
#define ml (1024*3)
#define RESET_ADMIN_FILE "./reset_admin_file"


#define _IP_IS_ADMIN_ (1)
#define _ADMIN_NOT_CONFIGURED_ (0)
#define _IP_IS_NOT_ADMIN_ (-1)
#define _SQL_ERROR_ (-2)

#define _ENABLE_TO_BROWSE (2)
#define _DISABLE_TO_BROWSE (3)
#define _NOT_ENABLE_TO_BROWSE (4)


/* Level of browse authorization */
#define __IP_IS_ADMIN_AUTH_BROWSE_STR "ADMIN TO BROWSE"
#define __ENABLE_TO_BROWSE_STR "ENABLE TO BROWSE"
#define __DISABLE_TO_BROWSE_STR "DISABLE TO BROWSE"
#define __NOT_AUTHORIZED_BROWSE_STR "NOT ENABLED TO BROWSE"

#define __EVENT_NEW_MAC_ADDRESS "new_mac_address"

#ifndef DB_HOST /* if it's not redefined at runtime */
#define DB_HOST "127.0.0.1"
#define DB_USER "root"
#define DB_PASSWORD "p4ssw0rd"
#define DB_SID "things"
#define DB_SOCK_DIRECTORY "$R/mysqld/mysqld.sock"
#endif


#define PARSE_PARAMETER(PAR)\
    char *PAR=""; \
    http_argument_get_string(req, #PAR ,&PAR);\
    func( "%s Parameter %s:%d [%s]",#PAR,__FILE__,__LINE__,PAR);\
    if (strcmp(PAR,"")==0 ){\
      char m[1024];\
      snprintf(m, sizeof(m), "%s is not validated ",#PAR);\
      webui_add_error_message(&attr, m );\
      err(m);\
      bad_parsing=1;\
    }

#define CHECK_PARAMETER()\
    if (bad_parsing) {\
        return show_generic_message_page(req, attr);\
    }

#include <webui_common_var.h>
#include <webui_prototype.h>

#endif
