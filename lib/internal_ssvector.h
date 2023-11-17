#ifndef INTERNAL_SS_VECTOR_H
#define INTERNAL_SS_VECTOR_H

#include <stdbool.h>
#include "ssvector.h"

#define GROWTH_FACTOR 2
#define SS_VECTOR_INITIAL_SIZE 32

bool _internal_ssv_resize(ss_Vector *vec);

#endif
