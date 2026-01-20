#include "test.h"

#define INT_TRAIT(X) \
	X(TYPE, int) \
	X(FREE, {}) \
	X(CLONE, { *new = *val; })
DATASTORE_VEC(int, vi)
typedef struct vi vi;
DATASTORE_VEC_IMPL_S(INT_TRAIT, vi, SETTINGS)

TESTS(vec_integer, {
	TEST("new", {
		vi a = vi_new(64);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 64)
		ASSERT(a.size == 0)

		vi_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vi b = vi_new(0);
		ASSERT(b.data == NULL)
		ASSERT(b.capacity == 0)
		ASSERT(b.size == 0)
		vi_free(&b);
	})
	TEST("double-free", {
		vi a = vi_new(64);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 64)
		ASSERT(a.size == 0)

		vi_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vi_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)
	})
	TEST("0-size", {
		vi a = vi_new(0);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vi_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		// Should not leak
		vi b = vi_new(0);
		(void)b;
	})
	TEST("0-size + push", {
		vi a = vi_new(0);
		vi_push(&a, 123);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity != 0)
		ASSERT(a.size == 1)
		ASSERT(a.data[0] == 123);

		vi_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)
	})
	TEST("reuse after free", {
		vi a = vi_new(15);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 15)
		ASSERT(a.size == 0)

		vi_push(&a, 12);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 15)
		ASSERT(a.size == 1)
		ASSERT(a.data[0] == 12)

		vi_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vi_push(&a, 5);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity != 0)
		ASSERT(a.size == 1)
		ASSERT(a.data[0] == 5)

		vi_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)
	})
	TEST("reserve(0)", {
		vi a = vi_new(0);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		// No-op
		vi_reserve(&a, 0);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vi_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		a = vi_new(64);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 64)
		ASSERT(a.size == 0)

		vi save = a;
		// No-op
		vi_reserve(&a, 0);
		ASSERT(a.data == save.data)
		ASSERT(a.capacity == save.capacity)
		ASSERT(a.size == save.size)

		vi_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)
	})
	TEST("reserve", {
		vi a = vi_new(64);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 64)
		ASSERT(a.size == 0)

		vi save = a;
		// Smaller: No-op
		vi_reserve(&a, 1);
		ASSERT(a.data == save.data)
		ASSERT(a.capacity == save.capacity)
		ASSERT(a.size == save.size)

		// Smaller: No-op
		vi_reserve(&a, 63);
		ASSERT(a.data == save.data)
		ASSERT(a.capacity == save.capacity)
		ASSERT(a.size == save.size)

		// Same-size: No-op
		vi_reserve(&a, 64);
		ASSERT(a.data == save.data)
		ASSERT(a.capacity == save.capacity)
		ASSERT(a.size == save.size)

		// Larger: Grow
		vi_reserve(&a, 65);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity >= 65)
		ASSERT(a.size == 0)

		vi_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)
	})
	TEST("shrink_to_fit(0)", {
		vi a = vi_new(0);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		// no-op
		vi_shrink_to_fit(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)
	})
	TEST("shrink_to_fit", {
		vi a = vi_new(64);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 64)
		ASSERT(a.size == 0)

		// free
		vi_shrink_to_fit(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vi_push(&a, 55);
		vi_push(&a, 44);
		vi_push(&a, 33);
		vi_push(&a, 22);
		vi_push(&a, 11);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity >= 5)
		ASSERT(a.size == 5)
		ASSERT(a.data[0] == 55)
		ASSERT(a.data[1] == 44)
		ASSERT(a.data[2] == 33)
		ASSERT(a.data[3] == 22)
		ASSERT(a.data[4] == 11)

		vi_shrink_to_fit(&a);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 5)
		ASSERT(a.size == 5)
		vi_free(&a);
	})
	TEST("clone(0)", {
		vi a = vi_new(0);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vi b = vi_clone(&a);
		ASSERT(b.data == NULL)
		ASSERT(b.capacity == 0)
		ASSERT(b.size == 0)
	})
	TEST("clone", {
		vi a = vi_new(0);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vi_push(&a, 55);
		vi_push(&a, 44);
		vi_push(&a, 33);
		vi_push(&a, 22);
		vi_push(&a, 11);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity >= 5)
		ASSERT(a.size == 5)
		ASSERT(a.data[0] == 55)
		ASSERT(a.data[1] == 44)
		ASSERT(a.data[2] == 33)
		ASSERT(a.data[3] == 22)
		ASSERT(a.data[4] == 11)

		vi b = vi_clone(&a);
		ASSERT(b.data != NULL)
		ASSERT(b.data != a.data)
		ASSERT(b.capacity >= 5)
		ASSERT(b.size == 5)
		ASSERT(b.data[0] == a.data[0])
		ASSERT(b.data[1] == a.data[1])
		ASSERT(b.data[2] == a.data[2])
		ASSERT(b.data[3] == a.data[3])
		ASSERT(b.data[4] == a.data[4])

		vi_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)
		vi_free(&b);
		ASSERT(b.data == NULL)
		ASSERT(b.capacity == 0)
		ASSERT(b.size == 0)
	})
	TEST("push from empty", {
		vi a = vi_new(0);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vi_push(&a, 0);
		vi_push(&a, 1);
		vi_push(&a, 2);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity >= 3)
		ASSERT(a.size == 3)
		ASSERT(a.data[0] == 0)
		ASSERT(a.data[1] == 1)
		ASSERT(a.data[2] == 2)

		vi_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)
	})
	TEST("push", {
		vi a = vi_new(64);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 64)
		ASSERT(a.size == 0)

		vi_push(&a, 0);
		vi_push(&a, 1);
		vi_push(&a, 2);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 64)
		ASSERT(a.size == 3)
		ASSERT(a.data[0] == 0)
		ASSERT(a.data[1] == 1)
		ASSERT(a.data[2] == 2)

		vi_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		a = vi_new(3);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 3)
		ASSERT(a.size == 0)

		// Must not grow
		vi_push(&a, 0);
		vi_push(&a, 1);
		vi_push(&a, 2);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 3)
		ASSERT(a.size == 3)
		ASSERT(a.data[0] == 0)
		ASSERT(a.data[1] == 1)
		ASSERT(a.data[2] == 2)

		// Must grow
		vi_push(&a, 3);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity >= 4)
		ASSERT(a.size == 4)
		ASSERT(a.data[0] == 0)
		ASSERT(a.data[1] == 1)
		ASSERT(a.data[2] == 2)
		ASSERT(a.data[3] == 3)

		vi_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)
	})
	TEST("pop", {
		vi a = vi_new(0);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vi_push(&a, 0);
		vi_push(&a, 1);
		vi_push(&a, 2);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity >= 3)
		ASSERT(a.size == 3)
		ASSERT(a.data[0] == 0)
		ASSERT(a.data[1] == 1)
		ASSERT(a.data[2] == 2)

		const size_t prev_cap = a.capacity;
		vi_pop(&a);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == prev_cap)
		ASSERT(a.size == 2)

		vi_pop(&a);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == prev_cap)
		ASSERT(a.size == 1)

		vi_pop(&a);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == prev_cap)
		ASSERT(a.size == 0)

		vi_free(&a);
	})
})
