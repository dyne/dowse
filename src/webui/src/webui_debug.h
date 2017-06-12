#include <kore.h>
#include <http.h>
#include <time.h>
#include <attributes_set.h>


/***/

#ifndef _WEBUI_DEBUG_H
#define _WEBUI_DEBUG_H

#ifndef __where_i_am__

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define __where_i_am__ __FILE__ ":" TOSTRING(__LINE__)

#endif


#define __WEBUI_DEBUG__
#ifndef __WEBUI_DEBUG__
#define WEBUI_DEBUG {}
#else
#define WEBUI_DEBUG {func("WEBUI_DEBUG: %ld : %s:%d",time(NULL),__FILE__,__LINE__);}
#endif

/* Define mapping with bootstap alert level */
#define WEBUI_level_error (char*)"danger"
#define WEBUI_level_warning (char*)"warning"
#define WEBUI_level_info (char*)"info"
#define WEBUI_level_success (char*)"success"

/* Definition of function and prototype for add message to TMPL_LOOP */
void _webui_add_message_level(attributes_set_t *ptr_attrl,const char*level,const char *log_message);

#define WEBUI_DEF_ERROR_LEVEL_MESSAGE_PROTOTYPE(LEVEL) \
    void webui_add_ ## LEVEL ## _message  (attributes_set_t *ptr_attrl,const char *log_message) ;

WEBUI_DEF_ERROR_LEVEL_MESSAGE_PROTOTYPE(info) ;
WEBUI_DEF_ERROR_LEVEL_MESSAGE_PROTOTYPE(warning) ;
WEBUI_DEF_ERROR_LEVEL_MESSAGE_PROTOTYPE(success) ;
WEBUI_DEF_ERROR_LEVEL_MESSAGE_PROTOTYPE(error);


#define WEBUI_DEF_ERROR_LEVEL_MESSAGE(LEVEL) \
        void webui_add_ ## LEVEL ## _message (attributes_set_t *ptr_attrl,const char *log_message) \
            { _webui_add_message_level(ptr_attrl, WEBUI_level_ ##LEVEL ,log_message); }
/**/

#define TMPL_VAR_LEVEL_MESSAGE "level"
#define TMPL_VAR_TEXT_MESSAGE "text"
#define TMPL_VAR_TIME_MESSAGE "when"

#define TMPL_VAR_MESSAGE_LOOP "message_loop"

/* TODO aggiungere stacktrace debug */

#define   log_entering() func("Entering into %s \t\t\t(%s:%d) ",__FUNCTION__,__FILE__,__LINE__)


#define push_performance(p) _push_performance(__FILE__,__FUNCTION__,__LINE__)
#define pop_performance(p) _pop_performance(__FILE__,__FUNCTION__,__LINE__)

#define PUSH_PERF() {}
//push_performance();
#define POP_PERF() {}
//pop_performance();


#endif
