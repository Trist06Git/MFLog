#ifndef UTILS_H
#define UTILS_H

static char* unique_prefix = "U_";

int digits(int);
int digits_neg(int);
char* unique_name(int*);
char* unique_name_incr(int*);
char* file_to_string(const char*);

#endif