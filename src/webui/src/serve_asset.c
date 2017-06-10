#include <webui.h>

int free_assetmap_f(any_t arg, any_t element) {
	kore_free(element);
	return MAP_OK;
}
void free_assetmap(map_t amap) {
	if(!amap) return;
	hashmap_iterate(amap, free_assetmap_f, NULL);
	hashmap_free(amap);
}

int serve_asset(struct http_request *req) {
    log_entering();
	map_t assetmap = NULL;
	char *asset_name;
	asset_t *rasset;
	attributes_set_t attr=NULL;

	http_populate_get(req);

	if( http_argument_get_string(req, "o", &asset_name) ) {
		act("asset file requested: %s", asset_name);

		assetmap = asset_init();
		func("assetmap initialised");

		if( hashmap_get(assetmap, asset_name, (void**)&rasset) == MAP_MISSING) {
			free_assetmap(assetmap);
			func("asset not found (404)");
			http_response(req, 404, NULL, 0);
			return(KORE_RESULT_OK);
		}

		http_response_header(req, "content-type", rasset->type);
		http_response(req, 200, rasset->data, rasset->len);
		
		free_assetmap(assetmap);
		return(KORE_RESULT_OK);
	}

	if ( ! error_during_startup ) {
	    attr=attrinit();

	    sprintf(line,"select value as cur_state from parameter where variable='state all things'");
	    sql_select_into_attributes(line,NULL,&attr);

	    load_current_identity(req,&attr);

	    load_global_attributes(attr);
	    return apply_template_and_return(req, attr,"assets/welcome.html",200);
	} else {
        return apply_template_and_return(req, startup_attributes,"assets/welcome.html",200);
	}
}
