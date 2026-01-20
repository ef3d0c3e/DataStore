#include "test.h"

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

struct foo {
	int x;
	void *ptr;
	float u;
};
#define FOO_TRAIT(X) \
	X(TYPE, struct foo) \
	X(FREE, {}) \
	X(CLONE, { *new = *val; })
DATASTORE_VEC(struct foo, vfoo)
typedef struct vfoo vfoo;
DATASTORE_VEC_IMPL(FOO_TRAIT, vfoo)

int main(int argc, char **argv)
{
	const char *filter = NULL;
	int id_filter = -1;
	if (argc >= 2)
		filter = argv[1];
	if (argc >= 3)
		id_filter = atoi(argv[2]);
	run_tests(filter, id_filter, (unit_test[]){ test_vec_integer, test_vec_string }, 2);
}
