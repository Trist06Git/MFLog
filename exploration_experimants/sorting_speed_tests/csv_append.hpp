#ifndef CSV_APPEND_H
#define CSV_APPEND_H

#include <stdbool.h>

#include "generic_vector.h"

enum ca_format {CA_INT, CA_FLOAT, CA_BOOL};

bool append_csv(const char* filename, vector* elements, vector* formats);
bool is_csv(const char* filename);

#endif