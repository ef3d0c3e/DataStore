#include "../tests/tests.h"
#include "vector.h"
#include <stdlib.h>

#define INT_TRAIT(X) \
	X(TYPE, int) \
	X(FREE, {}) \
	X(CLONE, { *new = *val; })
DATASTORE_VEC(int, vi)
typedef struct vi vi;
DATASTORE_VEC_IMPL(INT_TRAIT, vi)
#define FLOAT_TRAIT(X) \
	X(TYPE, float) \
	X(FREE, {}) \
	X(CLONE, { *new = *val; })
DATASTORE_VEC(float, vf)
typedef struct vf vf;
DATASTORE_VEC_IMPL(FLOAT_TRAIT, vf)
#define CHAR_TRAIT(X) \
	X(TYPE, char) \
	X(FREE, {}) \
	X(CLONE, { *new = *val; })
DATASTORE_VEC(char, vc)
typedef struct vc vc;
DATASTORE_VEC_IMPL(CHAR_TRAIT, vc)

TESTS(create_destroy, {
	TEST({
		vi a = vi_new(64);
		vi_free(&a);
		vi b = vi_new(0);
		vf_free(&b);
	})
})

int main(int argc, char **argv)
{
	const char *filter = NULL;
	int id_filter = -1;
	if (argc >= 2)
		filter = argv[1];
	if (argc >= 3)
		id_filter = atoi(argv[2]);
	run_tests(filter, id_filter, (unit_test[]){ test_create_destroy }, 1);
}
