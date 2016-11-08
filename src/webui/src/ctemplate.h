/*
 * C Template Library 1.0
 *
 * Copyright 2009 Stephen C. Losen.  Distributed under the terms
 * of the GNU General Public License (GPL)
 */

#ifndef _CTEMPLATE_H
#define _CTEMPLATE_H

typedef struct TMPL_varlist TMPL_varlist;
typedef struct TMPL_loop  TMPL_loop;
typedef struct TMPL_fmtlist TMPL_fmtlist;
typedef void (*TMPL_fmtfunc) (const char *, FILE *);

/*

TMPL_varlist *TMPL_add_var(TMPL_varlist *varlist,
    const char *varname1, const char *value1, ... , 0);
*/

TMPL_varlist *TMPL_add_var(TMPL_varlist *varlist, ...);

TMPL_varlist *TMPL_add_loop(TMPL_varlist *varlist,
    const char *name, TMPL_loop *loop);

TMPL_loop *TMPL_add_varlist(TMPL_loop *loop, TMPL_varlist *varlist);

void TMPL_free_varlist(TMPL_varlist *varlist);

TMPL_fmtlist *TMPL_add_fmt(TMPL_fmtlist *fmtlist,
    const char *name, TMPL_fmtfunc fmtfunc);

void TMPL_free_fmtlist(TMPL_fmtlist *fmtlist);

int TMPL_write(const char *filename, const char *tmplstr,
    const TMPL_fmtlist *fmtlist, const TMPL_varlist *varlist,
    FILE *out, FILE *errout);

void TMPL_encode_entity(const char *value, FILE *out);

void TMPL_encode_url   (const char *value, FILE *out);

#endif
