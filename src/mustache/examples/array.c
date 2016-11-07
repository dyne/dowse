/* This simple example reads the file "array.template" from the current
 * directory and replaces the section with an array of names. It shows on
 * how to deal with sections that are actually arrays.
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include <mustache.h>

static FILE *template = NULL;

static char *names[] =
{
    "Angus McFife",
    "Hootsman",
    "Ser Proletius",
    "Zargothrax",
    "Ralathor",
    NULL,
};

static uintmax_t
mustache_read_file(mustache_api_t *api, void *data, char *buf, uintmax_t sz)
{
    /* Do whatever needs to be done to get sz number of bytes into buf.
     */
    return fread(buf, sizeof(char), sz, template);
}

static uintmax_t
mustache_write_stdout(mustache_api_t *api, void *data,
                      char const *buf, uintmax_t sz)
{
    /* Do whatever it takes to write sz number of bytes from buf to somewhere.
     */
    return fwrite(buf, sizeof(char), sz, stdout);
}

static uintmax_t
mustache_write_null(mustache_api_t *api, void *data,
                    char const *buf, uintmax_t sz)
{
    /* Fake success
     */
    return sz;
}

static void
mustache_error(mustache_api_t *api, void *data,
               uintmax_t line, char const *error)
{
    fprintf(stderr, "error in template: %" PRIu64  ": %s\n", line, error);
}

static uintmax_t
mustache_sectget(mustache_api_t *api, void *data, mustache_token_section_t *s)
{
    if (strcmp(s->name, "members") == 0) {
        /* To resolve an array, repeatedly call mustache_render() on the
         * section but pass different user defined data. The user defined
         * data should point to one item of the array.
         */
        char **name = names;

        for (; *name; name++) {
            if (!mustache_render(api, *name, s->section)) {
                return false;
            }
        }

        return true;
    }

    return false;
}

static uintmax_t
mustache_varget(mustache_api_t *api, void *data, mustache_token_variable_t *t)
{
    if (strcmp(t->text, "name") == 0) {
        char *name = (char*)data;

        if (name == NULL) {
            fprintf(stderr, "error: name requested but not passed\n");
            return false;
        }

        api->write(api, data, (char*)name, strlen(name));
        return true;
    }

    fprintf(stderr, "error: unknown variable: %s\n", t->text);
    /* If you return false, parsing will stop. To ignore unknown variables
     * you should always return true.
     */

    return false;
}

int main(int ac, char **av)
{
    mustache_api_t api;
    mustache_template_t *t = NULL;

    template = fopen("array.template", "r");
    if (template == NULL) {
        fprintf(stderr, "error: failed to open file array.template: %s\n",
                strerror(errno)
            );
        return 2;
    }

    api.read = mustache_read_file;
    api.write = NULL;
    api.error = mustache_error;
    api.sectget = mustache_sectget;
    api.varget = mustache_varget;

    t = mustache_compile(&api, NULL);
    if (t == NULL) {
        fclose(template);
        return 3;
    }

    /* Prerender also calls sectget() and varget() handlers. If you
     * call api->write() in those handlers then stuff is written which
     * you might not want. One way to circumvent that is to have a null
     * writer.
     */
    api.write = mustache_write_null;
    if (!mustache_prerender(&api, NULL, t)) {
        fclose(template);
        return 3;
    }

    /* And then set the proper writer on the actual render call.
     */
    api.write = mustache_write_stdout;
    if (!mustache_render(&api, NULL, t)) {
        fclose(template);
        return 3;
    }

    fclose(template);

    return 0;
}
