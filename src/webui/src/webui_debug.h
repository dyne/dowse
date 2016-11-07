#include <kore.h>

#ifndef __WHERE_I_AM_
#define __WHERE_I_AM_
static char __buf_where_i_am__[256];

#define __where_i_am__\
	(\
		snprintf(__buf_where_i_am__,sizeof(__buf_where_i_am__),"%s>%s:%d",__FILE__,__func__,__LINE__)?\
				__buf_where_i_am__:__buf_where_i_am__)
	/* TODO Stack trace format in tmp directory ? */

#endif


#define __WEBUI_DEBUG__
#ifndef __WEBUI_DEBUG__
#define WEBUI_DEBUG {}
#else
#define WEBUI_DEBUG {fprintf(stderr,"WEBUI_DEBUG: %s\n",__where_i_am__);}
#endif

/* TODO aggiungere stacktrace debug */
