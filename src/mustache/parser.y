%{
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <config.h>
#include <mustache.h>
#include <mustache-internal.h>
#include <parser.tab.h>	

#define YY_END_OF_BUFFER_CHAR 0
typedef struct yy_buffer_state *YY_BUFFER_STATE;
typedef size_t yy_size_t;
extern YY_BUFFER_STATE mustache_p__scan_buffer (char *base,yy_size_t size  );
extern int mustache_p_lex_destroy(void);
extern int mustache_p_get_lineno(void);

void yyerror (mustache_ctx *, const char *);

%}

%start  start

%define api.pure
%parse-param {mustache_ctx *ctx}

%union {
	char                  *text;
	mustache_token_t      *template;
}
%token TEXT MUSTAG_START MUSTAG_END
%type  <text>                  TEXT MUSTAG_START MUSTAG_END text
%type  <template>              tpl_tokens
%type  <template>              tpl_token

%%

start : tpl_tokens { ctx->template = $1; }

tpl_tokens :
	/* empty */ {
		$$ = NULL;
	}
	| tpl_token {
		$$ = $1;
	}
	| tpl_tokens tpl_token {
		mustache_token_t *p = $1;
		
		while(p->next != NULL)
			p = p->next;
		
		p->next = $2;
		$$ = $1;
	}
	;

tpl_token :
	text {                                   // simple text
		$$ = malloc(sizeof(mustache_token_t));
		$$->type                     = TOKEN_TEXT;
		$$->token_simple.text        = $1;
		$$->token_simple.text_length = strlen($1);
		$$->token_simple.userdata    = NULL;
		$$->next                     = NULL;
	}
	| MUSTAG_START text MUSTAG_END {         // mustache tag
		$$ = malloc(sizeof(mustache_token_t));
		$$->type                     = TOKEN_VARIABLE;
		$$->token_simple.text        = $2;
		$$->token_simple.text_length = strlen($2);
		$$->token_simple.userdata    = NULL;
		$$->next                     = NULL;
	}
	| MUSTAG_START '#' text MUSTAG_END tpl_tokens MUSTAG_START '/' text MUSTAG_END { // mustache section
		$$ = malloc(sizeof(mustache_token_t));
		$$->type                   = TOKEN_SECTION;
		$$->token_section.name     = $3;
		$$->token_section.section  = $5;
		$$->token_section.inverted = 0;
		$$->token_section.userdata = NULL;
		$$->next                   = NULL;
	}
	| MUSTAG_START '^' text MUSTAG_END tpl_tokens MUSTAG_START '/' text MUSTAG_END { // mustache inverted section 
		$$ = malloc(sizeof(mustache_token_t));
		$$->type                   = TOKEN_SECTION;
		$$->token_section.name     = $3;
		$$->token_section.section  = $5;
		$$->token_section.inverted = 1;
		$$->token_section.userdata = NULL;
		$$->next                   = NULL;
	}
	;

text :
	TEXT {
		$$ = $1;
	}
	| text TEXT {    // eat up text duplicates
		uintmax_t len1, len2;
		
		len1 = strlen($1);
		len2 = strlen($2);
		$1   = realloc($1, len1 + len2 + 1);
		memcpy($1 + len1, $2, len2 + 1);
		
		$$  = $1;
		free($2);
	}

%%

void yyerror(mustache_ctx *ctx, const char *msg){ // {{{
	ctx->api->error(ctx->api, ctx->userdata, mustache_p_get_lineno(), (char *)msg);
} // }}}

#ifdef HELPERS
uintmax_t             mustache_std_strread(mustache_api_t *api, void *userdata, char *buffer, uintmax_t buffer_size){ // {{{
	char                  *string;
	uintmax_t              string_len;
	mustache_str_ctx      *ctx               = (mustache_str_ctx *)userdata; 
	
	string     = ctx->string + ctx->offset;
	string_len = strlen(string);
	string_len = (string_len < buffer_size) ? string_len : buffer_size;
	
	memcpy(buffer, string, string_len);
	
	ctx->offset += string_len;
	return string_len;
} // }}}
uintmax_t             mustache_std_strwrite(mustache_api_t *api, void *userdata, char *buffer, uintmax_t buffer_size){ // {{{
	mustache_str_ctx      *ctx               = (mustache_str_ctx *)userdata; 
	
	ctx->string = realloc(ctx->string, ctx->offset + buffer_size + 1);
	
	memcpy(ctx->string + ctx->offset, buffer, buffer_size);
	ctx->string[ctx->offset + buffer_size] = '\0';
	
	ctx->offset += buffer_size;
	return buffer_size;
} // }}}
#endif

mustache_template_t * mustache_compile(mustache_api_t *api, void *userdata){ // {{{
	mustache_ctx           ctx               = { api, NULL, userdata };
	char                  *content           = NULL;
	uintmax_t              content_off       = 0;
	uintmax_t              ret;
	
	while(1){
		content       = realloc(content, content_off + 1024 + 2); // 2 for terminating EOF of yy
		if(!content)
			break;
		
		if( (ret = api->read(api, userdata, content + content_off, 1024)) == 0)
			break;
		
		content_off += ret;
	}
	
	if(content){
		content[content_off] = content[content_off+1] = YY_END_OF_BUFFER_CHAR;
		
		mustache_p__scan_buffer(content, content_off + 2);
		
		yyparse(&ctx);
		
		mustache_p_lex_destroy();
		free(content);
	}
	return ctx.template;
} // }}}
uintmax_t             mustache_prerender (mustache_api_t *api, void *userdata, mustache_template_t *template){ // {{{
	mustache_template_t            *p;
	
	for(p = template; p; p = p->next){
		switch(p->type){
			case TOKEN_TEXT:
				break;
			case TOKEN_VARIABLE:
				if(api->varget(api, userdata, &p->token_simple) == 0)
					return 0;
				break;
			case TOKEN_SECTION:
				if(api->sectget(api, userdata, &p->token_section) == 0)
					return 0;
				break;
		};
	}
	return 1;
} // }}}
uintmax_t             mustache_render (mustache_api_t *api, void *userdata, mustache_template_t *template){ // {{{
	mustache_template_t            *p;
	
	for(p = template; p; p = p->next){
		switch(p->type){
			case TOKEN_TEXT:
				if(api->write(api, userdata, p->token_simple.text, p->token_simple.text_length) == 0)
					return 0;
				break;
			case TOKEN_VARIABLE:
				if(api->varget(api, userdata, &p->token_simple) == 0)
					return 0;
				break;
			case TOKEN_SECTION:
				if(api->sectget(api, userdata, &p->token_section) == 0)
					return 0;
				break;
		};
	}
	return 1;
} // }}}
void                  mustache_free   (mustache_api_t *api, mustache_template_t *template){ // {{{
	mustache_template_t            *p, *n;
	
	for(p = template; p; p = n){
		switch(p->type){
			case TOKEN_TEXT:
			case TOKEN_VARIABLE:
				if(p->token_simple.userdata && api->freedata)
					api->freedata(api, p->token_simple.userdata);
				
				if(p->token_simple.text)
					free(p->token_simple.text);
				break;
			case TOKEN_SECTION:
				if(p->token_section.userdata && api->freedata)
					api->freedata(api, p->token_section.userdata);
				
				mustache_free(api, p->token_section.section);
				if(p->token_section.name)
					free(p->token_section.name);
				break;
		};
		n = p->next;
		free(p);
	}
} // }}}

#ifdef DEBUG
char * token_descr[] = {
	[TOKEN_TEXT]     = "text",
	[TOKEN_VARIABLE] = "variable",
	[TOKEN_SECTION]  = "section",
};

void mustache_dump(mustache_template_t *template){ // {{{
	mustache_template_t            *p;
	
	p = template;
	do{
		fprintf(stderr, "token: ->type '%s'; ->text: '%s'; ->next = %p\n",
			token_descr[p->type],
			p->token_simple.text,
			p->next
		);
	}while( (p = p->next) != NULL);
} // }}}
#endif
