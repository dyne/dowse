#ifndef ATTRIBUTES_HM
#define ATTRIBUTES_HM

#include <ctemplate.h>

typedef struct pointed_attributes_set_t {
    TMPL_varlist *varlist;
    TMPL_loop    *loop;
} pointed_attributes_set_t;

typedef pointed_attributes_set_t * attributes_set_t ;


/* void attrget(attributes_set_t hm, char *key, int index, any_t *value); */
attributes_set_t  attrinit();
attributes_set_t  attrcat(attributes_set_t hm, const char *key, const char *value);
attributes_set_t  attr_add(attributes_set_t hm, const char *key,attributes_set_t value);
void attrfree(attributes_set_t hm);


#endif
