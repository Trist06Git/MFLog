
#include "../mafialog_alloc.h"

typedef struct test_results {
    char* name;
    bool result;
} test_results;

char* passed = "passed";
char* tick   = "\xE2\x9C\x94";
char* failed = "failed";
char* ecs    = "\xE2\x9C\x97";

void print_results(test_results);
test_results push_back_10(void);
test_results push_front_10(void);
test_results push_back_single(void);
test_results push_front_single(void);
test_results invalid_index_back(void);
test_results invalid_index_front(void);
test_results create_push_free_back(void);
test_results create_push_free_front(void);
test_results card_front(void);
test_results card_back(void);
test_results card_both(void);

//do tests for fail

int main(int argc, char** argv) {
    printf(":: Running tests ::\n");

    print_results(create_push_free_back());
    print_results(create_push_free_front());
    print_results(push_back_single());
    print_results(push_front_single());
    print_results(push_back_10());
    print_results(push_front_10());
    print_results(invalid_index_back());
    print_results(invalid_index_front());
    print_results(card_front());
    print_results(card_back());
    print_results(card_both());

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

test_results push_back_10(void) {
    test_results res;
    res.name = "push 10 elements to the end";
    res.result = false;
    
    array_store* myVar = new_var("myVar");
    value testVal = {.val = 0, .tag = Value};

    for (int i = 0; i < 10; i++) {
        set(myVar, testVal, i);
        testVal.val++;
    }
    for (int i = 0; i < 10; i++) {
        value res_val = at(myVar, i);//naming variables is hard..
        if (res_val.tag == Fail || res_val.val != i) return res;
    }

    res.result = true;
    return res;
}

test_results push_front_10(void) {
    test_results res;
    res.name = "push 10 elements to the front";
    res.result = false;

    array_store* myVar = new_var("myVar");
    value testVal = {.val = -1, .tag = Value};

    for (int i = -1; i >= -10; i--) {
        value res_val = set(myVar, testVal, i);
        if (res_val.tag == Fail) return res;
        testVal.val--;
    }
    for (int i = -1; i >= -10; i--) {
        value res_val = at(myVar, i);
        if (res_val.tag == Fail || res_val.val != i) return res;
    }

    res.result = true;
    return res;
}

test_results push_back_single(void) {
    test_results res;
    res.name = "push 1 element to the back";
    res.result = false;

    array_store* myVar = new_var("myVar");
    value testVal = {.val = 7, .tag = Value};

    value res_val = set(myVar, testVal, 0);
    res.result = res_val.tag == Fail ? false : true;

    return res;
}

test_results push_front_single(void) {
    test_results res;
    res.name = "push 1 element to the front";
    res.result = false;

    array_store* myVar = new_var("myVar");
    value testVal = {.val = 7, .tag = Value};

    value res_val = set(myVar, testVal, -1);
    res.result = res_val.tag == Fail ? false : true;

    return res;
}

test_results invalid_index_back(void) {
    test_results res;
    res.name = "invalid index forward";
    res.result = false;

    array_store* myVar = new_var("myVar");
    value testVal = {.val = -7, .tag = Value};

    set(myVar, testVal, 0);
    value res_val = set(myVar, testVal, 10);
    res.result = res_val.tag == Fail ? true : false;//we want it to fail

    return res;
}

test_results invalid_index_front(void) {
    test_results res;
    res.name = "invalid index backward";
    res.result = false;

    array_store* myVar = new_var("myVar");
    value testVal = {.val = -7, .tag = Value};

    set(myVar, testVal, -1);
    value res_val = set(myVar, testVal, -10);
    res.result = res_val.tag == Fail ? true : false;//we want it to fail

    return res;
}

test_results card_front(void) {
    test_results res;
    res.name = "cardinality of front only array";

    array_store* myVar = new_var("myVar");
    value testVal = {.val = 7, .tag = Value};

    int target = 10;
    for (int i = 0; i < target; i++) {
        set(myVar, testVal, i);
    }
    
    value cardinality = card(myVar);
    res.result = cardinality.tag != Fail && cardinality.val == target;
    return res;
}

test_results card_back(void) {
    test_results res;
    res.name = "cardinality of back only array";

    array_store* myVar = new_var("myVar");
    value testVal = {.val = 7, .tag = Value};

    int target = 10;
    for (int i = 0; i > -target; i--) {
        set(myVar, testVal, i);
    }

    value cardinality = card(myVar);
    res.result = cardinality.tag != Fail && cardinality.val == target;
    return res;
}

test_results card_both(void) {
    test_results res;
    res.name = "cardinality of both front and back array";

    array_store* myVar = new_var("myVar");
    value testVal = {.val = 7, .tag = Value};

    int target = 10;
    for (int i = 1; i >= -target; i-- ) {
        set(myVar, testVal, i);
    }
    for (int i = 0; i < target; i++) {
        set(myVar, testVal, i);
    }

    value cardinality = card(myVar);
    res.result = cardinality.tag != Fail && cardinality.val == target*2;
    return res;
}

test_results create_push_free_back(void) {
    array_store* myVar = new_var("myVar");
    value testVal = {.val = 88, .tag = Value};

    for (int i = 0; i < 16; i++) {
        set(myVar, testVal, i);
    }
    free_var(myVar);

    ///WARN, not actually checking anything
    //just relying on segfault

    test_results res;
    res.name = "create, push and free back";
    res.result = true;
    return res;
}

test_results create_push_free_front(void) {
    array_store* myVar = new_var("myVar");
    value testVal = {.val = -88, .tag = Value};

    for (int i = -1; i >= -16; i--) {
        set(myVar, testVal, i);
    }
    free_var(myVar);

    ///WARN, not actually checking anything
    //just relying on segfault

    test_results res;
    res.name = "create, push and free front";
    res.result = true;
    return res;
}

/*
ieee 754

*/

