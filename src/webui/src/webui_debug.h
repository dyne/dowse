#include <kore.h>

//#define __CICCIO__
#ifndef __CICCIO__
#define WEBUI_DEBUG {}
#else
#define WEBUI_DEBUG {fprintf(stderr,"WEBUI_DEBUG: %s>%s:%d\n",__FILE__,__func__,__LINE__);}
#endif

/* TODO aggiungere stacktrace debug */
