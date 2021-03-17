
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "generic_map.h"

typedef struct test_results {
    char* name;
    bool result;
} test_results;

char* passed = "passed";
char* tick   = "\xE2\x9C\x94";
char* failed = "failed";
char* ecs    = "\xE2\x9C\x97";

void print_results(test_results);
test_results test_test(void);
test_results create_free(void);
test_results add_els(void);
test_results get_at_index(void);
test_results get_by_key(void);
test_results get_by_val(void);
test_results remove_key(void);
test_results contains_key(void);
test_results contains_val(void);
test_results custom_comparators(void);
test_results custom_comparators2(void);
test_results string_comparator(void);
test_results builtin_string_comparator(void);
test_results byte_comp_struct(void);

int main(int argc, char** argv) {
    printf(":: Running tests ::\n");

    print_results(test_test());
    print_results(create_free());
    print_results(add_els());
    print_results(get_at_index());
    print_results(get_by_key());
    print_results(get_by_val());
    print_results(remove_key());
    print_results(contains_key());
    print_results(contains_val());
    print_results(custom_comparators());
    print_results(custom_comparators2());
    print_results(string_comparator());
    print_results(builtin_string_comparator());
    print_results(byte_comp_struct());

    printf(":: Done ::\n");
}

void print_results(test_results res) {
    printf("%-60s %s : ", res.name, (res.result ? passed:failed));
    if (res.result) {
        printf("\033[0;32m");//set colour to red
        printf("%s", tick);
    } else {
        printf("\033[0;31m");//set colour to green
        printf("%s", ecs);
    }
    printf("\033[0m");//clear colour
    printf("\n");
}

test_results test_test(void) {
    test_results res;
    res.name = "test";
    res.result = false;

    int something = 1;
    if (something) res.result = true;
    
    return res;
}

test_results create_free(void) {
    test_results res;
    res.name = "create, free";
    res.result = false;

    map* mp = new_map(sizeof(int), sizeof(char));
    free_map(mp);

    //relying on segfault..
    res.result = true;
    
    return res;
}

test_results add_els(void) {
    test_results res;
    res.name = "add elements, map_size";
    res.result = false;

    map* mp = new_map(sizeof(int), sizeof(char));
    int val1 = 1;
    char val2 = '1';
    map_add(mp, &val1, &val2);
    
    res.result = map_size(mp) == 1;
    
    free_map(mp);
    return res;
}

test_results get_at_index(void) {
    test_results res;
    res.name = "get at index";
    res.result = true;

    map* mp = new_map(sizeof(int), sizeof(char));
    int i;
    char c;
    for (i = 0, c = '0'; i < 4; i++, c++) {
        map_add(mp, &i, &c);
    }
    for (i = 0, c = '0'; i < map_size(mp); i++, c++) {
        res.result &= *(char*)((pair*)map_at_index(mp, i))->snd == c;
    }
    
    free_map(mp);
    return res;
}

test_results get_by_key(void) {
    test_results res;
    res.name = "get by key";
    res.result = true;

    map* mp = new_map(sizeof(int), sizeof(char));
    int i;
    char c;
    for (i = 0, c = '0'; i < 4; i++, c++) {
        map_add(mp, &i, &c);
    }
    for (i = 0, c = '0'; i < map_size(mp); i++, c++) {
        char* val = snd(mp, &i);
        res.result &= val != NULL && *val == c;
    }
    //invalid key
    int invalid = 99;
    char* val = snd(mp, &invalid);
    res.result &= val == NULL;
    
    free_map(mp);
    return res;
}

test_results get_by_val(void) {
    test_results res;
    res.name = "get by val";
    res.result = true;

    map* mp = new_map(sizeof(int), sizeof(char));
    int i;
    char c;
    for (i = 0, c = '0'; i < 4; i++, c++) {
        map_add(mp, &i, &c);
    }
    for (i = 0, c = '0'; i < map_size(mp); i++, c++) {
        int* key = fst(mp, &c);
        res.result &= key != NULL && *key == i;
    }
    //invalid val
    char invalid = '9';
    int* key = fst(mp, &invalid);
    res.result &= key == NULL;
    
    free_map(mp);
    return res;
}

test_results remove_key(void) {
    test_results res;
    res.name = "remove key";
    res.result = true;

    map* mp = new_map(sizeof(int), sizeof(char));
    int  i =  0 ;
    char c = '0';
    map_add(mp, &i, &c); i++; c++;
    map_add(mp, &i, &c); i++; c++;
    map_add(mp, &i, &c); i++; c++;
    map_add(mp, &i, &c); i++; c++;

    int remove_me = 1;
    res.result &= map_remove(mp, &remove_me) == 0;
    

    int dummy = 77;
    int  i_zero =  0 , i_one =  1 , i_two =  2 , i_three =  3 ;
    char c_zero = '0', c_one = '1', c_two = '2', c_three = '3';
    res.result &= *(char*)snd(mp, &i_zero)  == c_zero;
    res.result &= *(char*)snd(mp, &i_two)   == c_two;
    res.result &= *(char*)snd(mp, &i_three) == c_three;
    res.result &= snd(mp, &i_one) == NULL;
    res.result &= snd(mp, &dummy) == NULL;
    
    free_map(mp);
    return res;
}

test_results contains_key(void) {
    test_results res;
    res.name = "contains key";
    res.result = true;

    map* mp = new_map(sizeof(int), sizeof(char));
    int i;
    char c;
    for (i = 0, c = '0'; i < 4; i++, c++) {
        map_add(mp, &i, &c);
    }
    for (i = 0; i < map_size(mp); i++) {
        res.result &= map_contains_key(mp, &i);
    }

    //invalid
    int dummy = 7;
    res.result &= map_contains_key(mp, &dummy) == false;
    
    free_map(mp);
    return res;
}

test_results contains_val(void) {
    test_results res;
    res.name = "contains val";
    res.result = true;

    map* mp = new_map(sizeof(int), sizeof(char));
    int i;
    char c;
    for (i = 0, c = '0'; i < 4; i++, c++) {
        map_add(mp, &i, &c);
    }
    for (i = 0, c = '0'; i < map_size(mp); i++, c++) {
        res.result &= map_contains_val(mp, &c);
    }

    //invalid
    char dummy = '7';
    res.result &= map_contains_val(mp, &dummy) == false;
    
    free_map(mp);
    return res;
}

bool my_int_compare(const void* e1, const void* e2, int size) {
    //size not needed
    const int i = *(int*)e1;
    const int j = *(int*)e2;
    return i == j;
}

test_results custom_comparators(void) {
    test_results res;
    res.name = "custom comparator";
    res.result = true;

    map* mp = new_map(sizeof(int), sizeof(int));
    int i;
    for (i = 0; i < 4; i++) {
        map_add(mp, &i, &i);
    }
    set_snd_comparator(mp, my_int_compare);
    for (i = 0; i < map_size(mp); i++) {
        res.result &= map_contains_val(mp, &i);
    }

    //invalid
    int dummy = 7;
    res.result &= map_contains_val(mp, &dummy) == false;
    
    free_map(mp);
    return res;
}

bool my_bad_int_compare(const void* e1, const void* e2, int size) {
    //size not needed
    const int i = *(int*)e1;
    const int j = *(int*)e2;
    return i == j + 1;
}

test_results custom_comparators2(void) {
    test_results res;
    res.name = "custom comparator 2";
    res.result = true;

    map* mp = new_map(sizeof(int), sizeof(int));
    int i;
    for (i = 0; i < 4; i+=10) {
        map_add(mp, &i, &i);
    }
    set_snd_comparator(mp, my_bad_int_compare);
    for (i = 0; i < map_size(mp); i+=10) {
        res.result &= !map_contains_val(mp, &i);
    }

    //invalid
    int dummy = 7;
    res.result &= map_contains_val(mp, &dummy) == false;
    
    free_map(mp);
    return res;
}

bool my_str_comp(const void* el1, const void* el2, int size) {
    //size not needed..
    const char* str1 = *(char**)el1;
    const char* str2 = *(char**)el2;
    return strcmp(str1, str2) == 0;
}

test_results string_comparator(void) {
    test_results res;
    res.name = "string comparator";
    res.result = true;

    map* mp = new_map(sizeof(int), sizeof(char*));
    set_snd_comparator(mp, my_str_comp);
    int zero = 0;
    int one  = 1;
    int two  = 2;
    char* str0 = "zero";
    char* str1 = "one" ;
    char* str2 = "two" ;
    map_add(mp, &zero, &str0);
    map_add(mp, &one , &str1);
    map_add(mp, &two , &str2);
    
    res.result &= map_contains_val(mp, &str0);
    res.result &= map_contains_val(mp, &str1);
    res.result &= map_contains_val(mp, &str2);

    char* different_pointer = "two";
    res.result &= map_contains_val(mp, &different_pointer);

    //invalid
    char* dummy = "Nuuuu :-0";
    res.result &= map_contains_val(mp, &dummy) == false;
    
    free_map(mp);
    return res;
}

test_results builtin_string_comparator(void) {
    test_results res;
    res.name = "builtin string comparator";
    res.result = true;

    map* mp = new_map(sizeof(int), sizeof(char*));
    set_snd_comparator(mp, string_compare);
    int zero = 0;
    int one  = 1;
    int two  = 2;
    char* str0 = "zero";
    char* str1 = "one" ;
    char* str2 = "two" ;
    map_add(mp, &zero, &str0);
    map_add(mp, &one , &str1);
    map_add(mp, &two , &str2);
    
    res.result &= map_contains_val(mp, &str0);
    res.result &= map_contains_val(mp, &str1);
    res.result &= map_contains_val(mp, &str2);

    char* different_pointer = "two";
    res.result &= map_contains_val(mp, &different_pointer);

    //invalid
    char* dummy = "Nuuuu :-0";
    res.result &= map_contains_val(mp, &dummy) == false;
    
    free_map(mp);
    return res;
}

test_results byte_comp_struct(void) {
    test_results res;
    res.name = "byte comparator on struct";
    res.result = true;

    typedef struct {
        uint8_t a;
        uint8_t b;
    } test_struct;

    map* mp = new_map(sizeof(test_struct), sizeof(int));
    set_fst_comparator(mp, byte_compare);
    set_snd_comparator(mp, byte_compare);
    
    test_struct one = {.a = 1, .b = 'a'};
    int i_one = 1;
    map_add(mp, &one , &i_one);
    
    res.result &= map_contains_key(mp, &one);

    test_struct two = {.a = 1, .b = 'b'};
    res.result &= map_contains_key(mp, &two) == false;

    free_map(mp);
    return res;
}
