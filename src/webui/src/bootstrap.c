#include <kore.h>
#include <http.h>
#include <assets.h>


int bootstrap_css(struct http_request *req) {
    http_response(req,200,asset_bootstrap_min_css,asset_len_bootstrap_min_css);
    return(KORE_RESULT_OK);
}
