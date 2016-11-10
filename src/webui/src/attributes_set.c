#include <kore.h>
#include <attributes_set.h>

#include "hashmap.h"
#include "webui_debug.h"


/**/
attributes_set_t attrinit(){  
  return (attributes_set_t) calloc(sizeof(pointed_attributes_set_t),1);
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
  TMPL_free_varlist(hm->varlist);
}




