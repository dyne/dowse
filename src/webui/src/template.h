

#ifndef TEMPLATE_H
#define TEMPLATE_H
#include <kore.h>
#include "attributes_set.h"
#include "assetmap.h"
#include "ctemplate.h"

#define ENTITY "entity"

typedef struct template_t {
	u_int8_t *data;
	int len;
	TMPL_fmtlist *fmtlist; /* It's present here, the format function pointer list, because the
	HTML template might be reused with different value of "attributes" */
} template_t;

int template_load( u_int8_t *str, int len, template_t *tmpl);

void template_apply(template_t *t, attributes_set_t al, struct kore_buf *out);

void template_free(template_t *t);

#endif

