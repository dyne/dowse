#ifndef ATTRIBUTES_HM
#define ATTRIBUTES_HM
#include "hashmap.h"

/* Inheritance ... in C .
 * */
typedef map_t attributes_hm_t ;


/*
 * To integrate "mustache_c" will use an "hashmap of hashmap" to retrieve element in our data modelization
 * (watch in template.c how we retrieve element).
 */

/*
 * The "attributes" can be "simple" or "structured"
*  eg:
*   # Simple attributes
*   "title" "Dowse panel" is
*
*   # Structured attributes
*
*     In this case the "value" it's an attributes_hm_t[].
*
*     eg:
*     found= {"macaddr":"00:11...", "ipv4": "192.168.0.1" }
*
*     found it's descripted using an hashmap .
*
*      HM= {
*           1 : found[0]
*           2 : found[1]
*
*		attributes_t.value = &(found[0])
*		attributes_t.size = 2
*		attributes_t.isSimple = 0
*
*  */


typedef struct attributes_vect_t {
	int isSimple;
	char **simple_; /* if it's Simple it's a char*[]*/
	attributes_hm_t*complex_;/* if it's structured it's a attributes_hm_t[]  */
/*	any_t value;*/

	int size; /*--- If the attributes is "complex" it s an*/
} attributes_vect_t;


int debug_attributes(attributes_hm_t hm);
attributes_hm_t attrinit(void);
void attrget(attributes_hm_t hm, char *key, int index, any_t *value);
void attrcat(attributes_hm_t hm, char *key, char *value);
void attr_add(attributes_hm_t hm, char *key, attributes_hm_t value);
void attrfree(attributes_hm_t hm);


#endif
