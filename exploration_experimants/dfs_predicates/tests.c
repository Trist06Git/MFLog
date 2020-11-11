
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
test_results removing_at(void);

//do tests for fail

int main(int argc, char** argv) {
    printf(":: Running tests ::\n");

    print_results(push_back_single());
    print_results(push_back_10());
    print_results(invalid_index());
    print_results(card());
    print_results(create_push_free());
    print_results(at_set());
    print_results(removing_at());

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
    printf("v_res : %i\n", v_res);
    res.result &= v_res != -1;
    res.result &= size(vec) == 9;
    for (int i = 0; i < size(vec); i++) {
        res.result &= *(int*)at(vec, i) != val2;
    }

    free_vector(vec);
    return res;
}
