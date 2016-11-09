#include <kore.h>
#include <assetmap.h>
#include "template.h"
#include "webui_debug.h"
#include "attributes_set.h"
#include "test_unit.h"

/********/
int template_load( u_int8_t *str, int len, template_t *tmpl){
	tmpl->data=str;
	tmpl->len=len;
	tmpl->fmtlist = TMPL_add_fmt(0, ENTITY, TMPL_encode_entity);

	return 0;
}

void template_apply(template_t *tmpl, attributes_set_t al, struct kore_buf *out){
  char out_name[]="/tmp/out_stream_XXXXXX";
  char err_name[]="/tmp/err_stream_XXXXXX";
  FILE*out_stream,*err_stream;
  char buf_str[1024],end;
  unsigned long int size;
  int some_error,rv;

  mkstemp(out_name);
  mkstemp(err_name);

  kore_log(LOG_DEBUG," out [%s] err[%s] ",out_name,err_name);

  out_stream=fopen(out_name,"rw+");
  err_stream=fopen(err_name,"rw+");

  WEBUI_DEBUG;
  TMPL_write(NULL,tmpl->data, tmpl->fmtlist, al->varlist, out_stream,err_stream);

  size=ftell(out_stream);
  kore_log(LOG_DEBUG," out [%s] [%d] ",__where_i_am__,ftell(out_stream));

  some_error=0;
  if (size>0) {
    rewind(out_stream);
    rewind(err_stream);

    
    while ((rv=fread(buf_str,1,sizeof(buf_str),err_stream))>0) {
      buf_str[rv]=0;
      kore_log(LOG_ERR,"%s %s",__where_i_am__,buf_str);
      some_error=1;
    }
    if (!some_error) {
	while ((rv=fread(buf_str,1,sizeof(buf_str),out_stream))>0) {
	  buf_str[rv]=0;
	  kore_log(LOG_DEBUG,"%s %s",__where_i_am__,buf_str);
	  kore_buf_append(out,buf_str,rv);
	}
    }
  }
  WEBUI_DEBUG;
  end=0;

  kore_buf_append(out,&end,1);
 
  fclose(out_stream);
  fclose(err_stream);
  
  unlink(out_name);
  unlink(err_name);
}

void template_free(template_t *t){

}



WEBUI_TEST_UNIT(A001){
    attributes_set_t _attributes;
    char *rendered;
    char template[]="<TMPL_var name=\"title\">";
    template_t t;
    int size;
    struct kore_buf *out;
    
    out=kore_buf_alloc(0);
    _attributes=attrinit();
    _attributes=attrcat(_attributes, "title", "Dowse information panel");
    
    template_load(template,strlen(template),&t);

    template_apply(&t,_attributes,out);

    rendered=(char*)kore_buf_release(out,&size);
    
    RETURN_ASSERT(strcmp(rendered,"Dowse information panel")==0);
}

 
 WEBUI_TEST_UNIT(A002)
 {
    char template[]="<html>"
"<table>"
"<TMPL_LOOP name=\"studente\">"
"<tr>"
"<td><TMPL_VAR name=\"nome\"></td>"
"<td><TMPL_VAR name=\"cognome\"></td>"
"<td><TMPL_VAR name=\"indirizzo\"></td>"
"</tr>"
"</TMPL_LOOP>"
"</table>"
"</html>"
;    
    attributes_set_t studente,studente2,_attributes;   
    template_t t;
    int size;
    struct kore_buf *out;
    char *rendered;
    
    out=kore_buf_alloc(0);
    _attributes=attrinit();

    studente =attrinit();
    studente=attrcat(studente,"nome","Antonio");
    studente=attrcat(studente,"cognome","Rossi");
    studente=attrcat(studente,"indirizzo","Casa Sua");
    _attributes=attr_add(_attributes,"studente",studente);

    studente2 =attrinit();
    studente2=attrcat(studente2,"nome","Mario");
    studente2=attrcat(studente2,"cognome","Bianchi");
    studente2=attrcat(studente2,"indirizzo","Un altro posto");
    _attributes=attr_add(_attributes,"studente",studente2);

    template_load(template,strlen(template),&t);

    template_apply(&t,_attributes,out);

    rendered=(char*)kore_buf_release(out,&size);

    RETURN_ASSERT(strcmp(rendered,"<html><table><tr><td>Antonio</td><td>Rossi</td><td>Casa Sua</td></tr><tr><td>Mario</td><td>Bianchi</td><td>Un altro posto</td></tr></table></html>")==0);
   
}



 WEBUI_TEST_UNIT(A003)
 {
    char template[]="<html>"
"<table>"
"<TMPL_LOOP name=\"studente\">"
"<tr>"
"<td><TMPL_VAR name=\"nome\" fmt=\"" ENTITY "\"></td>"
"<td><TMPL_VAR name=\"cognome\" fmt=\"" ENTITY "\"></td>"
"<td><TMPL_VAR name=\"indirizzo\" fmt=\"" ENTITY "\"></td>"
"</tr>"
"</TMPL_LOOP>"
"</table>"
"</html>"
;
    attributes_set_t studente,studente2,_attributes;
    template_t t;
    int size;
    struct kore_buf *out;
    char *rendered;

    out=kore_buf_alloc(0);
    _attributes=attrinit();

    studente =attrinit();
    studente=attrcat(studente,"nome","Antonio");
    studente=attrcat(studente,"cognome","Rossi");
    studente=attrcat(studente,"indirizzo","Casa Sua");
    _attributes=attr_add(_attributes,"studente",studente);

    studente2 =attrinit();
    studente2=attrcat(studente2,"nome","<<Mario>>");
    studente2=attrcat(studente2,"cognome","Bianchi>>");
    studente2=attrcat(studente2,"indirizzo","Un altro posto");
    _attributes=attr_add(_attributes,"studente",studente2);

    template_load(template,strlen(template),&t);

    template_apply(&t,_attributes,out);

    rendered=(char*)kore_buf_release(out,&size);

    http_response(__webui_req,200,rendered,size);
    return 1;
    RETURN_ASSERT(1);

}
