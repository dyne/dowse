#include <kore.h>
#include <hashmap.h>
#include <attributes_set.h>
#include "webui_debug.h"


/**/
void* attrinit(){
  return NULL;
}


/* it store a simple string */
attributes_set_t attrcat(attributes_set_t hm, char*key,char* value) {
  return TMPL_add_var(hm,key,value,0);
}

/* it stores a structured attributes eg: {name , surname, age } */
attributes_set_t attr_add(attributes_set_t hm, char*key,attributes_set_t value) {
  TMPL_loop *loop;
  loop= TMPL_add_varlist(0, value);  
  return TMPL_add_loop(hm,key,loop);
}
 
void attrfree(attributes_set_t hm) {
  TMPL_free_varlist(hm);
}




