#ifndef ATTRIBUTES_HM
#define ATTRIBUTES_HM

#include <ctemplate.h>

typedef TMPL_varlist *attributes_set_t ;


/* void attrget(attributes_set_t hm, char *key, int index, any_t *value); */
void*  attrinit();
attributes_set_t  attrcat(attributes_set_t hm, char *key, char *value);
attributes_set_t  attr_add(attributes_set_t hm, char *key,attributes_set_t value);
void attrfree(attributes_set_t hm);


#endif
