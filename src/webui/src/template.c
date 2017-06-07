#include <kore.h>
#include <assetmap.h>
#include <dirent.h>
#include <sys/types.h>
#define __USE_GNU
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "template.h"
#include "webui_debug.h"
#include "attributes_set.h"
#include "test_unit.h"

/**/
int template_load(const char *asset_path, template_t *tmpl) {
    /* */
    struct stat buf;
    stat(asset_path, &buf);

#ifdef DYNAMIC_ASSET_LOADING
    func("template_load in DYNAMIC WAY of [%s]  %d %d\n", __FUNCTION__, asset_path, buf.st_size);
    tmpl->data = (u_int8_t*) malloc(sizeof(char) * (buf.st_size + 1));

    tmpl->len = buf.st_size;
    int fd = open(asset_path, O_SYNC | O_RDONLY);
    if (fd < 0) {
        err("Error at line %s %d : %s", __FILE__, __LINE__, strerror(errno));
        exit(-1);
    }

    int rv = read(fd, tmpl->data, tmpl->len);
    if (rv != tmpl->len) {
        err("Error at line %s %d : %s trying to open [%s][%d] readed [%d]",
        __FILE__, __LINE__, strerror(errno), asset_path, tmpl->len, rv);
        exit(-1);
    }
    (tmpl->data)[tmpl->len] = 0;
    close(fd);

    tmpl->fmtlist = TMPL_add_fmt(0, ENTITY_ESCAPE, TMPL_encode_entity);
    tmpl->fmtlist = TMPL_add_fmt(tmpl->fmtlist, URL_ESCAPE, TMPL_encode_url);
    tmpl->static_asset = 0;
    return 0;
#else
    func("template_load in STATIC WAY of [%s]  %d %d\n", __FUNCTION__,asset_path, buf.st_size);
    map_t assetmap;
    asset_t *rasset;
    assetmap = asset_init();
    if (hashmap_get(assetmap, asset_path, (void**)&rasset) == MAP_MISSING) {
        err("Error at line %s %d : Requested a non compiled asset [%s]", __FILE__, __LINE__,asset_path);
        exit(-1);
    }
    free_assetmap(assetmap);
    _internal_static_template_load( rasset->data,rasset->len,tmpl);

#endif
}

int _internal_static_template_load(u_int8_t *str, int len, template_t *tmpl) {
    tmpl->data = str;
    tmpl->len = len;
    tmpl->fmtlist = TMPL_add_fmt(0, ENTITY_ESCAPE, TMPL_encode_entity);
    tmpl->fmtlist = TMPL_add_fmt(tmpl->fmtlist, URL_ESCAPE, TMPL_encode_url);
    tmpl->static_asset = 1;
    return 0;
}

void template_apply(template_t *tmpl, attributes_set_t al, struct kore_buf *out) {
    char out_name[] = "/tmp/out_stream_XXXXXX";
    char err_name[] = "/tmp/err_stream_XXXXXX";
    FILE*out_stream, *err_stream;
    char buf_str[1024], end;
    int some_error, rv;

    rv=mkstemp(out_name);
    if (rv==-1) {
      err("Error on create temp file [%s]",strerror(errno));
    } else {
      close(rv);
    }
    rv=mkstemp(err_name);
    if (rv==-1) {
      err("Error on create temp file [%s]",strerror(errno));
    } else {
      close(rv);
    }

    func(
            "[%s] Template applying out file was [%s] err file was [%s] ",
            __where_i_am__, out_name, err_name);

    out_stream = fopen(out_name, "rw+");
    err_stream = fopen(err_name, "rw+");

    WEBUI_DEBUG
    ;
    TMPL_write(NULL, (const char *)tmpl->data, tmpl->fmtlist, al->varlist, out_stream,
            err_stream);

//  size=ftell(out_stream);
    func( " out [%s] [%d] ", __where_i_am__, ftell(out_stream));

    some_error = 0;
    rewind(out_stream);
    rewind(err_stream);

    while ((rv = fread(buf_str, 1, sizeof(buf_str), err_stream)) > 0) {
        buf_str[rv] = 0;
        char m[1024];
        snprintf(m, sizeof(m), "%s %s", __where_i_am__, buf_str);
        err( m);
        err(m);
        some_error = 1;
    }
    if (!some_error) {
        while ((rv = fread(buf_str, 1, sizeof(buf_str), out_stream)) > 0) {
            buf_str[rv] = 0;
//	  func("%s %s",__where_i_am__,buf_str);
            kore_buf_append(out, buf_str, rv);
        }
    }

    WEBUI_DEBUG
    end = 0;

    kore_buf_append(out, &end, 1);

    fclose(out_stream);
    fclose(err_stream);

    unlink(out_name);
    unlink(err_name);

    template_free(tmpl);
}

void template_free(template_t *t) {
    if ((t->data)&&(!t->static_asset)) {
        free(t->data);
        t->data=NULL;
    } else {
        t->data=NULL;
    }
    if (t->fmtlist) {
        TMPL_free_fmtlist(t->fmtlist);
        t->fmtlist=NULL;
    }
}

WEBUI_TEST_UNIT(A001) {
    attributes_set_t _attributes;
    char *rendered;
    u_int8_t template[] = "<TMPL_var name=\"title\">";
    template_t t;
    size_t size;
    struct kore_buf *out;

    out = kore_buf_alloc(0);
    _attributes = attrinit();
    _attributes = attrcat(_attributes, "title", "Dowse information panel");

    _internal_static_template_load(template, strlen((const char*)template), &t);

    template_apply(&t, _attributes, out);

    rendered = (char*) kore_buf_release(out, &size);

    RETURN_ASSERT(strcmp(rendered, "Dowse information panel") == 0);
}

WEBUI_TEST_UNIT(A002) {
    u_int8_t template[] = "<html>"
            "<table>"
            "<TMPL_LOOP name=\"studente\">"
            "<tr>"
            "<td><TMPL_VAR name=\"nome\"></td>"
            "<td><TMPL_VAR name=\"cognome\"></td>"
            "<td><TMPL_VAR name=\"indirizzo\"></td>"
            "</tr>"
            "</TMPL_LOOP>"
            "</table>"
            "</html>";
    attributes_set_t studente, studente2, _attributes;
    template_t t;
    size_t size;
    struct kore_buf *out;
    char *rendered;

    out = kore_buf_alloc(0);
    _attributes = attrinit();

    studente = attrinit();
    studente = attrcat(studente, "nome", "Antonio");
    studente = attrcat(studente, "cognome", "Rossi");
    studente = attrcat(studente, "indirizzo", "Casa Sua");
    _attributes = attr_add(_attributes, "studente", studente);

    studente2 = attrinit();
    studente2 = attrcat(studente2, "nome", "Mario");
    studente2 = attrcat(studente2, "cognome", "Bianchi");
    studente2 = attrcat(studente2, "indirizzo", "Un altro posto");
    _attributes = attr_add(_attributes, "studente", studente2);

    _internal_static_template_load(template, strlen((const char*)template), &t);

    template_apply(&t, _attributes, out);

    rendered = (char*) kore_buf_release(out, &size);

    RETURN_ASSERT(
            strcmp(rendered,
                    "<html><table><tr><td>Antonio</td><td>Rossi</td><td>Casa Sua</td></tr><tr><td>Mario</td><td>Bianchi</td><td>Un altro posto</td></tr></table></html>")
                    == 0);

}

WEBUI_TEST_UNIT(A003) {
    u_int8_t template[] = "<html>"
            "<table>"
            "<TMPL_LOOP name=\"studente\">"
            "<tr>"
            "<td><TMPL_VAR name=\"nome\" fmt=\"" ENTITY_ESCAPE "\"></td>"
    "<td><TMPL_VAR name=\"cognome\" fmt=\"" ENTITY_ESCAPE "\"></td>"
    "<td><TMPL_VAR name=\"indirizzo\" fmt=\"" ENTITY_ESCAPE "\"></td>"
    "</tr>"
    "</TMPL_LOOP>"
    "</table>"
    "</html>";
    attributes_set_t studente, studente2, _attributes;
    template_t t;
    size_t size;
    struct kore_buf *out;
    char *rendered;

    out = kore_buf_alloc(0);
    _attributes = attrinit();

    studente = attrinit();
    studente = attrcat(studente, "nome", "Antonio");
    studente = attrcat(studente, "cognome", "Rossi");
    studente = attrcat(studente, "indirizzo", "Casa Sua");
    _attributes = attr_add(_attributes, "studente", studente);

    studente2 = attrinit();
    studente2 = attrcat(studente2, "nome", "<<Mario>>");
    studente2 = attrcat(studente2, "cognome", "Bianchi>>");
    studente2 = attrcat(studente2, "indirizzo", "Un altro posto");
    _attributes = attr_add(_attributes, "studente", studente2);

    _internal_static_template_load(template, strlen((const char*)template), &t);

    template_apply(&t, _attributes, out);

    rendered = (char*) kore_buf_release(out, &size);

    http_response(__webui_req, 200, rendered, size);
    return 1;
    RETURN_ASSERT(1);

}
