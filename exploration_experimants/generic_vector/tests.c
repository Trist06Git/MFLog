
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "generic_vector.h"

typedef struct test_results {
    char* name;
    bool result;
} test_results;

char* passed = "passed";
char* tick   = "\xE2\x9C\x94";
char* failed = "failed";
char* ecs    = "\xE2\x9C\x97";

void print_results(test_results);
test_results push_back_single(void);
test_results push_back_10(void);
test_results invalid_index(void);
test_results card(void);
test_results create_push_free(void);
test_results at_set(void);
test_results inserting_single_at(void);
test_results inserting_at_empty(void);
test_results inserting_at_start(void);
test_results inserting_multiple_at(void);
test_results removing_at(void);
test_results remove_correct_i(void);
test_results push_enum(void);
test_results element_overflow(void);
test_results contains_char(void);
test_results contains_int(void);
test_results contains_struct(void);
test_results vec_contains_string(void);
test_results append_vectors(void);

void dump_int_vector(vector*);

//do tests for fail

int main(int argc, char** argv) {
    printf(":: Running tests ::\n");

    print_results(push_back_single());
    print_results(push_back_10());
    print_results(invalid_index());
    print_results(card());
    print_results(create_push_free());
    print_results(at_set());
    print_results(inserting_single_at());
    print_results(inserting_at_empty());
    print_results(inserting_at_start());
    print_results(inserting_multiple_at());
    print_results(removing_at());
    print_results(remove_correct_i());
    print_results(push_enum());
    print_results(element_overflow());
    print_results(contains_char());
    print_results(contains_int());
    print_results(contains_struct());
    print_results(vec_contains_string());
    print_results(append_vectors());

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

test_results push_back_single(void) {
    test_results res;
    res.name = "push back single";
    res.result = false;
    
    vector* vec = new_vector(0, sizeof(int));
    int val = 7;
    push_back(vec, &val);
    if (*(int*)at(vec, 0) == val) res.result = true;

    free_vector(vec);
    return res;
}

test_results push_back_10(void) {
    test_results res;
    res.name = "push back 10";
    res.result = true;

    vector* vec = new_vector(0, sizeof(int));
    for (int i = 0; i < 10; i++) push_back(vec, &i);
    for (int i = 0; i < 10; i++) res.result &= *(int*)at(vec, i) == i;

    free_vector(vec);
    return res;
}

test_results invalid_index(void) {
    test_results res;
    res.name = "invalid index";
    res.result = true;

    vector* vec = new_vector(0, sizeof(int));
    int val = 11;
    for (int i = 0; i < 3; i++) push_back(vec, &val);
    res.result &= at(vec, 3) == (void*)-1;
    res.result &= at(vec, 2) != (void*)-1;

    free_vector(vec);
    return res;
}

test_results card(void) {
    test_results res;
    res.name = "cardinality/size";
    res.result = true;

    vector* vec = new_vector(0, sizeof(int));
    int target = 107;
    for (int i = 0; i < target; i++) push_back(vec, &i);
    res.result &= size(vec) == target;

    free_vector(vec);
    return res;
}

test_results create_push_free(void) {
    test_results res;
    res.name = "create, push, free";
    res.result = true;

    vector* vec = new_vector(0, sizeof(int));
    int val = 11;
    for (int i = 0; i < 3; i++) push_back(vec, &val);
    free_vector(vec);

    //not actually checking...
    return res;
}

test_results at_set(void) {
    test_results res;
    res.name = "at_set";
    res.result = true;

    vector* vec = new_vector(0, sizeof(int));
    int val1 = 11;
    int val2 = 22;
    for (int i = 0; i < 4; i++) push_back(vec, &val1);
    *(int*)at(vec, 2) = val2;
    res.result &= *(int*)at(vec, 2) == val2;

    free_vector(vec);
    return res;
}

test_results inserting_at_start(void) {
    test_results res;
    res.name = "inserting at start";
    res.result = true;

    vector* vec = new_vector(0, sizeof(int));
    int val1 = 3;
    int val2 = 7;
    push_back(vec, &val1);
    push_back(vec, &val1);
    insert_at(vec, 0, &val2);
    res.result &= *(int*)at(vec, 0) == val2;
    res.result &= *(int*)at(vec, 1) == val1;
    res.result &= *(int*)at(vec, 2) == val1;

    return res;
}

test_results inserting_at_empty(void) {
    test_results res;
    res.name = "inserting at empty";
    res.result = true;

    vector* vec = new_vector(0, sizeof(int));
    int val = 17;
    insert_at(vec, 0, &val);
    res.result &= *(int*)at(vec, 0) == val;

    return res;
}

test_results inserting_single_at(void) {
    test_results res;
    res.name = "inserting single at";
    res.result = true;

    vector* vec = new_vector(0, sizeof(int));
    int val1 = 13;
    int val2 = 11;
    push_back(vec, &val1);
    push_back(vec, &val1);
    insert_at(vec, 1, &val2);
    res.result &= *(int*)at(vec, 0) == val1;
    res.result &= *(int*)at(vec, 1) == val2;
    res.result &= *(int*)at(vec, 2) == val1;

    return res;
}

test_results inserting_multiple_at(void) {
    test_results res;
    res.name = "inserting multiple at";
    res.result = true;

    vector* vec = new_vector(0, sizeof(int));
    int val1 = -7;
    int val2 = 11;
    for (int i = 0; i < 4; i++) {
        push_back(vec, &val1);
    }
    insert_at(vec, 0, &val2);////2 7 2 7 2 7 2 7 
    insert_at(vec, 2, &val2);
    insert_at(vec, 4, &val2);
    insert_at(vec, 6, &val2);
    for (int i = 0; i < 8; i++) {
        /*if (i % 2 == 0) {
            res.result &= *(int*)at(vec, i) == val2;
        } else {
            res.result &= *(int*)at(vec, i) == val1;
        }*/
        res.result &= *(int*)at(vec, i) == (i%2==0 ? val2 : val1);
    }
    return res;
}

test_results removing_at(void) {
    test_results res;
    res.name = "removing at";
    res.result = true;

    vector* vec = new_vector(0, sizeof(int));
    int val1 = 13;
    int val2 = -7;
    for (int i = 0; i < 10; i++) {
        push_back(vec, &val1);
    }
    *(int*)at(vec, 5) = val2;
    int v_res = remove_at(vec, 5);
    res.result &= v_res != -1;
    res.result &= size(vec) == 9;
    for (int i = 0; i < size(vec); i++) {
        res.result &= *(int*)at(vec, i) != val2;
    }

    free_vector(vec);
    return res;
}

test_results remove_correct_i(void) {
    test_results res;
    res.name = "remove correct index";
    res.result = true;

    vector* vec = new_vector(0, sizeof(int));
    for (int i = 0; i < 4; i++) {
        push_back(vec, &i);
    }
    remove_at(vec, 1);
    res.result &= *(int*)at(vec, 0) == 0;
    res.result &= *(int*)at(vec, 1) == 2;
    res.result &= *(int*)at(vec, 2) == 3;

    return res;
}

test_results push_enum(void) {
    test_results res;
    res.name = "push enum";
    res.result = true;

    vector* vec = new_vector(0, sizeof(int));

    enum e_test {A, B, C};
    enum e_test* myEnum = malloc(sizeof(enum e_test));
    *myEnum = A;
    push_back(vec, myEnum);
    res.result &= *(enum e_test*)at(vec, 0) == A;

    free_vector(vec);
    return res;
}

test_results element_overflow(void) {
    test_results res;
    res.name = "element overflow. WARN probably not a good test";
    res.result = true;

    int val = 11;
    vector* vec = new_vector(0, sizeof(int));
    int vals[10];
    for (int i = 0; i < 10; i++) vals[i] = i;
    insert_at(vec, 0, &val);
    insert_at(vec, 0, &val);
    insert_at(vec, 0, &val);

    for (int i = 0; i < 10; i++) {
        res.result &= vals[i] == i;
    }
    
    free_vector(vec);
    return res;
}

test_results contains_char(void) {
    test_results res;
    res.name = "contains char";
    res.result = true;

    vector* vec = new_vector(0, sizeof(char));
    for (char c = 'p'; c < 'v'; c++) {
        push_back(vec, &c);
    }

    //contains: pqrstu
    for (char c = 'p'; c < 'v'; c++) {
        res.result &= contains(vec, &c);
    }
    
    char a = 'a';
    res.result &= !contains(vec, &a);
    char z = 'z';
    res.result &= !contains(vec, &z);

    free_vector(vec);
    return res;
}

test_results contains_int(void) {
    test_results res;
    res.name = "contains int";
    res.result = true;

    vector* vec = new_vector(0, sizeof(int));
    int a = 0xff00ff00;
    int b = 0x00ff00ff;
    int c = 0x0000ffff;
    push_back(vec, &a);
    push_back(vec, &b);
    push_back(vec, &c);
    res.result &= contains(vec, &a);
    res.result &= contains(vec, &b);
    res.result &= contains(vec, &c);

    int d = 0xffff0000;
    int e = 0xff00ffff;
    int f = 0x00ffff00;
    res.result &= !contains(vec, &d);
    res.result &= !contains(vec, &e);
    res.result &= !contains(vec, &f);

    free_vector(vec);
    return res;
}

test_results contains_struct(void) {
    test_results res;
    res.name = "contains struct";
    res.result = true;

    typedef struct s s;
    struct s {
        int i;
        char c;
        bool b;
    };
    vector* vec = new_vector(0, sizeof(s));

    int i = 0;
    char c = 'a';
    bool b = false;
    for (; i < 4; i++, c++, b = !b) {
        s st = {i, c, b};
        push_back(vec, &st);
    }
    i = 0;
    c = 'a';
    b = false;
    for (; i < 4; i++, c++, b = !b) {
        s st = {i, c, b};
        res.result &= contains(vec, &st);
    }
    i = 10;
    c = 'j';
    b = true;
    for (; i < 14; i++, c++, b = !b) {
        s st = {i, c, b};
        res.result &= !contains(vec, &st);
    }

    free_vector(vec);
    return res;
}

test_results vec_contains_string(void) {
    test_results res;
    res.name = "contains string";
    res.result = true;

    vector* vec = new_vector(0, sizeof(char*));

    char* a = "abcdefg";
    char* b = "R'lyeh wgah'nagl fhtagn";
    char* c = "hello world";
    push_back(vec, &a);
    push_back(vec, &b);
    push_back(vec, &c);
    res.result &=  contains_string(vec, a);
    res.result &=  contains_string(vec, b);
    res.result &=  contains_string(vec, c);
    res.result &= !contains_string(vec, "a");
    res.result &= !contains_string(vec, "Ph'nglui mglw'nafh");
    res.result &= !contains_string(vec, "goodbye world");

    free_vector(vec);
    return res;
}

test_results append_vectors(void) {
    test_results res;
    res.name = "append vectors";
    res.result = true;

    vector* vec1 = new_vector(0, sizeof(int));
    vector* vec2 = new_vector(0, sizeof(int));

    for (int i = 0; i < 5; i++) {
        push_back(vec1, &i);
        int j = 4-i;
        push_back(vec2, &j);
    }
    int vec1_orig_size = size(vec1);
    int vec2_orig_size = size(vec2);

    append_vector(vec1, vec2);
    res.result &= size(vec1) == vec1_orig_size+vec2_orig_size;
    res.result &= size(vec2) == vec2_orig_size;

    for (int i = 0; i < 5; i++) {
        int* e = at(vec1, i);
        res.result &= *e == i;
    }
    for (int i = 0; i < 5; i++) {
        int* e = at(vec1, vec1_orig_size+i);
        int j = 4-i;
        res.result &= *e == j;
    }

    free_vector(vec1);
    free_vector(vec2);
    return res;
}

void dump_int_vector(vector* vec) {
    printf("dumping int vector :: count : %i :: alloced : %i\n", vec->count, vec->allocated);
    for (int i = 0; i < size(vec); i++) {
        printf("%i : %i\n", i, *(int*)at(vec, i));
    }
}
