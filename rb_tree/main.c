#include "rb_tree.h"
#include <string.h>
struct arb_node {
	char *key;
	int value
};

#define ARB_TYPE struct arb_node
#define ARB_KEY_REF const char*
#define ARB_KEY_CMP(cmp, lhs, rhs) { cmp = strcmp(lhs->key, rhs->key); }
#define ARB_DESTROY(data) { free(data->key); }
#define ARB_ALLOC_NODE(ptr, size) { ptr = malloc(size); if (!ptr) abort(); }
#define ARB_FREE_NODE(ptr) { free(ptr); }

DATASTORE_RBTREE(rb, ARB)
DATASTORE_RBTREE_IMPL(rb, ARB)

int main()
{
	struct rb tree = rb_new();

	rb_insert(&tree, (struct arb_node){
			.key = strdup("foobar"),
			.value = 64
	});
	rb_insert(&tree, (struct arb_node){
			.key = strdup("foobar"),
			.value = 32
	});
	rb_free(&tree);
}

