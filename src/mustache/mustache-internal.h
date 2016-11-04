#ifndef MUSTACHEC_INTERNAL_H
#define MUSTACHEC_INTERNAL_H

typedef struct mustache_ctx {
	mustache_api_t        *api;
	mustache_template_t   *template;
	void                  *userdata;
} mustache_ctx;

#endif
