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

#define mb (1024*1024)
#define ml (1024*3)
#define RESET_ADMIN_FILE "./reset_admin_file"

#define CHAN "command-fifo-pipe"
#define ACK_CHAN "ack-command-fifo-pipe"

#define _IP_IS_ADMIN_ (1)
#define _ADMIN_NOT_CONFIGURED_ (0)
#define _IP_IS_NOT_ADMIN_ (-1)
#define _SQL_ERROR_ (-2)

#define _ENABLE_TO_BROWSE (2)
#define _DISABLE_TO_BROWSE (3)
#define _NOT_ENABLE_TO_BROWSE (4)

/* redis authorization state*/
#define __R_ADMIN_SHOULD_CHECK "admin_should_check"

#define __R_AUTH_ADMIN_AUTHORIZED "admin"
#define __R_AUTH_CLIENT_AUTHORIZED "authorized_to_browse"
#define __R_AUTH_DISABLED "disable_to_browse"

/* Level of browse authorization */
#define __IP_IS_ADMIN_AUTH_BROWSE_STR "ADMIN TO BROWSE"
#define __ENABLE_TO_BROWSE_STR "ENABLE_TO_BROWSE"
#define __DISABLE_TO_BROWSE_STR "DISABLE_TO_BROWSE"
#define __NOT_AUTHORIZED_BROWSE_STR "NOT ENABLED TO BROWSE"

#define __EVENT_NEW_MAC_ADDRESS "new_mac_address"



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
        err("Bad parsing at [%s]:[%d]",__FUNCTION__,__LINE__);\
        return show_generic_message_page(req, attr);\
    }

#include <webui_common_var.h>
#include <webui_prototype.h>

#endif
