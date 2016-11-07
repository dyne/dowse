/* This simple example reads the file "simple.template" from the current
 * directory and replaces the variable "name" with a self defined string.
 * It shows basic usage of the mustache-c API.
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include <mustache.h>

static FILE *template = NULL;

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

static void
mustache_error(mustache_api_t *api, void *data,
               uintmax_t line, char const *error)
{
    fprintf(stderr, "error in template: %" PRIu64  ": %s\n", line, error);
}

static uintmax_t
mustache_sectget(mustache_api_t *api, void *data, mustache_token_section_t *s)
{
    /* Compare to s->name and do section related stuff. This usually revolves
     * around calling mustache_render() again on the section itself.
     *
     * See array.c for an example on how to handle arrays.
     */
    return mustache_render(api, data, s->section);
}

static uintmax_t
mustache_varget(mustache_api_t *api, void *data, mustache_token_variable_t *t)
{
    static const char *name = "Angus McFife";

    if (strcmp(t->text, "name") == 0) {
        api->write(api, data, name, strlen(name));
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

    template = fopen("simple.template", "r");
    if (template == NULL) {
        fprintf(stderr, "error: failed to open file simple.template: %s\n",
                strerror(errno)
            );
        return 2;
    }

    api.read = mustache_read_file;
    api.write = mustache_write_stdout;
    api.error = mustache_error;
    api.sectget = mustache_sectget;
    api.varget = mustache_varget;

    t = mustache_compile(&api, NULL);
    if (t == NULL) {
        fclose(template);
        return 3;
    }

    if (!mustache_render(&api, NULL, t)) {
        fclose(template);
        return 3;
    }

    fclose(template);

    return 0;
}
