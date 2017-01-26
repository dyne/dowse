#ifndef __FIND_NEAREST_ASSET_H
#define __FIND_NEAREST_ASSET_H
typedef struct nearest_filename {
        char nearest[256];
        char name_to_search[256];
        int max_point;
        int (*utility_function)(char*,char*);
} nearest_filename;


#endif
/* find_nearest_asset.c */
int print_file_and_link(char *prefix, struct dirent *entry, void *data);
char filtered(char a);
int span_directory(char *dir_path, char *prefix, int (*to_execute)(char *, struct dirent *, void *), void *data);
void init_data_to_search(nearest_filename *p, char *val, int (*util_function)(char *, char *));
int example_to_use(void);
