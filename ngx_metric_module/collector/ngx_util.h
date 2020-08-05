#ifndef NGX_UTIL_H_
#define NGX_UTIL_H_

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include "global.h"

typedef struct
{
    char **data;
    size_t len;
} node_array_t;

// replace char by char
int replace_char(char *str, char find, char replace);

// remove space
int trim(char *str);

// replace string by another string
char *replace_str(char *string, const char *substr, const char *replacement);

// remove char from string
int remove_char(char *str, char c);

// insert a character in to string at pos
char *insert_char(const char *source, const char add, int pos, int headOnly);

// read file
char *read_proc_file(char *filename, ngx_http_request_t *r);

// check start_with in String
int start_with(const char *str, const char *prefix);
int end_with(const char *str, const char *suffix);

// split string with delim
node_array_t str_split(char *a_str, const char a_delim);

void *remove_all_characters(char *inputStr);

#endif /* NGX_UTIL_H_ */