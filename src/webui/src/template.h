

#ifndef TEMPLATE_H
#define TEMPLATE_H
#include <kore.h>
#include "attributes_set.h"
#include "assetmap.h"
#include "ctemplate.h"


typedef struct template_t {
	u_int8_t *data;
	int len;
}template_t;

int template_load( u_int8_t *str, int len, template_t *tmpl);

void template_apply(template_t *t, attributes_set_t al, struct kore_buf *out);

void template_free(template_t *t);

#endif

