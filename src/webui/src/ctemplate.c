/*
 * C Template: template expander library based on the perl
 * HTML::Template package.
 *
 * Version: 1.0
 *
 * Author: Stephen C. Losen, University of Virginia
 *
 * Copyright 2009 Stephen C. Losen.  Distributed under the terms
 * of the GNU General Public License (GPL)
 *
 * A template consists of text sequences, comments and template tags.
 * Anything that is not a tag and not a comment is considered text.
 * The tags include:
 *
 * <TMPL_VAR name = "varname" default = "value" fmt = "fmtname">
 * <TMPL_INCLUDE name = "filename">
 * <TMPL_LOOP name = "loopname">
 * <TMPL_BREAK level = N>
 * <TMPL_CONTINUE level = N>
 * </TMPL_LOOP>
 * <TMPL_IF name = "varname" value = "testvalue">
 * <TMPL_ELSIF name = "varname" value = "testvalue">
 * <TMPL_ELSE>
 * </TMPL_IF>
 *
 * The "name =" attribute is required, and the "value =", "fmt =",
 * "default =", and "level ="  attributes are optional.
 *
 * A comment is any text enclosed by <* and *>
 *
 * We read the entire template into memory, scan it, parse it, and
 * build a parse tree.  To generate the output, we walk the parse
 * tree and output the tree nodes.  A list of variables and values
 * determines what tree nodes we visit and what we output.
 *
 * The scanner splits the template into text sequences and tags.
 * Each call of scan() returns a tagnode struct representing the
 * next text sequence or tag.  The parser uses recursive descent
 * to link the tagnodes into a parse tree.
 */

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <ctemplate.h>

/* To prevent infinite TMPL_INCLUDE cycles, we limit the depth */

#define MAX_INCLUDE_DEPTH 30

/* template tag kinds (used in bitmaps) */

typedef enum {
    tag_text    = 0x001,   /* text sequence */
    tag_var     = 0x002,   /* TMPL_VAR      */
    tag_if      = 0x004,   /* TMPL_IF       */
    tag_elsif   = 0x008,   /* TMPL_ELSIF    */
    tag_else    = 0x010,   /* <TMPL_ELSE>   */
    tag_endif   = 0x020,   /* </TMPL_IF>    */
    tag_include = 0x040,   /* TMPL_INCLUDE  */
    tag_loop    = 0x080,   /* TMPL_LOOP     */
    tag_break   = 0x100,   /* TMPL_BREAK    */
    tag_cont    = 0x200,   /* TMPL_CONTINUE */
    tag_endloop = 0x400    /* </TMPL_LOOP>  */
} tag_kind;

typedef struct tagnode tagnode;
typedef struct template template;

/* The parse tree consists of tagnodes */

struct tagnode {
    tag_kind kind;
    tagnode *next;
    union {

        /* text sequence */

        struct {
            const char *start;
            int len;
        }
        text;

        /* TMPL_VAR tag */

        struct {
            const char *varname, *dfltval;
            TMPL_fmtfunc fmtfunc;
        }
        var;

        /* TMPL_IF tag or TMPL_ELSIF tag */

        struct {
            const char *varname, *testval;
            tagnode *tbranch, *fbranch;
        }
        ifelse;

        /* TMPL_LOOP tag */

        struct {
            const char *loopname;
            tagnode *body;
        }
        loop;

        /* TMPL_BREAK tag or TMPL_CONTINUE tag */

        struct {
            int level;
        }
        breakcont;

        /* TMPL_INCLUDE tag */

        struct {
            const char *filename;
            template *tmpl;
        }
        include;
    }
    tag;
};

/* template information */

struct template {
    const char *filename;  /* name of template file */
    const char *tmplstr;   /* contents of template file */
    FILE *out;             /* template output file pointer */
    FILE *errout;          /* error output file pointer */
    tagnode *roottag;      /* root of parse tree */
    const TMPL_fmtlist
        *fmtlist;          /* list of format functions */

    /* scanner and parser state variables */

    const char *scanptr;  /* next character to be scanned */
    tagnode *nexttag;     /* next tag to be returned by scanner */
    tagnode *curtag;      /* current tagnode being parsed */
    int linenum;          /* current template line number */
    int tagline;          /* line number of current tag's name */
    int error;            /* error indicator */
    int include_depth;    /* avoids TMPL_INCLUDE cycles */
    int loop_depth;       /* current loop nesting depth */
    int break_level;      /* for processing a TMPL_BREAK tag */
    int cont_level;       /* for processing a TMPL_CONTINUE tag */
    tagnode reusable;     /* reusable storage for simple tags */
};

/*
 * TMPL_fmtlist is a list of format functions, which are passed to
 * a template.  A TMPL_VAR tag can specify a format function for
 * outputting the variable with the fmt="fmtname" attribute.
 */

struct TMPL_fmtlist {
    TMPL_fmtlist *next;   /* next list member */
    TMPL_fmtfunc fmtfunc; /* pointer to format function */
    char name[1];         /* name of format function */
};

/*
 * variables are passed to a template in a tree consisting of
 * TMPL_var, TMPL_varlist and TMPL_loop nodes.
 *
 * TMPL_var is a simple variable (name and value)
 */

typedef struct TMPL_var TMPL_var;

struct TMPL_var {
    TMPL_var *next;     /* next simple variable on list */
    const char *name;
    char value[1];      /* value and name stored here */
};

/*
 * TMPL_varlist is a variable list of simple variables and/or
 * loop variables
 */

struct TMPL_varlist {
    TMPL_varlist *next;  /* next variable list on a list */
    TMPL_var   *var;     /* list of my simple variables */
    TMPL_loop  *loop;    /* list of my loop variables */
    TMPL_loop  *parent;  /* my parent loop variable (if any) */
};

/* TMPL_loop is a loop variable, which is a list of variable lists */

struct TMPL_loop {
    TMPL_loop *next;       /* next loop variable on a list */
    const char *name;      /* my name */
    TMPL_varlist *varlist; /* list of my variable lists */
    TMPL_varlist *tail;    /* tail of "varlist" */
    TMPL_varlist *parent;  /* my parent variable list */
};

/* mymalloc() is a malloc wrapper that exits on failure */

static void *
mymalloc(size_t size) {
    void *ret = malloc(size);
    if (ret == 0) {
        fputs("C Template library: out of memory\n", stderr);
        exit(1);
    }
    return ret;
}

/*
 * newtemplate() creates a new template struct and reads the template
 * file "filename" into memory.  If "tmplstr" is non-null then it is
 * the template, so we do not read "filename".
 */

static template *
newtemplate(const char *filename, const char *tmplstr,
    const TMPL_fmtlist *fmtlist, FILE *out, FILE *errout)
{
    template *t;
    FILE *fp;
    char *buf = 0;
    struct stat stb;

    if (tmplstr == 0 && filename == 0) {
        if (errout != 0) {
            fputs("C Template library: no template specified\n", errout);
        }
        return 0;
    }
    if (tmplstr == 0) {
        if ((fp = fopen(filename, "r")) != 0 &&
            fstat(fileno(fp), &stb) == 0 &&
            S_ISREG(stb.st_mode) != 0 &&
            (buf = (char *) mymalloc(stb.st_size + 1)) != 0 &&
            (stb.st_size == 0 ||
            fread(buf, 1, stb.st_size, fp) == stb.st_size))
        {
            fclose(fp);
            buf[stb.st_size] = 0;
        }
        else {
            if (errout != 0) {
                fprintf(errout, "C Template library: failed to read "
                    "template from file \"%s\"\n", filename);
            }
            if (buf != 0) {
                free(buf);
            }
            if (fp != 0) {
                fclose(fp);
            }
            return 0;
        }
    }
    t = (template *) mymalloc(sizeof(*t));
    t->filename = filename != 0 ? filename : "(none)";
    t->tmplstr = tmplstr != 0 ? tmplstr : buf;
    t->fmtlist = fmtlist;
    t->scanptr = t->tmplstr;
    t->roottag = t->curtag = t->nexttag = 0;
    t->out = out;
    t->errout = errout;
    t->linenum = 1;
    t->error = 0;
    t->include_depth = 0;
    t->loop_depth = 0;
    t->break_level = t->cont_level = 0;
    return t;
}

/* newtag() allocates a new tagnode */

static tagnode *
newtag(template *t, tag_kind kind) {
    tagnode *ret;

    switch(kind) {

    /*
     * The following tags are simple parse tokens that are
     * never linked into the parse tree so they share storage.
     */

    case tag_else:
    case tag_endif:
    case tag_endloop:
        ret = &t->reusable;
        break;

    default:
        ret = (tagnode *) mymalloc(sizeof(*ret));
        break;
    }
    ret->kind = kind;
    ret->next = 0;
    return ret;
}

/*
 * freetag() recursively frees parse tree tagnodes.  We do not free
 * the text in a tag_text tagnode because it points to memory where
 * the input template is stored, which we free elsewhere.
 */

static void
freetag(tagnode *tag) {
    template *t;

    if (tag == 0) {
        return;
    }
    switch(tag->kind) {

    case tag_var:
        free((void *) tag->tag.var.varname);
        if (tag->tag.var.dfltval != 0) {
            free((void *) tag->tag.var.dfltval);
        }
        break;

    case tag_if:
    case tag_elsif:
        free((void *) tag->tag.ifelse.varname);
        if (tag->tag.ifelse.testval != 0) {
            free((void *) tag->tag.ifelse.testval);
        }
        freetag(tag->tag.ifelse.tbranch);
        freetag(tag->tag.ifelse.fbranch);
        break;

    case tag_loop:
        free((void *) tag->tag.loop.loopname);
        freetag(tag->tag.loop.body);
        break;

    case tag_include:
        free((void *) tag->tag.include.filename);
        if ((t = tag->tag.include.tmpl) != 0) {
            free((void *) t->filename);
            free((void *) t->tmplstr);
            freetag(t->roottag);
            free(t);
        }
        break;
    }
    freetag(tag->next);
    free(tag);
}

/* map tag_kind to a human readable string */

static const char *
tagname(tag_kind kind) {
    switch(kind) {
    case tag_var:
        return "TMPL_VAR";
    case tag_if:
        return "TMPL_IF";
    case tag_elsif:
        return "TMPL_ELSIF";
    case tag_else:
        return "TMPL_ELSE";
    case tag_endif:
        return "/TMPL_IF";
    case tag_include:
        return "TMPL_INCLUDE";
    case tag_loop:
        return "TMPL_LOOP";
    case tag_break:
        return "TMPL_BREAK";
    case tag_cont:
        return "TMPL_CONTINUE";
    case tag_endloop:
        return "/TMPL_LOOP";
    }
    return "unknown";
}

/*
 * SCANNER FUNCTIONS
 *
 * scanspaces() scans white space
 */

static const char *
scanspaces(template *t, const char *p) {
    while (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t') {
        if (*p++ == '\n') {
            t->linenum++;
        }
    }
    return p;
}

/*
 * scancomment() scans a comment delimited by <* and *>. If we find a
 * comment then we advance t->scanptr to the first character after the
 * comment and return 1.  Otherwise we return 0.
 */

static int
scancomment(template *t, const char *p) {
    int linenum = t->linenum;

    if (p[0] != '<' || p[1] != '*') {
        return 0;
    }
    for (p += 2; *p != 0; p++) {
        if (*p == '\n') {
            t->linenum++;
        }
        if (p[0] == '*' && p[1] == '>') {
            t->scanptr = p + 2;
            return 1;
        }
    }

    /* end of template, comment not terminated */

    if (t->errout != 0) {
        fprintf(t->errout, "\"<*\" in file \"%s\" line %d "
            "has no \"*>\"\n", t->filename, linenum);
    }
    t->error = 1;
    return 0;
}

/*
 * scanattr() scans an attribute such as:  name="value".  If
 * successful we advance t->scanptr to the character after the
 * attribute and return a copy of the attribute value.  Otherwise
 * we return null.  We accept double or single quotes around "value".
 * We accept no quotes if "value" contains only letters, digits,
 * '.' or '-'.
 */

static char *
scanattr(template *t, const char *attrname, const char *p) {
    int i = strlen(attrname);
    int quote = 0;
    char *ret;

    if (strncasecmp(p, attrname, i) != 0) {
        return 0;
    }
    p = scanspaces(t, p + i);
    if (*p++ != '=') {
        return 0;
    }
    p = scanspaces(t, p);
    if (*p == '"' || *p == '\'') {
        quote = *p++;
    }

    /* p now points to the start of the attribute value */

    if (quote != 0) {
        for (i = 0; p[i] != quote && p[i] != '\n' && p[i] != 0; i++)
            ;
        if (p[i] != quote) {
            return 0;
        }
        t->scanptr = p + i + 1;
    }
    else {
        for (i = 0; isalnum(p[i]) || p[i] == '.' || p[i] == '-'; i++)
            ;
        if (i == 0) {
            return 0;
        }
        t->scanptr = p + i;
    }

    /* i is now the length of the attribute value */

    ret = (char *) mymalloc(i + 1);
    memcpy(ret, p, i);
    ret[i] = 0;
    return ret;
}

/*
 * findfmt() looks up a format function by name.  If successful
 * we return a pointer to the function, otherwise we return null.
 */

static TMPL_fmtfunc
findfmt(const TMPL_fmtlist *fmtlist, const char *name) {
    for (; fmtlist != 0; fmtlist = fmtlist->next) {
        if (strcmp(fmtlist->name, name) == 0) {
            return fmtlist->fmtfunc;
        }
    }
    return 0;
}

/*
 * scantag() scans a template tag.  If successful we return a tagnode
 * for the tag and advance t->scanptr to the first character after the
 * tag.  Otherwise we clean up and return null.
 */

static tagnode *
scantag(template *t, const char *p) {
    tag_kind kind;
    int commentish = 0; /* true if tag enclosed by <!-- and --> */
    int hasname = 0;    /* true if tag has name= attribute */
    int container = 0;  /* true if tag may not end with /> */
    tagnode *tag;
    int linenum = t->linenum;
    int len, level;
    char *name = 0, *value = 0, *fmt = 0;
    TMPL_fmtfunc func;
    const char *err = "";

    if (*p++ != '<') {
        return 0;
    }
    if (p[0] == '!' && p[1] == '-' && p[2] == '-') {
        commentish = 1;
        p = scanspaces(t, p + 3);
    }
    t->tagline = t->linenum;   /* tag name is on this line */

    if (strncasecmp(p, "TMPL_VAR", len = 8) == 0) {
        kind = tag_var;
        hasname = 1;
    }
    else if (strncasecmp(p, "TMPL_INCLUDE", len = 12) == 0) {
        kind = tag_include;
        hasname = 1;
    }
    else if (strncasecmp(p, "TMPL_IF", len = 7) == 0) {
        kind = tag_if;
        hasname = 1;
        container = 1;
    }
    else if (strncasecmp(p, "TMPL_ELSIF", len = 10) == 0) {
        kind = tag_elsif;
        hasname = 1;
    }
    else if (strncasecmp(p, "TMPL_ELSE", len = 9) == 0) {
        kind = tag_else;
    }
    else if (strncasecmp(p, "/TMPL_IF", len = 8) == 0) {
        kind = tag_endif;
        container = 1;
    }
    else if (strncasecmp(p, "TMPL_LOOP", len = 9) == 0) {
        kind = tag_loop;
        hasname = 1;
        container = 1;
    }
    else if (strncasecmp(p, "/TMPL_LOOP", len = 10) == 0) {
        kind = tag_endloop;
        container = 1;
    }
    else if (strncasecmp(p, "TMPL_BREAK", len = 10) == 0) {
        kind = tag_break;
    }
    else if (strncasecmp(p, "TMPL_CONTINUE", len = 13) == 0) {
        kind = tag_cont;
    }
    else {
        kind = 0;
        goto failure;
    }
    t->scanptr = p + len;

    /* white space required between tag name and attributes */

    p = scanspaces(t, t->scanptr);
    if (hasname != 0 && p == t->scanptr) {
        goto failure;
    }

    /*
     * These tags require one "name =" attribute. The TMPL_VAR tag
     * may have optional "fmt =" and "default =" attributes.  The
     * TMPL_IF and TMPL_ELSIF tags may have an optional "value ="
     * attribute. Attributes can come in any order.
     */

    switch(kind) {

    case tag_include:
    case tag_loop:
        if ((name = scanattr(t, "name", p)) != 0) {
            p = scanspaces(t, t->scanptr);
        }
        break;

    case tag_var:
        while ((name  == 0 && (name  = scanattr(t, "name",    p)) != 0) ||
               (fmt   == 0 && (fmt   = scanattr(t, "fmt",     p)) != 0) ||
               (value == 0 && (value = scanattr(t, "default", p)) != 0))
        {
            p = scanspaces(t, t->scanptr);
        }
        break;

    case tag_if:
    case tag_elsif:
        while ((name  == 0 && (name  = scanattr(t, "name",  p)) != 0) ||
               (value == 0 && (value = scanattr(t, "value", p)) != 0))
        {
            p = scanspaces(t, t->scanptr);
        }
        break;

    /*
     * These tags may have an optional "level =" attribute, which
     * must be preceded by white space.
     */

    case tag_break:
    case tag_cont:
        if (p != t->scanptr &&
            (value = scanattr(t, "level", p)) != 0)
        {
            p = scanspaces(t, t->scanptr);
        }
        break;
    }

    /* check for end of tag */

    if (commentish == 0 && p[0] == '>') {
        t->scanptr = p + 1;
    }
    else if (commentish == 0 && container == 0 &&
        p[0] == '/' && p[1] == '>')
    {
        t->scanptr = p + 2;
    }
    else if (commentish != 0 && p[0] == '-' &&
        p[1] == '-' && p[2] == '>')
    {
        t->scanptr = p + 3;
    }
    else {
        goto failure;
    }

    /* check attributes and build tag node */

    if (hasname != 0 && name == 0) {
        err = "(missing \"name=\" attribute) ";
        goto failure;
    }

    switch(kind) {

    case tag_var:
        func = 0;
        if (fmt != 0) {
            if ((func = findfmt(t->fmtlist, fmt)) == 0) {
                err = "(bad \"fmt=\" attribute) ";
                goto failure;
            }
            free(fmt);
        }
        tag = newtag(t, kind);
        tag->tag.var.varname = name;
        tag->tag.var.dfltval = value;
        tag->tag.var.fmtfunc = func;
        break;

    case tag_include:
        if (t->include_depth >= MAX_INCLUDE_DEPTH) {
            err = "(check for include cycle) ";
            goto failure;
        }
        tag = newtag(t, kind);
        tag->tag.include.filename = name;
        tag->tag.include.tmpl = 0;
        break;

    case tag_loop:
        tag = newtag(t, kind);
        tag->tag.loop.loopname = name;
        tag->tag.loop.body = 0;
        break;

    case tag_break:
    case tag_cont:
        if (t->loop_depth < 1) {
            err = "(not inside a loop) ";
            goto failure;
        }
        level = 1;
        if (value != 0) {
            if ((level = atoi(value)) < 1 || level > t->loop_depth) {
                err = "(bad \"level=\" attribute) ";
                goto failure;
            }
            free(value);
        }
        tag = newtag(t, kind);
        tag->tag.breakcont.level = level;
        break;

    case tag_if:
    case tag_elsif:
        tag = newtag(t, kind);
        tag->tag.ifelse.varname = name;
        tag->tag.ifelse.testval = value;
        tag->tag.ifelse.tbranch = 0;
        tag->tag.ifelse.fbranch = 0;
        break;

    default:
        tag = newtag(t, kind);
        break;
    }
    return tag;

failure:

    /* restore line number, clean up and return null */

    t->linenum = linenum;
    if (name != 0) {
        free(name);
    }
    if (value != 0) {
        free(value);
    }
    if (fmt != 0) {
        free(fmt);
    }
    if (kind != 0 && t->errout != 0) {
        fprintf(t->errout, "Ignoring bad %s tag %sin file \"%s\" line %d\n",
            tagname(kind), err, t->filename, t->tagline);
    }
    return 0;
}

/*
 * scan() is the main scanner function.  We return the next text sequence
 * or template tag in t->curtag or we set it to null at the end of the
 * template.  We start scanning at t->scanptr and when we are done,
 * t->scanptr points to where the next call to scan() should
 * start scanning.  We scan text until we find a tag or comment.  If we
 * find a comment, then we return the text.  If we find a tag, then we
 * save it in t->nexttag and return the text.  We will return t->nexttag
 * the next time scan() is called.  If we find a tag with no preceding
 * text, then we return the tag.  If we find a comment with no preceding
 * text, then we try again.
 */

static void
scan(template *t) {
    tagnode *tag = 0;
    const char *p;
    int i;

    if (t->nexttag != 0) {   /* return tag from previous call */
        t->curtag = t->nexttag;
        t->nexttag = 0;
        return;
    }

    /* scan text until we find a tag or a comment or null */

    p = t->scanptr;
    for (i = 0; p[i] != 0; i++) {
        if (p[i] == '\n') {
            t->linenum++;
        }
        if (p[i] != '<') {
            continue;
        }

        /* we found a '<' so we look for a comment or tag */

        if (scancomment(t, p + i) != 0) {
            if (i == 0) {
                scan(t);  /* no text so try again */
                return;
            }
            break;
        }
        if ((tag = scantag(t, p + i)) != 0) {
            break;
        }
    }

    /*
     * At this point p is where we started scanning and p[i] is
     * the first character of the tag or comment that ended this
     * scan or else p[i] is the null at the end of the template.
     */

    if (p[i] == 0) {
        t->scanptr = p + i;
    }
    if (i > 0) {
        t->nexttag = tag;            /* save the tag (if any)    */
        tag = newtag(t, tag_text);   /* return the text sequence */
        tag->tag.text.start = p;
        tag->tag.text.len   = i;
    }
    t->curtag = tag;
}

/*
 * PARSER FUNCTIONS
 *
 * forward declaration for recursive calls
 */

static tagnode *parselist(template *t, int stop);

/*
 * parseif() parses a TMPL_IF statement, which looks like this:
 *
 * <TMPL_IF name = "varname" value = "testvalue" >
 *    template-list
 * <TMPL_ELSIF name = "varname" value = "testvalue" >
 *    template-list
 * <TMPL_ELSE>
 *    template-list
 * </TMPL_IF>
 *
 * A template-list is any sequence (including an empty sequence)
 * of text, template tags, if statements or loop statements.  There
 * can be zero or more TMPL_ELSIF tags followed by zero or one
 * TMPL_ELSE tag.  There must be a final /TMPL_IF tag.
 *
 * "iftag" is a TMPL_IF tagnode, which has pointers for a true branch
 * and a false branch.  We construct a parse tree for the if statement
 * with "iftag" at the root.  When we are done t->curtag points to
 * the tag that follows the /TMPL_IF tag for this statement.
 */

static void
parseif(template *t, int stop) {
    tagnode *iftag = t->curtag;
    int linenum = t->tagline;
    int mystop = stop | tag_else | tag_elsif | tag_endif;

    iftag->tag.ifelse.tbranch = parselist(t, mystop);
    while (t->curtag != 0 && t->curtag->kind == tag_elsif) {
        iftag->tag.ifelse.fbranch = t->curtag;
        iftag = t->curtag;
        iftag->tag.ifelse.tbranch = parselist(t, mystop);
    }
    if (t->curtag != 0 && t->curtag->kind == tag_else) {
        iftag->tag.ifelse.fbranch = parselist(t, stop | tag_endif);
    }
    if (t->curtag != 0 && t->curtag->kind == tag_endif) {
        scan(t);  /* success, scan next tag */
    }
    else {
        if (t->errout != 0) {
            fprintf(t->errout, "TMPL_IF tag in file \"%s\" line %d "
                "has no /TMPL_IF tag\n", t->filename, linenum);
        }
        t->error = 1;
    }
}

/*
 * parseloop() parses a TMPL_LOOP statement which looks like this:
 *
 * <TMPL_LOOP name = "loopname">
 *   template-list
 * </TMPL_LOOP>
 *
 * "looptag" is a TMPL_LOOP tagnode, which has a pointer for the
 * loop body.  We construct a parse tree for the loop statement
 * with "looptag" at the root.  When we are done, t->curtag points
 * to the tag that follows the /TMPL_LOOP tag for this statement.
 */

static void
parseloop(template *t, int stop) {
    tagnode *looptag = t->curtag;
    int linenum = t->tagline;

    t->loop_depth++;
    looptag->tag.loop.body = parselist(t, stop | tag_endloop);
    t->loop_depth--;

    if (t->curtag != 0 && t->curtag->kind == tag_endloop) {
        scan(t);  /* success, scan next tag */
    }
    else {
        if (t->errout != 0) {
            fprintf(t->errout, "TMPL_LOOP tag in file \"%s\" line %d "
                "has no /TMPL_LOOP tag\n", t->filename, linenum);
        }
        t->error = 1;
    }
}

/*
 * parselist() is the top level parser function.  It parses a
 * template-list which is any sequence of text, TMPL_VAR tags,
 * TMPL_INCLUDE tags, if statements or loop statements.
 * We return a parse tree which is a linked list of tagnodes.  The
 * "stop" parameter is a bitmap of tag kinds that we expect to end
 * this list.  For example, if we are parsing the template-list
 * following a TMPL_IF tag, then we expect the list to end with a
 * TMPL_ELSIF tag or TMPL_ELSE tag or /TMPL_IF tag.  If we are parsing the
 * template-list that comprises the entire template, then "stop" is zero
 * so that we keep going to the end of the template.  When we are done,
 * t->curtag is the tag that caused us to stop, or null at the end of
 * the template.
 */

static tagnode *
parselist(template *t, int stop) {
    tagnode *list = 0, *tail, *tag;

    scan(t);
    while ((tag = t->curtag) != 0) {
        switch(tag->kind) {

        case tag_elsif:    /* check for terminator tag */
        case tag_else:
        case tag_endif:
        case tag_endloop:
            if ((tag->kind & stop) != 0) {
                return list;
            }

            /* unexpected terminator tag -- keep going */

            if (t->errout != 0) {
                fprintf(t->errout, "Unexpected %s tag in file \"%s\" "
                    "line %d\n", tagname(tag->kind), t->filename,
                    t->tagline);
            }
            t->error = 1;
            scan(t);
            if (tag->kind == tag_elsif) {
                break;  /* tag linked to list to be freed later */
            }
            continue;   /* tag not linked to list */

        case tag_if:
            parseif(t, stop);
            break;

        case tag_loop:
            parseloop(t, stop);
            break;

        default:
            scan(t);
            break;
        }

        /* link the tag into the list of tags */

        tag->next = 0;
        if (list == 0) {
            list = tail = tag;
        }
        else {
            tail->next = tag;
            tail = tag;
        }
    }
    return list;
}

/*
 * PARSE TREE WALKING FUNCTIONS
 *
 * valueof() looks up a variable by name and returns its value
 * or returns null if not found.  We search "varlist" and any
 * enclosing variable lists.  The parent of "varlist" is a
 * loop variable, whose parent is a variable list that encloses
 * "varlist".
 */

static char *
valueof(const char *varname, const TMPL_varlist *varlist) {
    TMPL_var *var;

    while(varlist != 0) {
        for (var = varlist->var; var != 0; var = var->next) {
            if (strcmp(varname, var->name) == 0) {
                return var->value;
            }
        }
        varlist = varlist->parent == 0 ? 0 : varlist->parent->parent;
    }
    return 0;
}

/*
 * findloop() looks up a loop variable by name and returns it or
 * returns null if not found.  We search "varlist" and any
 * enclosing variable lists.
 */

static TMPL_loop *
findloop(const char *loopname, const TMPL_varlist *varlist) {
    TMPL_loop *loop;

    while (varlist != 0) {
        for (loop = varlist->loop; loop != 0; loop = loop->next) {
            if (strcmp(loopname, loop->name) == 0) {
                return loop;
            }
        }
        varlist = varlist->parent == 0 ? 0 : varlist->parent->parent;
    }
    return 0;
}

/*
 * istrue() evaluates a TMPL_IF (or TMPL_ELSIF) tag for true or false.
 *
 * <TMPL_IF name="varname"> is true if 1) simple variable "varname"
 * exists and is not the null string or 2) the loop variable "varname"
 * exists.  Otherwise false.
 *
 * <TMPL_IF name="varname" value=""> is true if "varname" does not exist
 * or if "varname" has a null value.
 *
 * <TMPL_IF name="varname" value="testvalue"> is true if simple variable
 * "varname" has value "testvalue". Otherwise false.
 */

static int
is_true(const tagnode *iftag, const TMPL_varlist *varlist) {
    const char *testval = iftag->tag.ifelse.testval;
    const char *value;
    TMPL_loop *loop = 0;

    if ((value = valueof(iftag->tag.ifelse.varname, varlist)) == 0) {
        loop = findloop(iftag->tag.ifelse.varname, varlist);
    }

    return
        (testval == 0 && value != 0 && *value != 0) ||

        (testval == 0 && loop != 0) ||

        (testval != 0 && *testval == 0 && loop == 0 &&
        (value == 0 || *value == 0)) ||

        (testval != 0 && value != 0 && strcmp(value, testval) == 0);
}

/*
 * write_text() writes a text sequence handling \ escapes.
 *
 * A single \ at the end of a line is not output and neither
 * is the line terminator (\n or \r\n).
 *
 * \\ at the end of a line is output as a single \ followed
 * by the line terminator.
 *
 * Any other \ is output unchanged.
 */

static void
write_text(const char *p, int len, FILE *out) {
    int i, k;

    for (i = 0; i < len; i++) {

        /* check for \ or \\ before \n or \r\n */

        if (p[i] == '\\') {
            k = i + 1;
            if (k < len && p[k] == '\\') {
                k++;
            }
            if (k < len && p[k] == '\r') {
                k++;
            }
            if (k < len && p[k] == '\n') {
                if (p[i + 1] == '\\') {
                    i++;      /* skip first \ */
                }
                else {
                    i = k;    /* skip \ and line terminator */
                    continue;
                }
            }
        }
        fputc(p[i], out);
    }
}

/*
 * newfilename() returns a copy of an include file name with
 * possible modifications.  If the include file name begins
 * with ".../" then we replace "..." with the directory name
 * of the parent template file.  If there is no directory
 * name then we strip ".../".
 */

static const char *
newfilename(const char *inclfile, const char *parentfile) {
    char *newfile, *cp;

    newfile = mymalloc(strlen(parentfile) + strlen(inclfile));
    if (strncmp(inclfile, ".../", 4) != 0) {
        return strcpy(newfile, inclfile);
    }
    strcpy(newfile, parentfile);
    cp = strrchr(newfile, '/');
    strcpy(cp == 0 ? newfile : cp + 1, inclfile + 4);
    return newfile;
}

/*
 * walk() walks the template parse tree and outputs the result.  We
 * process the tree nodes according to the data in "varlist".
 */

static void
walk(template *t, tagnode *tag, const TMPL_varlist *varlist) {
    const char *value;
    TMPL_loop *loop;
    TMPL_varlist *vl;
    template *t2;
    const char *newfile;

    /*
     * if t->break_level is non zero then we are unwinding the
     * recursion after encountering a TMPL_BREAK tag.  The same
     * is true for t->cont_level and TMPL_CONTINUE.
     */

    if (tag == 0 || t->break_level > 0 || t->cont_level > 0 ||
        t->error != 0)
    {
        return;
    }
    switch(tag->kind) {

    case tag_text:
        write_text(tag->tag.text.start, tag->tag.text.len, t->out);
        break;

    case tag_var:
        if ((value = valueof(tag->tag.var.varname, varlist)) == 0 &&
            (value = tag->tag.var.dfltval) == 0)
        {
            break;
        }

        /* Use the tag's format function or else just use fputs() */

        if (tag->tag.var.fmtfunc != 0) {
            tag->tag.var.fmtfunc(value, t->out);
        }
        else {
            fputs(value, t->out);
        }
        break;

    case tag_if:
    case tag_elsif:
        if (is_true(tag, varlist)) {
            walk(t, tag->tag.ifelse.tbranch, varlist);
        }
        else {
            walk(t, tag->tag.ifelse.fbranch, varlist);
        }
        break;

    case tag_loop:
        if ((loop = findloop(tag->tag.loop.loopname, varlist)) == 0) {
            break;
        }

        for (vl = loop->varlist; vl != 0; vl = vl->next) {
            walk(t, tag->tag.loop.body, vl);

            /*
             * if t->break_level is nonzero then we encountered a
             * TMPL_BREAK tag inside this TMPL_LOOP so we need to
             * break here.
             */

            if (t->break_level > 0) {
                t->break_level--;
                break;
            }

            /*
             * if t->cont_level is nonzero then we encountered a
             * TMPL_CONTINUE inside this TMPL_LOOP.  Depending
             * on the level we either break here or continue
             */

            if (t->cont_level > 0 && --t->cont_level > 0) {
                break;
            }
        }
        break;

    /*
     * For a TMPL_BREAK or TMPL_CONTINUE tag we terminate the walk
     * of this TMPL_LOOP body and set t->break_level or t->cont_level
     * to unwind the recursion.
     */

    case tag_break:
        t->break_level = tag->tag.breakcont.level;
        return;

    case tag_cont:
        t->cont_level = tag->tag.breakcont.level;
        return;

    case tag_include:

        /* if first visit, open and parse the included file */

        if ((t2 = tag->tag.include.tmpl) == 0) {
            newfile = newfilename(tag->tag.include.filename, t->filename);
            t2 = newtemplate(newfile, 0, t->fmtlist,
                t->out, t->errout);
            if (t2 == 0) {
                free((void *) newfile);
                t->error = 1;
                break;
            }
            tag->tag.include.tmpl = t2;
            t2->include_depth = t->include_depth + 1;
            t2->roottag = parselist(t2, 0);
        }

        /* walk the included file's parse tree */

        walk(t2, t2->roottag, varlist);
        t->error = t2->error;
        break;
    }
    walk(t, tag->next, varlist);
}

/*
 * EXPORTED FUNCTIONS
 *
 * TMPL_add_var() adds one or more simple variables to variable list
 * "varlist" and returns the result.  If "varlist" is null, then we
 * create it.  The parameter list has a variable number of "char *"
 * parameters terminated by a null parameter.  Each pair of parameters
 * is a variable name and value that we store in a TMPL_var struct and
 * link into "varlist".
 */

TMPL_varlist *
TMPL_add_var(TMPL_varlist *varlist, ...) {
    va_list ap;
    const char *name, *value;
    TMPL_var *var;
    int nlen, vlen;

    va_start(ap, varlist);
    while ((name = va_arg(ap, char *)) != 0 &&
        (value = va_arg(ap, char *)) != 0)
    {
        /*
         * get enough memory to store the TMPL_var struct and
         * the name string and the value string
         */

        nlen = strlen(name) + 1;
        vlen = strlen(value) + 1;
        var = (TMPL_var *) mymalloc(sizeof(*var) + nlen + vlen);
        strcpy(var->value, value);
        var->name = strcpy(var->value + vlen, name);
        if (varlist == 0) {
            varlist = (TMPL_varlist *) mymalloc(sizeof(*varlist));
            memset(varlist, 0, sizeof(*varlist));
        }
        var->next = varlist->var;
        varlist->var = var;
    }
    va_end(ap);
    return varlist;
}

/*
 * TMPL_add_loop() adds loop variable "loop" to variable list "varlist"
 * and returns the result.  If "varlist" is null, then we create it.
 * We decline to add "loop" if 1) "loop" has already been added to a
 * variable list or 2) adding "loop" would create a cycle because
 * "loop" contains "varlist".
 */

TMPL_varlist *
TMPL_add_loop(TMPL_varlist *varlist, const char *name, TMPL_loop *loop) {
    TMPL_loop *lp;

    /* if sanity check fails, just return */

    if (name == 0 || loop == 0 || loop->parent != 0) {
        return varlist;
    }
    if (varlist == 0) {
        varlist = (TMPL_varlist *) mymalloc(sizeof(*varlist));
        memset(varlist, 0, sizeof(*varlist));
    }

    /* if sanity check for cycle fails, just return */

    for (lp = varlist->parent; lp != 0;
        lp = lp->parent == 0 ? 0 : lp->parent->parent)
    {
        if (lp == loop) {
            return varlist;
        }
    }
    loop->name = strdup(name);
    loop->parent = varlist;
    loop->next = varlist->loop;
    varlist->loop = loop;
    return varlist;
}

/*
 * TMPL_add_varlist() adds variable list "varlist" to loop variable
 * "loop" and returns the result.  If "loop" is null, then we create it.
 * We decline to add "varlist" if 1) "varlist" has already been added
 * to a loop variable or 2) adding "varlist" would create a cycle
 * because "varlist" contains "loop".
 */

TMPL_loop *
TMPL_add_varlist(TMPL_loop *loop, TMPL_varlist *varlist) {
    TMPL_varlist *vl;

    /* if sanity check fails, just return */

    if (varlist == 0 || varlist->parent != 0) {
        return loop;
    }
    if (loop == 0) {
        loop = (TMPL_loop *) mymalloc(sizeof(*loop));
        memset(loop, 0, sizeof(*loop));
    }

    /* if sanity check for cycle fails, just return */

    for (vl = loop->parent; vl != 0;
        vl = vl->parent == 0 ? 0 : vl->parent->parent)
    {
        if (vl == varlist) {
            return loop;
        }
    }
    varlist->parent = loop;
    varlist->next = 0;
    if (loop->varlist == 0) {
        loop->varlist = loop->tail = varlist;
    }
    else {
        loop->tail->next = varlist;
        loop->tail = varlist;
    }
    return loop;
}

/* TMPL_free_varlist() recursively frees memory used by a TMPL_varlist */

void
TMPL_free_varlist(TMPL_varlist *varlist) {
    TMPL_loop *loop, *loopnext;
    TMPL_var  *var,  *varnext;

    if (varlist == 0) {
        return;
    }
    for (loop = varlist->loop; loop != 0; loop = loopnext) {
        loopnext = loop->next;
        TMPL_free_varlist(loop->varlist);
        free((void *) loop->name);
        free(loop);
    }
    for (var = varlist->var; var != 0; var = varnext) {
        varnext = var->next;
        free(var);
    }
    TMPL_free_varlist(varlist->next);
    free(varlist);
}

/*
 * TMPL_add_fmt() adds a name and function pointer to format function
 * list "fmtlist" and returns the result.  If "fmtlist" is null, then
 * we create it, otherwise we return "fmtlist".  Parameter "name" is
 * the name used in the in the fmt="fmtname" attribute of a TMPL_VAR
 * tag.  Parameter "fmtfunc" is a pointer to a user supplied function
 * with a prototype like this:
 *
 *   void funcname(const char *value, FILE *out);
 *
 * The function should output "value" to "out" with appropriate
 * formatting or encoding.
 */

TMPL_fmtlist *
TMPL_add_fmt(TMPL_fmtlist *fmtlist, const char *name,
    TMPL_fmtfunc fmtfunc)
{
    TMPL_fmtlist *newfmt;
    if (name == 0 || fmtfunc == 0) {
        return fmtlist;
    }
    newfmt = (TMPL_fmtlist *) mymalloc(sizeof(*newfmt) + strlen(name));
    strcpy(newfmt->name, name);
    newfmt->fmtfunc = fmtfunc;
    if (fmtlist == 0) {
        newfmt->next = 0;
        return newfmt;
    }

    /* if fmtlist is not null, then we return its original value */

    newfmt->next = fmtlist->next;
    fmtlist->next = newfmt;
    return fmtlist;
}

/* TMPL_free_fmtlist frees memory used by a format function list */

void
TMPL_free_fmtlist(TMPL_fmtlist *fmtlist) {
    if (fmtlist != 0) {
        TMPL_free_fmtlist(fmtlist->next);
        free(fmtlist);
    }
}

/*
 * TMPL_write() outputs a template to open file pointer "out" using
 * variable list "varlist".  If "tmplstr" is null, then we read the
 * template from "filename", otherwise "tmplstr" is the template.
 * Parameter "fmtlist" is a format function list that contains
 * functions that TMPL_VAR tags can specify to output variables.
 * We return 0 on success otherwise -1.  We write errors to open
 * file pointer "errout".
 */

int
TMPL_write(const char *filename, const char *tmplstr,
    const TMPL_fmtlist *fmtlist, const TMPL_varlist *varlist,
    FILE *out, FILE *errout)
{
    int ret;
    template *t;

    if ((t = newtemplate(filename, tmplstr, fmtlist, out, errout)) == 0) {
        return -1;
    }
    t->roottag = parselist(t, 0);
    walk(t, t->roottag, varlist);
    ret = t->error == 0 ? 0 : -1;
    if (tmplstr == 0 && t->tmplstr != 0) {
        free((void *) t->tmplstr);
    }
    freetag(t->roottag);
    free(t);
    return ret;
}

/*
 * Some handy format functions
 *
 * TMPL_encode_entity() converts HTML markup characters to entities
 */

void
TMPL_encode_entity(const char *value, FILE *out) {
    for (; *value != 0; value++) {
        switch(*value) {

        case '&':
            fputs("&amp;", out);
            break;

        case '<':
            fputs("&lt;", out);
            break;

        case '>':
            fputs("&gt;", out);
            break;

        case '"':
            fputs("&quot;", out);
            break;

        case '\'':
            fputs("&#39;", out);
            break;

        case '\n':
            fputs("&#10;", out);
            break;

        case '\r':
            fputs("&#13;", out);
            break;

        default:
            fputc(*value, out);
            break;
        }
    }
}

/* TMPL_encode_url() does URL encoding (%xx)  */

void
TMPL_encode_url(const char *value, FILE *out) {
    static const char hexdigit[] = "0123456789ABCDEF";
    int c;

    for (; *value != 0; value++) {
        if (isalnum(*value) || *value == '.' ||
            *value == '-' || *value == '_')
        {
            fputc(*value, out);
            continue;
        }
        if (*value == ' ') {
            fputc('+', out);
            continue;
        }
        c = (unsigned char) *value;
        fputc('%', out);
        fputc(hexdigit[c >> 4],  out);
        fputc(hexdigit[c & 0xf], out);
    }
}
