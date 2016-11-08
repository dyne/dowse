#include <kore.h>
#include <assetmap.h>
#include "template.h"
#include "webui_debug.h"
#include "attributes_set.h"
#include "test_unit.h"

extern attributes_set_t attributes;
/********/
int template_load( u_int8_t *str, int len, template_t *tmpl){
	tmpl->data=str;
	tmpl->len=len;
	return 0;
}

void template_apply(template_t *tmpl, attributes_set_t al, struct kore_buf *out){
  char out_name[]="/tmp/out_stream_XXXXXX";
  char err_name[]="/tmp/err_stream_XXXXXX";
  FILE*out_stream,*err_stream;

  mkstemp(out_name);
  mkstemp(err_name);

  kore_log(LOG_DEBUG," out [%s] err[%s] ",out_name,err_name);

  out_stream=fopen(out_name,"w");
  err_stream=fopen(err_name,"w");
  TMPL_write(NULL,tmpl->data, 0, al, out_stream,err_stream);

  fclose(out_stream);
  fclose(err_stream);
  
}

void template_free(template_t *t){

}



WEBUI_TEST_UNIT(A001){
    attributes_set_t _attributes;
    char *titolo;
     char template[]="<TMPL_var name=\"title\"";
     template_t t;
     struct kore_buf *out;

     _attributes=attrinit();
    _attributes=attrcat(_attributes, "title", "Dowse information panel");

    template_load(template,strlen(template),&t);

    template_apply(&t,_attributes,out);

    RETURN_ASSERT(1);
      //    RETURN_ASSERT(strcmp(titolo,"Dowse information panel"),0);
}


/*
 WEBUI_TEST_UNIT(A002)
 {
    attributes_set_t studente,studente2,stud,_attributes;
	char *name;
	_attributes=attrinit();

    studente =attrinit();
    attrcat(studente,"nome","Antonio");
    attrcat(studente,"cognome","Rossi");
    attrcat(studente,"indirizzo","Casa Sua");
    attr_add(_attributes,"studente",studente);

    studente2 =attrinit();
    attrcat(studente2,"nome","Mario");
    attrcat(studente2,"cognome","Bianchi");
    attrcat(studente2,"indirizzo","Un altro posto");
    attr_add(_attributes,"studente",studente2);

    attrget(_attributes,"studente",1,(any_t*)&stud);
    attrget(stud,"nome",0,(any_t*)&name);
    return (strcmp(name,"Mario")==0);
}
*/
