

#ifndef TEMPLATE_H
#define TEMPLATE_H
#include "attributes_hm.h"
#include "assetmap.h"

typedef struct kore_buf kore_buf_t;

typedef struct template_t {
	u_int8_t *data;
	int len;
}template_t;

typedef struct mustache_webui_ctx {
	uint8_t *data;
	int count;
	int max;
	attributes_hm_t attr;
	struct kore_buf *kbuf;
} mustache_webui_ctx;

int template_load( u_int8_t *str, int len, template_t *tmpl);

void template_apply(template_t *t, attributes_hm_t al, kore_buf_t *out);

void template_free(template_t *t);

#endif
