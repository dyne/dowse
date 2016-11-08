#include <kore.h>
#include <hashmap.h>
#include <attributes_set.h>
#include "webui_debug.h"


/**/
void* attrinit(){
  return NULL;
}


//-- It search the array containing all the element associated with the "key" and return the "index"-th element.
/* void attrget(attributes_set_t hm,char*key,int index,any_t *value) { */
		
/* 	WEBUI_DEBUG */
/* 	kore_log(LOG_DEBUG,"%s Looking for %s[%d]",__where_i_am__,key,index); */
/* 	if (index<0) { */
/* 		*value=NULL; */
/* 		return; */
/* 	} */
/*     value=(any_t)valueof(key,hm); */
/* } */


/* it store a simple string */
attributes_set_t attrcat(attributes_set_t hm, char*key,char* value) {
  return TMPL_add_var(hm,key,value,0);
}

/* it stores a structured attributes eg: {name , surname, age } */
attributes_set_t attr_add(attributes_set_t hm, char*key,attributes_set_t value) {
  TMPL_loop *loop;
  loop=0;
  loop= TMPL_add_varlist(loop, value);  
  return TMPL_add_loop(hm,key,loop);
}
 
void attrfree(attributes_set_t hm) {
  TMPL_free_varlist(hm);
}




