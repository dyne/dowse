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
	map_t assetmap = NULL;
	char *asset_name;
	asset_t *rasset;

	
	func("CALL: serve_asset");


	http_populate_get(req);

	if( http_argument_get_string(req, "o", &asset_name) ) {
		func("asset file requested: %s", asset_name);

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

    return apply_template_and_return(req, global_attributes,asset_welcome_html,asset_len_welcome_html,200);
	/*
	struct kore_buf*out;
	struct template_t tmpl;
	out=kore_buf_alloc(0);
	template_load(asset_welcome_html,asset_len_welcome_html,&tmpl);
	template_apply(&tmpl,global_attributes,out);

	char*html_rendered;int len;
	html_rendered=kore_buf_release(out,&len);

	http_response_header(req, "content-type", "text/html");
	http_response(req, 200, html_rendered,len);

	kore_free(html_rendered);

	return(KORE_RESULT_OK);
	 */
}
