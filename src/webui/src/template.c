
#include <assetmap.h>
#include "template.h"
#include <mustache.h>
#include "webui_debug.h"
#include "attributes_hm.h"

extern attributes_hm_t attributes;

static uintmax_t read_template_from_asset(mustache_api_t *api, void *data, char *buf, uintmax_t sz)
{

	 WEBUI_DEBUG
	 mustache_webui_ctx *ctx;

	 ctx=(mustache_webui_ctx *)data;

	 kore_log(LOG_DEBUG,"%s %d Letti %u di %u",__func__,__LINE__,ctx->count,ctx->max );

	 /* Do whatever needs to be done to get sz number of bytes into buf.
     */
	/* TODO change from copy to ... ? */

	int _to_read,_available;
	_available=(ctx->max - ctx->count);
	_to_read=(_available<sz ? _available : sz);
	 kore_log(LOG_DEBUG,"%s %d available %u to_read %u sz %20ju",__func__,__LINE__,_available,_to_read,sz);
	memcpy(buf,ctx->data,_to_read);
	ctx->count+=_to_read;

	return _to_read;
}

static uintmax_t
write_to_kore_buf(mustache_api_t *api, void *data,
                      char const *buf, uintmax_t sz)
{
	FILE *fp=fopen("output.log","a+");
	mustache_webui_ctx *ctx;

	ctx=(mustache_webui_ctx *)data;
	WEBUI_DEBUG
	 /* Do whatever it takes to write sz number of bytes from buf to somewhere. */
	kore_buf_append(ctx->kbuf,buf,sz);

#ifdef __WEBUI_DEBUG__
	char message[600*1024];
	char to_add[600*1024];
	snprintf(to_add,sz+1,"%s",buf);
	snprintf(message,1+ctx->kbuf->length,"%s",ctx->kbuf->data);
	kore_log(LOG_DEBUG,
	//fprintf(fp,
			"======================\n"
			"-------------------\n"
			"%s  offset=[%d] kbuf->length=[%u]\n"
//			"+++++++ Rendered +++++\n"
//			"%s\n"
			"+++++++ ADDED ++++++++\n"
			"%s\n"
			"sz=[%20ju]\n"
			"-------------------\n"
			"======================\n",
			__where_i_am__,ctx->kbuf->offset,ctx->kbuf->length
//			,			message
			,			to_add
			,			sz
			);
#endif
	fclose(fp);

	return sz;
}


static void
mustache_error(mustache_api_t *api, void *data,
               uintmax_t line, char const *error)
{
	 char message[1024];
	 snprintf(message,sizeof(message),"error in template: %20ju : %s\n", line, error);
	 kore_log(LOG_ERR," Error on apply template: %s",message);

}




static uintmax_t
mustache_varget(mustache_api_t *api, void *data, mustache_token_variable_t *t)
{
	mustache_webui_ctx *ctx;
	char *_value;
	ctx=(mustache_webui_ctx *)data;
	attrget(ctx->attr,t->text,0,(any_t*)&_value);

	if (_value!=NULL) {
	    api->write(api, data, (char*)_value, strlen((char*)_value));
	    return 1;
	} else {
		WEBUI_DEBUG
#define NOT_AVAILABLE "n/a"
	    api->write(api, data, (char*)NOT_AVAILABLE, strlen((char*)NOT_AVAILABLE));
	    kore_log(LOG_WARNING, "error: unknown variable: %s\n", t->text);
	    return 1;
	    /* If you return false, parsing will stop. To ignore unknown variables
	     * you should always return true.
	     */
	}

}


static uintmax_t
mustache_sectget(mustache_api_t *api, void *data, mustache_token_section_t *s)
{
	mustache_webui_ctx *ctx;
	attributes_hm_t *_value;
	ctx=(mustache_webui_ctx *)data;
	int index;
	WEBUI_DEBUG

	mustache_webui_ctx *local_section_attr;
	local_section_attr=(mustache_webui_ctx*)malloc(sizeof(mustache_webui_ctx));
	memcpy(local_section_attr,ctx,sizeof(mustache_webui_ctx));

	for (index=0;;index++) {
		WEBUI_DEBUG
		attrget(ctx->attr,s->name,index,(any_t*)&_value);

		if ((_value==NULL)) {
			if ((index==0)) {
				kore_log(LOG_ERR,"%s attributo [%s] non trovato al [%d] iterazione",
						__where_i_am__,s->name,index);
				break;
			} else {
				break;
			}
		} else {
			WEBUI_DEBUG
			local_section_attr->attr=(attributes_hm_t)_value;
			if (!mustache_render(api, local_section_attr, s->section)) {
					return 0;
			}
		}
	}
	free(local_section_attr);
	return 1;
}
/********/
int template_load( u_int8_t *str, int len, template_t *tmpl){

	tmpl->data=str;
	tmpl->len=len;
	return 0;
}

void template_apply(template_t *tmpl, attributes_hm_t al, kore_buf_t *out){
	mustache_api_t api;
    mustache_template_t *t = NULL;
    WEBUI_DEBUG


    /* */
   api.read = read_template_from_asset ;
   api.write = write_to_kore_buf;
   api.error = mustache_error;
   api.sectget = mustache_sectget;
   api.varget = mustache_varget;

	mustache_webui_ctx  *src_context =(mustache_webui_ctx  *)malloc(sizeof(mustache_webui_ctx  ));
	src_context->data = tmpl->data ;
	src_context->attr=NULL;
	src_context->count=0;
	src_context->kbuf=NULL;
	src_context->max=tmpl->len;

	mustache_webui_ctx  *dst_context =(mustache_webui_ctx  *)malloc(sizeof(mustache_webui_ctx  ));
	dst_context->attr=al;
	dst_context->count=0;
	dst_context->data=NULL;

	dst_context->kbuf=out;
	dst_context->max=0;

    /**/
    WEBUI_DEBUG
	t = mustache_compile(&api,src_context );

    WEBUI_DEBUG
	  debug_attributes(attributes);

    WEBUI_DEBUG
	if (t == NULL) {
    	kore_log(LOG_ERR,"ERROR in %s %d",__func__,__LINE__);
    	return;
    }
    WEBUI_DEBUG
    if (!mustache_render(&api, dst_context, t)) {
    	kore_log(LOG_ERR,"ERROR in %s %d",__func__,__LINE__);
    	return;
    }
    /*TODO Una gestione unificata della memoria free(dst_context);
    free(src_context);*/
}

void template_free(template_t *t){

}
