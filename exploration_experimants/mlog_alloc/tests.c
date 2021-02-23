
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "mlog_array.h"

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
test_results push_front_single(void);
test_results at_front_5(void);
test_results push_back_front_10(void);
test_results remove_at_mid_back(void);
test_results remove_at_mid_front(void);
test_results remove_all_back_front(void);
test_results push_back_wrapper(void);
test_results push_front_wrapper(void);
test_results remove_back_wrapper(void);
test_results remove_front_wrapper(void);
test_results pop_back_wrapper(void);
test_results pop_front_wrapper(void);

//do tests for fail

int main(int argc, char** argv) {
    printf(":: Running tests ::\n");

    print_results(push_back_single());
    print_results(push_front_single());
    print_results(at_front_5());
    print_results(push_back_front_10());
    print_results(remove_at_mid_back());
    print_results(remove_at_mid_front());
    print_results(remove_all_back_front());
    print_results(push_back_wrapper());
    print_results(push_front_wrapper());
    print_results(remove_back_wrapper());
    print_results(remove_front_wrapper());
    print_results(pop_back_wrapper());
    print_results(pop_front_wrapper());

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
    res.result = true;
    
    mf_array* arr = new_mfarray(sizeof(int));
    int one = 1;
    res.result &= mfa_set(arr, 0, &one);
    res.result &= mfa_card(arr) == 1;
    int* one2 = mfa_at(arr, 0);
    res.result &= *one2 == one;
    
    free_mfarray(arr);
    return res;
}

test_results push_front_single(void) {
    test_results res;
    res.name = "push front single";
    res.result = true;
    
    mf_array* arr = new_mfarray(sizeof(int));
    int one = 1;
    res.result &= mfa_set(arr, -1, &one);
    res.result &= mfa_card(arr) == 1;
    int* one2 = mfa_at(arr, 0);
    res.result &= *one2 == one;
    
    free_mfarray(arr);
    return res;
}

test_results at_front_5(void) {
    test_results res;
    res.name = "at front 5";
    res.result = true;
    
    mf_array* arr = new_mfarray(sizeof(int));
    for (int i = 4; i >= 0; i--) {
        res.result &= mfa_set(arr, -1, &i);
    }
    for (int i = 0; i < mfa_card(arr); i++) {
        int* val = mfa_at(arr, i);
        if (val == NULL) {
            res.result = false;
            return res;
        }
        res.result &= *val == i;
    }
    
    free_mfarray(arr);
    return res;
}

test_results push_back_front_10(void) {
    test_results res;
    res.name = "push back and front 10";
    res.result = true;
    
    mf_array* arr = new_mfarray(sizeof(int));
    for (int i = 0; i < 5; i++) {
        int val = i+5;
        res.result &= mfa_set(arr, i, &val);
    }
    for (int i = 4; i >= 0; i--) {
        res.result &= mfa_set(arr, -1, &i);
    }
    for (int i = 0; i < mfa_card(arr); i++) {
        int* val = mfa_at(arr, i);
        res.result &= *val == i;
    }
    
    free_mfarray(arr);
    return res;
}

test_results remove_at_mid_back(void) {
    test_results res;
    res.name = "remove at mid back";
    res.result = true;
    
    mf_array* arr = new_mfarray(sizeof(int));
    for (int i = 0; i < 10; i++) {
        if(mfa_set(arr, i, &i) == false) {
            res.result = false;
            return res;
        }
    }
    for (int i = 4; i >= 0; i--) {
        if (mfa_remove_at(arr, i*2) == false) {
            res.result = false;
            return res;
        }
    }
    for (int i = 0; i < mfa_card(arr); i++) {
        int* val = mfa_at(arr, i);
        res.result &= *val % 2 != 0;
    }

    free_mfarray(arr);
    return res;
}

test_results remove_at_mid_front(void) {
    test_results res;
    res.name = "remove at mid front";
    res.result = true;
    
    mf_array* arr = new_mfarray(sizeof(int));
    for (int i = 0; i < 10; i++) {
        if(mfa_set(arr, -1, &i) == false) {
            res.result = false;
            return res;
        }
    }
    for (int i = 4; i >= 0; i--) {
        if (mfa_remove_at(arr, mfa_card(arr)-1-i*2) == false) {
            res.result = false;
            return res;
        }
    }
    for (int i = 0; i < mfa_card(arr); i++) {
        int* val = mfa_at(arr, i);
        res.result &= *val % 2 != 0;
    }

    free_mfarray(arr);
    return res;
}

test_results remove_all_back_front(void) {
    test_results res;
    res.name = "remove all back front";
    res.result = true;
    
    mf_array* arr = new_mfarray(sizeof(int));
    for (int i = 0; i < 5; i++) {
        mfa_set(arr, -1, &i);
        mfa_set(arr, mfa_card(arr), &i);
    }
    while (mfa_remove_at(arr, 0));
    res.result &= mfa_card(arr) == 0;

    free_mfarray(arr);
    return res;
}

test_results push_back_wrapper(void) {
    test_results res;
    res.name = "push back wrapper";
    res.result = true;
    
    mf_array* arr = new_mfarray(sizeof(int));
    for (int i = 0; i < 10; i++) {
        if (mfa_push_back(arr, &i) == false) {
            res.result = false;
            return res;
        } 
    }
    for (int i = 0; i < mfa_card(arr); i++) {
        int* val = mfa_at(arr, i);
        if (val == NULL) {
            res.result = false;
            return res;
        }
        res.result &= *val == i;
    }

    free_mfarray(arr);
    return res;
}

test_results push_front_wrapper(void) {
    test_results res;
    res.name = "push front wrapper";
    res.result = true;
    
    mf_array* arr = new_mfarray(sizeof(int));
    for (int i = 9; i >= 0; i--) {
        if (mfa_push_front(arr, &i) == false) {
            res.result = false;
            return res;
        } 
    }
    for (int i = 0; i < mfa_card(arr); i++) {
        int* val = mfa_at(arr, i);
        if (val == NULL) {
            res.result = false;
            return res;
        }
        res.result &= *val == i;
    }

    free_mfarray(arr);
    return res;
}

test_results remove_back_wrapper(void) {
    test_results res;
    res.name = "remove back wrapper";
    res.result = true;
    
    mf_array* arr = new_mfarray(sizeof(int));
    for (int i = 0; i < 10; i++) {
        if (mfa_push_back(arr, &i) == false) {
            res.result = false;
            return res;
        } 
    }
    mfa_remove_back(arr);
    for (int i = 0; i < mfa_card(arr); i++) {
        int* val = mfa_at(arr, i);
        if (val == NULL) {
            res.result = false;
            return res;
        }
        res.result &= *val == i;
    }
    res.result &= mfa_card(arr) == 9;

    free_mfarray(arr);
    return res;
}

test_results remove_front_wrapper(void) {
    test_results res;
    res.name = "remove front wrapper";
    res.result = true;
    
    mf_array* arr = new_mfarray(sizeof(int));
    for (int i = 9; i >= 0; i--) {
        if (mfa_push_front(arr, &i) == false) {
            res.result = false;
            return res;
        } 
    }
    mfa_remove_front(arr);
    for (int i = 0; i < mfa_card(arr); i++) {
        int* val = mfa_at(arr, i);
        if (val == NULL) {
            res.result = false;
            return res;
        }
        res.result &= *val == i+1;
    }
    res.result &= mfa_card(arr) == 9;

    free_mfarray(arr);
    return res;
}

test_results pop_back_wrapper(void) {
    test_results res;
    res.name = "pop back wrapper";
    res.result = true;
    
    mf_array* arr = new_mfarray(sizeof(int));
    for (int i = 0; i < 10; i++) {
        if (mfa_push_back(arr, &i) == false) {
            res.result = false;
            return res;
        } 
    }
    int* item = mfa_pop_back(arr);
    if (item == NULL) {
        res.result = false;
        return res;
    }
    res.result &= *item == 9;
    for (int i = 0; i < mfa_card(arr); i++) {
        int* val = mfa_at(arr, i);
        if (val == NULL) {
            res.result = false;
            return res;
        }
        res.result &= *val == i;
    }
    res.result &= mfa_card(arr) == 9;

    free_mfarray(arr);
    return res;
}

test_results pop_front_wrapper(void) {
    test_results res;
    res.name = "pop front wrapper";
    res.result = true;
    
    mf_array* arr = new_mfarray(sizeof(int));
    for (int i = 9; i >= 0; i--) {
        if (mfa_push_front(arr, &i) == false) {
            res.result = false;
            return res;
        } 
    }
    int* item = mfa_pop_front(arr);
    if (item == NULL) {
        res.result = false;
        return res;
    }
    res.result &= *item == 0;
    for (int i = 0; i < mfa_card(arr); i++) {
        int* val = mfa_at(arr, i);
        if (val == NULL) {
            res.result = false;
            return res;
        }
        res.result &= *val == i+1;
    }
    res.result &= mfa_card(arr) == 9;

    free_mfarray(arr);
    return res;
}
