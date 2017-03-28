#include <webui.h>

int websocket_example(struct http_request *req) {
	// template_t tmpl;
	attributes_set_t attributes;
	struct kore_buf *buf;
	char *address;

	// allocate output buffer
	buf = kore_buf_alloc(1024*1000);

	// load template
	template_load
		("assets/websocket.html", &tmpl);
	attributes = attrinit();

	address = getenv("address");
	if(!address) address = "127.0.0.1";

	attrcat(attributes, "address", address);


	template_apply(&tmpl,attributes,buf);

	http_response_header(req, "content-type", "text/html");
	http_response(req, 200, buf->data, buf->offset);


	template_free(&tmpl);
	attrfree(attributes);

	kore_buf_free(buf);

	return(KORE_RESULT_OK);
}
                     
