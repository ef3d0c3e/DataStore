#ifndef DATASTORE_VEC_TEST_H
#define DATASTORE_VEC_TEST_H

#include "../tests/tests.h"
#include "vector.h"

#define SETTINGS(X) \
    X(NEW, { ptr = iso_malloc(size); if (!ptr) abort(); }) \
    X(REALLOC, { ptr = iso_realloc(ptr, size); if (!ptr) abort(); }) \
    X(FREE, { iso_free(ptr); }) \
    X(GROW, { new_capacity = capacity != 0 ? (capacity * 2) : 1; })
// Use this to re-enable -fanalyzer checks
#define SETTINGS_FANALYZER(X) \
    X(NEW, { ptr = malloc(size); if (!ptr) abort(); }) \
    X(REALLOC, { ptr = realloc(ptr, size); if (!ptr) abort(); }) \
    X(FREE, { free(ptr); }) \
    X(GROW, { new_capacity = capacity != 0 ? (capacity * 2) : 1; })

extern const unit_test test_vec_integer;
extern const unit_test test_vec_string;

#endif // DATASTORE_VEC_TEST_H
