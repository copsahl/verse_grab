/* 
    Increase arg validation for more coherent usage
    https://github.com/json-c/json-c
*/

#include "json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <error.h>

#define TRANSLATION "kjv"

void validate_args(int argc, char **argv);
size_t retrieve_data(char *buffer, size_t item_size, size_t num_of_items, void *garbage);
static void parse_data(json_value *value, int depth);
static void print_depth_shift(int depth);
static void process_object(json_value* value, int depth);
static void process_array(json_value* value, int depth);
static void process_value(json_value* value, int depth);

int main(int argc, char **argv){

    CURL *curl;
    CURLUcode res;
    const char *base_url = "https://bible-api.com";
    char full_url[100];

    if(argc < 2){
        printf("Usage; %s <book> <chapter> [<verse>, <verse>-<verse>]\n", argv[0]);
        printf("%s genesis 1 27\n", argv[0]);
        printf("%s 1_john 1 1-5\n", argv[0]);
        exit(0);
    } 

    validate_args(argc, argv);

    snprintf(full_url, 100, "%s/%s+%s:%s?translation=%s", base_url, argv[1], argv[2], argv[3], TRANSLATION);

    curl = curl_easy_init();
    if(curl){
        curl_easy_setopt(curl, CURLOPT_URL, full_url); 
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, retrieve_data);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
    }else{
        perror("curl_easy_init");        
        exit(0);
    }
    
    curl_easy_cleanup(curl);

	return 0;
}

void validate_args(int argc, char **argv){

    /* Only validation being done is switching '_' to '+' */
    int i;
    for(i = 0; i < strlen(argv[1]); i++){
        if(argv[1][i] == '_')
            argv[1][i] = '+';
    }

    return;
}

size_t retrieve_data(char *buffer, size_t item_size, size_t num_of_items, void *garbage){
    size_t bytes = item_size * num_of_items; 
    json_char *json;
    json_value *value;

    json = (json_char *)buffer;

    /* json_parse() is limited in memory allocation */
    value = json_parse(json, bytes);

    if(value == NULL){
        fprintf(stderr, "Unable to parse data\n");
        exit(0);
    }

    parse_data(value, 0);

    json_value_free(value);
    return bytes;
}

static void parse_data(json_value *value, int depth){
    int length, x;
    if(value == NULL)
        return;

    length = value->u.object.length;
    for(x = 0; x < length; x++){
        if(strncmp("text", value->u.object.values[x].name, 4) == 0){
            process_value(value->u.object.values[x].value, depth+1);
        }
        if(strncmp("reference", value->u.object.values[x].name, 9) == 0){
            process_value(value->u.object.values[x].value, depth+1);
        }
    }
}

static void print_depth_shift(int depth){
        int j;
        for (j=0; j < depth; j++) {
                printf(" ");
        }
}


static void process_object(json_value* value, int depth){
        int length, x;
        if (value == NULL) {
                return;
        }
        length = value->u.object.length;
        for (x = 0; x < length; x++) {
                print_depth_shift(depth);
                process_value(value->u.object.values[x].value, depth+1);
        }
}

static void process_array(json_value* value, int depth){
        int length, x;
        if (value == NULL) {
                return;
        }
        length = value->u.array.length;
        for (x = 0; x < length; x++) {
                process_value(value->u.array.values[x], depth);
        }
}

static void process_value(json_value* value, int depth){
        if (value == NULL) {
                return;
        }
        if (value->type != json_object) {
                print_depth_shift(depth);
        }
        switch (value->type) {
                case json_none:
                        printf("none\n");
                        break;
                case json_null:
                        printf("null\n");
                        break;
                case json_object:
                        process_object(value, depth+1);
                        break;
                case json_array:
                        process_array(value, depth+1);
                        break;
                case json_integer:
                        printf("%10ld\n", (long)value->u.integer);
                        break;
                case json_double:
                        printf("%f\n", value->u.dbl);
                        break;
                case json_string:
                        printf("%s\n", value->u.string.ptr);
                        break;
                case json_boolean:
                        printf("%d\n", value->u.boolean);
                        break;
        }
}
