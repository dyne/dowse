/*
 * show_error.c
 *
 *  Created on: 14 nov 2016
 *      Author: nop
 */
#include <show_error.h>
#include <webui.h>


void show_mysql_error(MYSQL *mysql,attributes_set_t *ptr_attrl) {
#define __SIZE (2048)
 char *log_message=kore_malloc(__SIZE);

 snprintf(log_message, __SIZE,
                       "Error(%d) [%s] \"%s\"", mysql_errno(mysql),
                                                mysql_sqlstate(mysql),
                                                mysql_error(mysql));
 err( "%s: [%s]\n", log_message, DB_SID);

 webui_add_error_message(ptr_attrl,log_message);

  mysql_close(mysql);
}

int show_generic_message_page(struct http_request *req, attributes_set_t error_messages_attribute_list){
    return apply_template_and_return(
            req,
            error_messages_attribute_list,
            "assets/generic_message_list.html",404);
}

int apply_template_and_return(struct http_request *req, attributes_set_t attribute_list,const char*path_asset_template_html,int ret_value){
    template_t tmpl;
    struct kore_buf*out;
    size_t len;
    u_int8_t *data;
    out=kore_buf_alloc(0);

    template_load(path_asset_template_html,&tmpl);
    template_apply(&tmpl,attribute_list,out);

    data=kore_buf_release(out,&len);
    http_response(req,ret_value,data,len);
    attrfree(attribute_list);
    if (data) {
        free(data);
    }
    return KORE_RESULT_OK;
}
