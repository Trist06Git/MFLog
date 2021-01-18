
#include "utils.h"

int digits(int num) {
    int res = 0;
    do {res++;} while (num/=10);
    return res;
}

int digits_neg(int num) {
    int res = num >= 0 ? 0 : 1;
    do {res++;} while (num/=10);
    return res;    
}
