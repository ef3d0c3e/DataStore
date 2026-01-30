#ifndef RB_TREE_H
#define RB_TREE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>


#define DATASTORE_RBTREE(name__, trait__) \
struct name__##_impl_node { \
	struct name__##_impl_##node *parent; \
	struct name__##_impl_##node *left; \
	struct name__##_impl_##node *right; \
	uint8_t color; \
	trait__##_TYPE data; \
}; \
struct name__ { \
	struct name__##_impl_##node *root; \
	size_t size; \
}; \
struct name__ name__##_new(void); \
void name__##_free(struct name__ *rb); \
trait__##_TYPE* name__##_insert(struct name__ *rb, trait__##_TYPE value); \
bool name__##_delete(struct name__ *rb, trait__##_KEY_REF key); \
trait__##_TYPE* name__##_min(struct name__ *rb); \
const trait__##_TYPE* name__##_cmin(const struct name__ *rb); \
trait__##_TYPE* name__##_max(struct name__ *rb); \
const trait__##_TYPE* name__##_cmax(const struct name__ *rb); \
trait__##_TYPE* name__##_get(struct name__ *rb, trait__##_KEY_REF key); \
const trait__##_TYPE* name__##_cget(const struct name__ *rb, trait__##_KEY_REF key); \
void name__##_apply(struct name__ *rb, void (*fn)(trait__##_TYPE *data, size_t depth, void *cookie)); \
void name__##_capply(const struct name__ *rb, void (*fn)(const trait__##_TYPE *data, size_t depth, void *cookie));


#define DATASTORE_RBTREE_IMPL(name__, trait__) \
struct name__ name__##_new(void) \
{ \
	return (struct name__){ .root = NULL, .size = 0 };\
} \
static void name__##_impl_free_node(struct name__##_impl_node *node) \
{ \
	if (!node) return; \
	trait__##_DESTROY((&node->data)) \
	name__##_impl_free_node(node->left); \
	name__##_impl_free_node(node->right); \
	trait__##_FREE_NODE((node)); \
} \
void name__##_free(struct name__ *rb) \
{ \
	name__##_impl_free_node(rb->root); \
	rb->root = NULL; \
	rb->size = 0; \
} \
static inline void name__##_impl_rotate_left(struct name__ *rb, struct name__##_impl_node *x) \
{ \
	struct name__##_impl_node *y; \
	y = x->right; \
	x->right = y->left; \
	if (y->left != NULL) \
		y->left->parent = x; \
	y->parent = x->parent; \
	if (x->parent == NULL) \
		rb->root = y; \
	else if (x == x->parent->left) \
		x->parent->left = y; \
	else \
		x->parent->right = y; \
	y->left = x; \
	x->parent = y; \
} \
static inline void name__##_impl_rotate_right(struct name__ *rb, struct name__##_impl_node *x) \
{ \
	struct name__##_impl_node *y; \
	y = x->left; \
	x->left = y->right; \
	if (y->right != NULL) \
		y->right->parent = x; \
	y->parent = x->parent; \
	if (x->parent == NULL) \
		rb->root = y; \
	else if (x == x->parent->left) \
		x->parent->left = y; \
	else \
		x->parent->right = y; \
	y->right = x; \
	x->parent = y; \
} \
trait__##_TYPE* name__##_insert(struct name__ *rb, trait__##_TYPE value) \
{ \
	struct name__##_impl_node *parent = NULL, *current = rb->root; \
	while (current != NULL) \
	{ \
		int cmp; \
		trait__##_KEY_CMP(cmp, (&value), (&current->data)); \
		parent = current; \
		if (cmp == 0) \
		{ \
			trait__##_DESTROY((&current->data)); \
			current->data = value; \
			return (&current->data); \
		} \
		else if (cmp > 0) \
			current = current->right; \
		else \
			current = current->left; \
	} \
	++rb->size; \
	trait__##_ALLOC_NODE(current, (sizeof(*current))); \
	current->parent = parent; \
	current->left = current->right = NULL; \
	current->color = 0; \
	current->data = value; \
	if (parent == NULL) \
	{ \
		assert(rb->root == NULL); \
		rb->root = current; \
	} \
	else \
	{ \
		int cmp; \
		trait__##_KEY_CMP(cmp, (&value), (&parent->data)); \
		if (cmp < 0) \
		{ \
			assert(parent->left == NULL); \
			parent->left = current; \
		} \
		else \
		{ \
			assert(parent->right == NULL); \
			parent->right = current; \
		} \
	} \
	if (current->parent != NULL && current->parent->color == 0) \
	{ \
		struct name__##_impl_node *node = current, *parent, *gparent; \
		while (node != rb->root && node->parent->color == 0) \
		{ \
			parent = node->parent; \
			gparent = parent->parent; \
			if (gparent == NULL) break; \
			if (parent == gparent->left) \
			{ \
				struct name__##_impl_node *cur = gparent->right; \
				if (cur != NULL && cur->color == 0) \
				{ \
					cur->color = 1; \
					parent->color = 1; \
					gparent->color = 0; \
					node = gparent; \
				} \
				else \
				{ \
					if (node == parent->right) \
					{ \
						node = parent; \
						name__##_impl_rotate_left(rb, node); \
						parent = node->parent; \
						gparent = parent->parent; \
					} \
					parent->color = 1; \
					gparent->color = 0; \
					name__##_impl_rotate_right(rb, gparent); \
					break; \
				} \
			} \
			else \
			{ \
				struct name__##_impl_node *cur = gparent->left; \
				if (cur != NULL && cur->color == 0) \
				{ \
					cur->color = 1; \
					parent->color = 1; \
					gparent->color = 0; \
					node = gparent; \
				} \
				else \
				{ \
					if (node == parent->left) \
					{ \
						node = parent; \
						name__##_impl_rotate_right(rb, node); \
						parent = node->parent; \
						gparent = parent->parent; \
					} \
					parent->color = 1; \
					gparent->color = 1; \
					name__##_impl_rotate_left(rb, gparent); \
					break; \
				} \
			} \
		} \
	} \
	rb->root->color = 1; \
	return (&current->data); \
}


#endif // RB_TREE_H
