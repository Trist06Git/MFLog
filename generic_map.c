
#include "generic_map.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "generic_vector.h"

void map_push_back(map* mp, void* element1, void* element2) {
    map_add(mp, element1, element2);
}

//does not deal with duplicate keys..
void map_add(map* mp, void* element1, void* element2) {
    pair els;
    els.fst = malloc(mp->el1_size);
    els.snd = malloc(mp->el2_size);
    memcpy(els.fst, element1, mp->el1_size);
    memcpy(els.snd, element2, mp->el2_size);
    vec_push_back(mp->store, &els);
}

//returns second of pair
void* map_at(map* mp, void* key) {
    return fst(mp, key);
}

//returns pair
void* map_at_index(map* mp, int i) {
    return vec_at(mp->store, i);
}

void* snd(map* mp, void* key) {
    for (int i = 0; i < vec_size(mp->store); i++) {
        pair* kv = vec_at(mp->store, i);
        if (mp->el1_comparator(key, kv->fst, mp->el1_size)) {
            return kv->snd;
        }
    }
    return NULL;
}

void* fst(map* mp, void* val) {//could be merged with snd
    for (int i = 0; i < vec_size(mp->store); i++) {
        pair* kv = vec_at(mp->store, i);
        if (mp->el2_comparator(val, kv->snd, mp->el2_size)) {
            return kv->fst;
        }
    }
    return NULL;
}

int map_remove(map* mp, void* key) {
    for (int i = 0; i < vec_size(mp->store); i++) {
        pair* kv = vec_at(mp->store, i);
        if (mp->el1_comparator(key, kv->fst, mp->el1_size)) {
            free(kv->fst);
            free(kv->snd);
            vec_remove_at(mp->store, i);
            return 0;
        }
    }
    return -1;
}

bool map_contains_key(map* mp, void* key) {
    for (int i = 0; i < vec_size(mp->store); i++) {
        pair* kv = vec_at(mp->store, i);
        if (mp->el1_comparator(key, kv->fst, mp->el1_size)) {
            return true;
        }
    }
    return false;
}///////reaaaaly needs merging
bool map_contains_val(map* mp, void* val) {
    for (int i = 0; i < vec_size(mp->store); i++) {
        pair* kv = vec_at(mp->store, i);
        if (mp->el2_comparator(val, kv->snd, mp->el2_size)) {
            return true;
        }
    }
    return false;
}

int map_size(map* mp) {
    return vec_size(mp->store);
}

bool byte_compare(void* e1, void* e2, int bytes) {
    char* c_lhs = e1;
    char* c_rhs = e2;
    for (int i = 0; i < bytes; i++) {
        //if (*(char*)e1 != *(char*)e2) return false;
        if (c_lhs[i] != c_rhs[i]) return false;
    }
    return true;
}

bool string_compare(void* el1, void* el2, int size) {
    //size not needed..
    char* str1 = *(char**)el1;
    char* str2 = *(char**)el2;
    return strcmp(str1, str2) == 0;
}

void set_fst_comparator(map* mp, bool (*comp1)(void*, void*, int)) {
    mp->el1_comparator = comp1;
}

void set_snd_comparator(map* mp, bool (*comp2)(void*, void*, int)) {
    mp->el2_comparator = comp2;
}

map* new_map(int el1_size, int el2_size) {
    map* res = malloc(sizeof(map));
    res->store = new_vector(0, sizeof(pair));
    res->el1_size = el1_size;
    res->el2_size = el2_size;
    res->el1_comparator = byte_compare;
    res->el2_comparator = byte_compare;
    
    /*printf("created map ::\n");
    printf("element1 size   : %i\n\n", res->el1_size);
    printf("element2 size   : %i\n\n", res->el2_size);*/
    return res;
}

void free_map(map* mp) {
    if (mp == NULL) return;
    if (mp->store != NULL) {
        for (int i = 0; i < vec_size(mp->store); i++) {
            pair* els = vec_at(mp->store, i);
            free(els->fst);
            free(els->snd);
        }
        free_vector(mp->store);
    }
    free(mp);
}
