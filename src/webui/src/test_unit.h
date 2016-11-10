/*
 * test_unit.h
 *
 *  Created on: 07 nov 2016
 *      Author: nop
 */
#include "webui_debug.h"

#ifndef SRC_TEST_UNIT_H_
#define SRC_TEST_UNIT_H_


#define WEBUI_TEST_UNIT(NAME) int ___webui___test_unit__ ## NAME (struct http_request *__webui_req)

#define __OK_MESSAGE "<h1>Ok</h1>"
#define __KO_MESSAGE(NAME) "<h1><strong>Sorry it doesn't work test [" #NAME "]<strong></h1>"

#define RETURN_ASSERT(test) {\
	if (test) {\
		http_response(__webui_req, 200, __OK_MESSAGE , strlen(__OK_MESSAGE));\
	} else {\
		http_response(__webui_req, 404, __KO_MESSAGE(NAME), strlen(__KO_MESSAGE(NAME)));\
	}\
	return (test);\
}



#endif /* SRC_TEST_UNIT_H_ */
