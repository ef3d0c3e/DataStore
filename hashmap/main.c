#include "hashmap.h"
#include <stdlib.h>

static uint64_t hash_fn(const char *str) {
    uint64_t h = 14695981039346656037ULL;
    while (*str) {
        h ^= (uint64_t)(unsigned char)(*str++);
        h *= 1099511628211ULL;
    }
    return h;
}

#define HS_STR(X) \
	X(KEY_TYPE, char*) \
	X(KEY_REF, const char*) \
	X(KEY_DELETE, { free(*key); }) \
	X(KEY_COPY, { copy = strdup(key); }) \
	X(KEY_HASH, { hash = hash_fn(key); }) \
	X(KEY_EQ, { eq = !strcmp(*key_a, *key_b); }) \
	X(CAPACITY, 256)

DATASTORE_HS_STATIC(HS_STR, hs)
DATASTORE_HS_STATIC_IMPL(HS_STR, hs)

int main()
{
	struct hs set;
	hs_init(&set);
	hs_insert(&set, "foobar");
	hs_insert(&set, "foobarv");
	hs_free(&set);
}
