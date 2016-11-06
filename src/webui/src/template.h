

#ifndef TEMPLATE_H
#define TEMPLATE_H
#include "attributes_hm.h"


typedef struct kore_buf kore_buf_t;

typedef void * template_t ;

int template_load(const char *str, int len, template_t *tmpl);

void template_apply(template_t *t, attributes_hm_t al, kore_buf_t *out);

void template_free(template_t *t);

#endif
