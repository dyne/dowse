#ifndef __FIND_NEAREST_ASSET_H
#define __FIND_NEAREST_ASSET_H
typedef struct nearest_filename {
        char nearest[256];
        char name_to_search[256];
        int max_point;
        int (*utility_function)(char*,char*);
} nearest_filename;


#endif
