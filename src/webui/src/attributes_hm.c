#include <kore.h>
#include <hashmap.h>
#include <attributes_hm.h>
#include "webui_debug.h"


#define MAX_PREFIX_SIZE (256)

void _print_k_idx_v_attributes(any_t nop,any_t key,int idx,any_t val){
	kore_log(LOG_DEBUG,"%s/%s[%d]\t->\t[%s]",
			(char*)(nop?nop:""),
			(char*)key,
			idx,
			(val?(char*)val:""));
}

int _debug_attributes(char*prefix,attributes_hm_t hm){
	int i,idx;
	hashmap_map * p;
	p=(hashmap_map *)hm;
	kore_log(LOG_DEBUG,"Debug attributes list [%s] of [%d] element",prefix,p->size);
	for (i=0;i<p->table_size;i++) {
		if ((p->data[i]).in_use!=0){
			attributes_vect_t *a;
			a=(attributes_vect_t *)(p->data[i].data);
			if (a->isSimple) {
				for (idx=0;idx<a->size;idx++){
					_print_k_idx_v_attributes(prefix,p->data[i].key,idx,a->simple_[idx]);
				}
			} else {
				char *prefix2;
				prefix2=(char*)malloc(MAX_PREFIX_SIZE );
				for (idx=0;idx<a->size;idx++){
					snprintf(prefix2,MAX_PREFIX_SIZE,"%s>%s[%d]",prefix,p->data[i].key,idx);
					_debug_attributes(prefix2,(a->complex_[idx]));
				}
				free(prefix2);
			}
		}
	}
	kore_log(LOG_DEBUG,"End Debug [%s] attributes list",prefix);
	return 0;
}

int debug_attributes(attributes_hm_t hm){
	return _debug_attributes("",hm);
}

/**/
attributes_hm_t attrinit(){
	return hashmap_new();
}

//-- It search the array containing all the element associated with the "key" and return the "index"-th element.
void attrget(attributes_hm_t hm,char*key,int index,any_t *value) {
	attributes_vect_t *a;

	WEBUI_DEBUG
	kore_log(LOG_DEBUG,"%s Looking for %s[%d]",__where_i_am__,key,index);
	if (index<0) {
		*value=NULL;
		return;
	}
	int rv=hashmap_get(hm,key,(any_t*)&a);

	if (rv == MAP_MISSING) {
		WEBUI_DEBUG
		*value=NULL;
		return;
	}
	WEBUI_DEBUG
	any_t *p;
	if (index>=a->size) {
		*value=NULL;
		return;
	}
	if (!a->isSimple) {
		p=(any_t*)a->complex_;
	} else {
		p=(any_t*)a->simple_;
	}
	*value=p[index];
}


/*
 *  In the the hashmap_hm_t  "hm" we add the value "value"
 *   to the array containing the "key" element .
 *   We'll add  it to the last position of the array.
 */
void _attr_add(attributes_hm_t hm, char*key,any_t value,int _is_simple){
	attributes_vect_t *present;

	/*  Where is the array of attributes_t[] that are associated with "key"? */
	hashmap_get(hm,key,(any_t *)&present);

	if (present==NULL) {
		/* it's not present in the "hm" ... */
		present=(attributes_vect_t*)kore_malloc(sizeof(attributes_vect_t));
		present->isSimple=_is_simple;
		present->simple_=NULL;
		present->complex_=NULL;
		present->size=0;
		hashmap_put(hm,key,(any_t)present);
	}
	/* it's present the array so we push_back it ... */
	any_t *_vect;
	if (_is_simple) {
		present->simple_= kore_realloc(
								present->simple_,
								(present->size+1)*sizeof(char*)
								);
		_vect=(any_t*)present->simple_;

	} else {
		(present->complex_)= kore_realloc(
								present->complex_,
								(present->size+1)*sizeof(attributes_hm_t)
								);
		_vect=(any_t*)present->complex_;
	}
	_vect[present->size]=value;
	present->size++;
	present->isSimple=_is_simple;
}

/* it store a simple string */
void attrcat(attributes_hm_t hm, char*key,char* value) {
	_attr_add(hm, key,(any_t*)value,1);
}

/* it stores a structured attributes eg: {name , surname, age } */
void attr_add(attributes_hm_t hm, char*key,attributes_hm_t value) {
	_attr_add(hm, key,(any_t)value,0);
}

int _free_attributes_vect_t(any_t nop, any_t v){
	attributes_vect_t *p;
	int idx;
	p=(attributes_vect_t*)v;
	if (!p->isSimple) {
		for (idx=0;idx<p->size;idx++) {
			attrfree((attributes_hm_t)(p->complex_[idx]));
		}
	}
	kore_free(p);
	return 0;
}

void attrfree(attributes_hm_t hm) {
	hashmap_iterate(hm,_free_attributes_vect_t,NULL);

	return hashmap_free(hm);
}

/*

#define __OK_MESSAGE "<h1>Ok</h1>"
#define __KO_MESSAGE "<h1><strong>Sorry it doesn't work test [" #NAME "]<strong></h1>"


#define RETURN_ASSERT(a,val) {\
	if ((val)==(a)) {\
		http_response(__webui_req, 200, , strlen(__OK_MESSAGE));\
	} else {\
		http_response(__webui_req, 404, __KO_MESSAGE, strlen(__KO_MESSAGE));\
	}\
}

#define WEBUI_TEST_UNIT(NAME) int ___webui___test_unit__ ## NAME (struct http_request *__webui_req)

WEBUI_TEST_UNIT(A001){
	attributes_hm_t _attributes;
	char *titolo;
    attrcat(_attributes, "title", "Dowse information panel");
    attrget(_attributes,"title",0,(any_t*)&titolo);
    RETURN_ASSERT(strcmp(titolo,"Dowse information panel"),0);
}



 WEBUI_TEST_UNIT(A002)
 {
    attributes_hm_t studente,studente2,stud,_attributes;
	char *name;
	_attributes=attrinit();

    studente =attrinit();
    attrcat(studente,"nome","Antonio");
    attrcat(studente,"cognome","Rossi");
    attrcat(studente,"indirizzo","Casa Sua");
    attr_add(_attributes,"studente",studente);

    studente2 =attrinit();
    attrcat(studente2,"nome","Mario");
    attrcat(studente2,"cognome","Bianchi");
    attrcat(studente2,"indirizzo","Un altro posto");
    attr_add(_attributes,"studente",studente2);

    attrget(_attributes,"studente",1,(any_t*)&stud);
    attrget(stud,"nome",0,(any_t*)&name);
    return (strcmp(name,"Mario")==0);
}

*/
