
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
test_results push_10(void);
test_results create_push_free(void);

int main(int argc, char** argv) {
    printf(":: Running tests ::\n");

    test_results res = create_push_free();
    print_results(res);
    res = push_10();
    print_results(res);

    printf(":: Done ::\n");
}

void print_results(test_results res) {
    printf("%s : %s ", res.name, (res.result ? passed:failed));
    if (res.result) {
        printf("\033[0;32m");//set colour to red
        printf(tick);
    } else {
        printf("\033[0;31m");//set colour to green
        printf(ecs);
    }
    printf("\033[0m");//clear colour
    printf("\n");
}

test_results push_10(void) {
    test_results res;
    res.name = "push 10 elements to the end";
    res.result = false;
    
    array_store* myVar = new_var("myVar");
    value testVal;
    testVal.val = 0;
    testVal.tag = Value;

    for (unsigned long int i = 0; i < 10; i++) {
        set(myVar, testVal, i);
        testVal.val++;
    }
    for (unsigned long int i = 0; i < 10; i++) {
        value res_val = at(myVar, i);//naming variables is hard..
        if (res_val.tag == Fail || res_val.val != i) return res;
    }

    res.result = true;
    return res;
}

test_results create_push_free(void) {
    array_store* myVar = new_var("myVar");
    value testVal;
    testVal.val = 88;
    testVal.tag = Value;
    for (int i = 0; i < 16; i++) {
        set(myVar, testVal, i);
    }
    //debug_var(myVar);
    //dump_var(myVar);
    free_var(myVar);

    ///WARN, not actually checking anything

    test_results res;
    //strcpy(res.name, "create, push and free");
    res.name = "create, push and free";
    res.result = true;
    return res;
}
