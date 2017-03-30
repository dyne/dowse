#include <kore.h>

#include <attributes_set.h>
#include "webui_debug.h"

#include <libdowse/dowse.h>

/**/
attributes_set_t attrinit(){  
  attributes_set_t rv=(attributes_set_t) malloc(sizeof(pointed_attributes_set_t));
  rv->loop=NULL;
  rv->varlist=NULL;
  return rv;
}


/* it store a simple string */
attributes_set_t attrcat(attributes_set_t hm, const char*key,const char* value) {
  hm->varlist=TMPL_add_var(hm->varlist,key,value,0);
  return hm;
}

/* it stores a structured attributes eg: {name , surname, age } */
attributes_set_t attr_add(attributes_set_t hm, const char*key,attributes_set_t value) {
  TMPL_loop *loop;
  loop= TMPL_add_varlist(hm->loop, value->varlist);
  hm->loop=loop;
  hm->varlist=TMPL_add_loop(hm->varlist,key,loop);
  return hm;
}
 
void attrfree(attributes_set_t hm) {
    if (!hm) return;
    if (hm->varlist) TMPL_free_varlist(hm->varlist);
//    if (hm->loop) TMPL_free_looplist(hm->loop);
    free(hm);
}




