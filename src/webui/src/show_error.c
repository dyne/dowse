/*
 * show_error.c
 *
 *  Created on: 14 nov 2016
 *      Author: nop
 */
#include <show_error.h>
#include <webui.h>


inline void show_mysql_error(MYSQL *mysql,attributes_set_t *ptr_attrl) {
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

int show_generic_message_page(struct http_request *req, attributes_set_t error_messages_attribute_list){
    template_t tmpl;
    struct kore_buf*out;
    int len;
    char*data;
    out=kore_buf_alloc(0);

    template_load(asset_generic_message_list_html,asset_len_generic_message_list_html,&tmpl);
    template_apply(&tmpl,error_messages_attribute_list,out);

    data=kore_buf_release(out,&len);
    http_response(req,404,data,len);
    return KORE_RESULT_OK;
}
