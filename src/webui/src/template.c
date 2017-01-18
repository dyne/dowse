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
#include "find_nearest_asset.h"



int asset_name_distance(char* a,char*b) {
    int rv=0;
    int i;

    for ( i=0;(i<(strlen(a))&&(i<(strlen(b))));) {
       if (filtered(a[i])==filtered(b[i])) {
           rv++;
           i++;
           continue;
       }
       break;
    }

    int ia=strlen(a)-1;
    int ib=strlen(b)-1;
    for ( ; ia>=0 && ib >= 0 ; ) {
       if (filtered(a[ia])==filtered(b[ib])) {
           rv++;
           ia--;
           ib--;
           continue;
       }
       break;
    }

    return rv;
}


int print_nearest_name(char*prefix,struct dirent *entry,void*data) {
    char entry_file_name[256];
    nearest_filename* p;

    if ((entry->d_type==DT_REG) || (entry->d_type==DT_LNK)) {
        snprintf(entry_file_name,sizeof(entry_file_name),"%s/%s",prefix,entry->d_name);
        p=(nearest_filename*) data;

        int nv=p->utility_function(entry_file_name,p->name_to_search);
        if (nv > p->max_point) {
            snprintf(p->nearest,sizeof(p->nearest),"%s",entry_file_name);
            p->max_point=nv;
        }
    }
    return 0;
}

/**/
int find_nearest_asset_and_load_template(char *template_name, int len, struct template_t *tmpl) {
    /*--- We find the nearest file with the name similar to template_name */
    struct nearest_filename data_to_search;

    init_data_to_search(&data_to_search,template_name,asset_name_distance);

    span_directory(".",".",print_nearest_name,&data_to_search);

    func("Nearest of \n[%s]\n[%s]\n %d\n",
            data_to_search.name_to_search,
            data_to_search.nearest,
            data_to_search.max_point);

    /* */
    struct stat buf;
    stat(data_to_search.nearest,&buf);

    func("Nearest of \n[%s]  %d\n",
             data_to_search.name_to_search,buf.st_size);


    tmpl->data = (char*)malloc(sizeof(char)*(buf.st_size+1));
    tmpl->len = buf.st_size;
    int fd=open(data_to_search.nearest,O_SYNC|O_RDONLY);
    if (fd<0) {
        err("Error at line %s %d : %s",__FILE__,__LINE__,strerror(errno));
        exit(-1);
    }

    int rv=read(fd,tmpl->data,tmpl->len);
    if (rv!=tmpl->len) {
        err("Error at line %s %d : %s trying to open [%s][%d] readed [%d]",
                __FILE__,__LINE__,strerror(errno),data_to_search.nearest,tmpl->len,rv);
        exit(-1);
    }
    (tmpl->data)[tmpl->len]=0;
    close(fd);

    tmpl->fmtlist = TMPL_add_fmt(0, ENTITY_ESCAPE, TMPL_encode_entity);
    tmpl->fmtlist = TMPL_add_fmt(tmpl->fmtlist, URL_ESCAPE, TMPL_encode_url);


}


int _internal_static_template_load(u_int8_t *str, int len, template_t *tmpl) {
    tmpl->data = str;
    tmpl->len = len;
    tmpl->fmtlist = TMPL_add_fmt(0, ENTITY_ESCAPE, TMPL_encode_entity);
    tmpl->fmtlist = TMPL_add_fmt(tmpl->fmtlist, URL_ESCAPE, TMPL_encode_url);
    return 0;
}

void template_apply(template_t *tmpl, attributes_set_t al, struct kore_buf *out) {
    char out_name[] = "/tmp/out_stream_XXXXXX";
    char err_name[] = "/tmp/err_stream_XXXXXX";
    FILE*out_stream, *err_stream;
    char buf_str[1024], end;
    unsigned long int size;
    int some_error, rv;

    mkstemp(out_name);
    mkstemp(err_name);

    func(
            "[%s] Template applying out file was [%s] err file was [%s] ",
            __where_i_am__, out_name, err_name);

    out_stream = fopen(out_name, "rw+");
    err_stream = fopen(err_name, "rw+");

    WEBUI_DEBUG
    ;
    TMPL_write(NULL, tmpl->data, tmpl->fmtlist, al->varlist, out_stream,
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
}

void template_free(template_t *t) {

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

    template_load(template, strlen(template), &t);

    template_apply(&t, _attributes, out);

    rendered = (char*) kore_buf_release(out, &size);

    RETURN_ASSERT(strcmp(rendered, "Dowse information panel") == 0);
}

WEBUI_TEST_UNIT(A002) {
    char template[] = "<html>"
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
    int size;
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

    template_load(template, strlen(template), &t);

    template_apply(&t, _attributes, out);

    rendered = (char*) kore_buf_release(out, &size);

    RETURN_ASSERT(
            strcmp(rendered,
                    "<html><table><tr><td>Antonio</td><td>Rossi</td><td>Casa Sua</td></tr><tr><td>Mario</td><td>Bianchi</td><td>Un altro posto</td></tr></table></html>")
                    == 0);

}

WEBUI_TEST_UNIT(A003) {
    char template[] = "<html>"
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
    int size;
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

    template_load(template, strlen(template), &t);

    template_apply(&t, _attributes, out);

    rendered = (char*) kore_buf_release(out, &size);

    http_response(__webui_req, 200, rendered, size);
    return 1;
    RETURN_ASSERT(1);

}
