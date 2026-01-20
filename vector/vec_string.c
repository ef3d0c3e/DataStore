#include "test.h"
#include <string.h>

static char *strdup_(const char *s)
{
	const size_t len = strlen(s) + 1;
	char *new = malloc(len);
	if (new)
		memcpy(new, s, len);
	return new;
}

#define STR_TRAIT(X) \
	X(TYPE, char*) \
	X(FREE, { free(*val); }) \
	X(CLONE, { *new = strdup_(*val); if (!*new) abort(); })
DATASTORE_VEC(char*, vs)
typedef struct vs vs;
DATASTORE_VEC_IMPL_S(STR_TRAIT, vs, SETTINGS)


TESTS(vec_string, {
	TEST("new", {
		vs a = vs_new(64);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 64)
		ASSERT(a.size == 0)

		vs_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vs b = vs_new(0);
		ASSERT(b.data == NULL)
		ASSERT(b.capacity == 0)
		ASSERT(b.size == 0)
		vs_free(&b);
	})
	TEST("double-free", {
		vs a = vs_new(64);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 64)
		ASSERT(a.size == 0)

		vs_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vs_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)
	})
	TEST("0-size", {
		vs a = vs_new(0);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vs_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		// Should not leak
		vs b = vs_new(0);
		(void)b;
	})
	TEST("0-size + push", {
		vs a = vs_new(0);
		vs_push(&a, strdup_("foobar"));
		ASSERT(a.data != NULL)
		ASSERT(a.capacity != 0)
		ASSERT(a.size == 1)
		ASSERT(!strcmp(a.data[0], "foobar"));

		vs_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)
	})
	TEST("reuse after free", {
		vs a = vs_new(15);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 15)
		ASSERT(a.size == 0)

		vs_push(&a, strdup_("foo"));
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 15)
		ASSERT(a.size == 1)
		ASSERT(!strcmp(a.data[0], "foo"));

		vs_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vs_push(&a, strdup_("bar"));
		ASSERT(a.data != NULL)
		ASSERT(a.capacity != 0)
		ASSERT(a.size == 1)
		ASSERT(!strcmp(a.data[0], "bar"));

		vs_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)
	})
	TEST("reserve(0)", {
		vs a = vs_new(0);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		// No-op
		vs_reserve(&a, 0);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vs_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		a = vs_new(64);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 64)
		ASSERT(a.size == 0)

		vs save = a;
		// No-op
		vs_reserve(&a, 0);
		ASSERT(a.data == save.data)
		ASSERT(a.capacity == save.capacity)
		ASSERT(a.size == save.size)

		vs_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)
	})
	TEST("reserve", {
		vs a = vs_new(64);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 64)
		ASSERT(a.size == 0)

		vs save = a;
		// Smaller: No-op
		vs_reserve(&a, 1);
		ASSERT(a.data == save.data)
		ASSERT(a.capacity == save.capacity)
		ASSERT(a.size == save.size)

		// Smaller: No-op
		vs_reserve(&a, 63);
		ASSERT(a.data == save.data)
		ASSERT(a.capacity == save.capacity)
		ASSERT(a.size == save.size)

		// Same-size: No-op
		vs_reserve(&a, 64);
		ASSERT(a.data == save.data)
		ASSERT(a.capacity == save.capacity)
		ASSERT(a.size == save.size)

		// Larger: Grow
		vs_reserve(&a, 65);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity >= 65)
		ASSERT(a.size == 0)

		vs_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)
	})
	TEST("shrink_to_fit(0)", {
		vs a = vs_new(0);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		// no-op
		vs_shrink_to_fit(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)
	})
	TEST("shrink_to_fit", {
		vs a = vs_new(64);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 64)
		ASSERT(a.size == 0)

		// free
		vs_shrink_to_fit(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vs_push(&a, strdup_("lorem"));
		vs_push(&a, strdup_("ipsum"));
		vs_push(&a, strdup_("dolor"));
		vs_push(&a, strdup_("sit"));
		vs_push(&a, strdup_("amet"));
		ASSERT(a.data != NULL)
		ASSERT(a.capacity >= 5)
		ASSERT(a.size == 5)
		ASSERT(!strcmp(a.data[0], "lorem"))
		ASSERT(!strcmp(a.data[1], "ipsum"))
		ASSERT(!strcmp(a.data[2], "dolor"))
		ASSERT(!strcmp(a.data[3], "sit"))
		ASSERT(!strcmp(a.data[4], "amet"))

		vs_shrink_to_fit(&a);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 5)
		ASSERT(a.size == 5)
		vs_free(&a);
	})
	TEST("clone(0)", {
		vs a = vs_new(0);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vs b = vs_clone(&a);
		ASSERT(b.data == NULL)
		ASSERT(b.capacity == 0)
		ASSERT(b.size == 0)
	})
	TEST("clone", {
		vs a = vs_new(0);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vs_push(&a, strdup_("lorem"));
		vs_push(&a, strdup_("ipsum"));
		vs_push(&a, strdup_("dolor"));
		vs_push(&a, strdup_("sit"));
		vs_push(&a, strdup_("amet"));
		ASSERT(a.data != NULL)
		ASSERT(a.capacity >= 5)
		ASSERT(a.size == 5)
		ASSERT(!strcmp(a.data[0], "lorem"))
		ASSERT(!strcmp(a.data[1], "ipsum"))
		ASSERT(!strcmp(a.data[2], "dolor"))
		ASSERT(!strcmp(a.data[3], "sit"))
		ASSERT(!strcmp(a.data[4], "amet"))

		vs b = vs_clone(&a);
		ASSERT(b.data != NULL)
		ASSERT(b.data != a.data)
		ASSERT(b.capacity >= 5)
		ASSERT(b.size == 5)
		ASSERT(!strcmp(a.data[0], b.data[0]))
		ASSERT(!strcmp(a.data[1], b.data[1]))
		ASSERT(!strcmp(a.data[2], b.data[2]))
		ASSERT(!strcmp(a.data[3], b.data[3]))
		ASSERT(!strcmp(a.data[4], b.data[4]))

		vs_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)
		vs_free(&b);
		ASSERT(b.data == NULL)
		ASSERT(b.capacity == 0)
		ASSERT(b.size == 0)
	})
	TEST("push from empty", {
		vs a = vs_new(0);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vs_push(&a, strdup_("foo"));
		vs_push(&a, strdup_("bar"));
		vs_push(&a, strdup_("baz"));
		ASSERT(a.data != NULL)
		ASSERT(a.capacity >= 3)
		ASSERT(a.size == 3)
		ASSERT(!strcmp(a.data[0], "foo"))
		ASSERT(!strcmp(a.data[1], "bar"))
		ASSERT(!strcmp(a.data[2], "baz"))

		vs_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)
	})
	TEST("push", {
		vs a = vs_new(64);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 64)
		ASSERT(a.size == 0)

		vs_push(&a, strdup_("foo"));
		vs_push(&a, strdup_("bar"));
		vs_push(&a, strdup_("baz"));
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 64)
		ASSERT(a.size == 3)
		ASSERT(!strcmp(a.data[0], "foo"))
		ASSERT(!strcmp(a.data[1], "bar"))
		ASSERT(!strcmp(a.data[2], "baz"))

		vs_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		a = vs_new(3);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 3)
		ASSERT(a.size == 0)

		// Must not grow
		vs_push(&a, strdup_("foo"));
		vs_push(&a, strdup_("bar"));
		vs_push(&a, strdup_("baz"));
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == 3)
		ASSERT(a.size == 3)
		ASSERT(!strcmp(a.data[0], "foo"))
		ASSERT(!strcmp(a.data[1], "bar"))
		ASSERT(!strcmp(a.data[2], "baz"))

		// Must grow
		vs_push(&a, strdup_("quz"));
		ASSERT(a.data != NULL)
		ASSERT(a.capacity >= 4)
		ASSERT(a.size == 4)
		ASSERT(!strcmp(a.data[0], "foo"))
		ASSERT(!strcmp(a.data[1], "bar"))
		ASSERT(!strcmp(a.data[2], "baz"))
		ASSERT(!strcmp(a.data[3], "quz"))

		vs_free(&a);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)
	})
	TEST("pop", {
		vs a = vs_new(0);
		ASSERT(a.data == NULL)
		ASSERT(a.capacity == 0)
		ASSERT(a.size == 0)

		vs_push(&a, strdup_("foo"));
		vs_push(&a, strdup_("bar"));
		vs_push(&a, strdup_("baz"));
		ASSERT(a.data != NULL)
		ASSERT(a.capacity >= 3)
		ASSERT(a.size == 3)
		ASSERT(!strcmp(a.data[0], "foo"))
		ASSERT(!strcmp(a.data[1], "bar"))
		ASSERT(!strcmp(a.data[2], "baz"))


		const size_t prev_cap = a.capacity;
		vs_pop(&a);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == prev_cap)
		ASSERT(a.size == 2)

		vs_pop(&a);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == prev_cap)
		ASSERT(a.size == 1)

		vs_pop(&a);
		ASSERT(a.data != NULL)
		ASSERT(a.capacity == prev_cap)
		ASSERT(a.size == 0)

		vs_free(&a);
	})
})
