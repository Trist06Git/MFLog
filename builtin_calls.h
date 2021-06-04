#ifndef BUILTIN_CALLS_H
#define BUILTIN_CALLS_H

#include "internal_struct.h"
#include "unify.h"

outcome call_builtin(fcall*, frame*, int call_sequ);

#endif