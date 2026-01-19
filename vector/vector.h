/* Copyright © 2026 Lino Gamba

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the “Software”), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
#ifndef DATASTORE_VEC_H
#define DATASTORE_VEC_H

/**
 * @file An efficient, single-header, generic dynamic array iplementation
 *
 * # Usage
 *
 * @code{.c}
 * // Dynamic array definitions and methods declaration (in the .h)
 * DATASTORE_VEC(char*, str_vec)
 *
 * // Define the trait for the object in the array
 * #define STR_TRAIT(X) \
 * 	X(TYPE, char*) \
 * 	X(FREE, { free(*val); }) \
 * 	X(CLONE, { *new = strdup(*val); })
 * // Methods definitionion (in the .c)
 * DATASTORE_VEC_IMPL(STR_TRAIT, str_vec)
 * @endcode
 *
 * # Trait type
 *
 * The trait type is used to help DataStore understand how to work with your
 * types.
 *
 * The trait type for Vec requires the following X-macros:
 *  - `TYPE`: The object's type
 *  - `FREE`: Function to free your object, `{}` for none
 *  - `CLONE`: Function to clone your object
 *
 * ## Examples
 *
 * **For primitives** `int`, `float`, `char`, etc.
 *
 * @code{.c}
 * #define INT_TRAIT(X) \
 * 	X(TYPE, int) \
 * 	X(FREE, {}) \
 * 	X(CLONE, { *new = *val; })
 *
 * #define FLOAT_TRAIT(X) \
 * 	X(TYPE, float) \
 * 	X(FREE, {}) \
 * 	X(CLONE, { *new = *val; })
 * @endcode
 *
 * # Exposed methods
 *
 * The following methods are exposed and can be used DataStore's vectors:
 * - `new(size_t initial_capacity)`: Create a new vector with an initial capacity
 *   - `initial_capacity`: Number of elements the newly created vector can
 *   contain before needing to `realloc`.
 * - `free(struct vec *self)`: Free memory taken by the vector by calling the
 *   `FREE` function defined in the type trait for every elements. Then the vector
 *   itself is `free`d.
 *   - `self`: instance
 * - `clone(struct vec *self)`: Make a carbon copy of the vector. This function
 *   will call the `CLONE` function defined in the type trait for every elements.
 *   - `self`: instance
 *
 * # Examples
 *
 * @code{.c}
 * // Vector of ints
 * #define INT_TRAIT(X) \
 * 	X(TYPE, char) \
 * 	X(FREE, {}) \
 * 	X(CLONE, { *new = *val; })
 *
 * @endcode
 *
 */

#include <stdlib.h>
#include <assert.h>

#ifndef DATASTORE_CONCAT_
	#define DATASTORE_CONCAT_(x, y) x##y
#endif
#ifndef DATASTORE_CONCAT
	#define DATASTORE_CONCAT(x, y) DATASTORE_CONCAT_(x, y)
#endif

#ifndef DATASTORE_IDENT
	#define DATASTORE_IDENT(x, y) DATASTORE_CONCAT(x, DATASTORE_CONCAT(_, y))
#endif

#define DATASTORE_VEC(type__, name__) \
struct name__ \
{ \
	type__ *data; \
	size_t size; \
	size_t capacity; \
}; \
DATASTORE_VEC_METHODS(type__, name__)

#define DATASTORE_VEC_TYPEDEF(type__, name__, typedef__) \
typedef struct name__ \
{ \
	type__ *data; \
	size_t size; \
	size_t capacity; \
} typedef__; \
DATASTORE_VEC_METHODS(type__, name__)

#define DATASTORE_VEC_METHODS(type__, name__) \
struct name__ DATASTORE_IDENT(name__, new)(size_t initial_capacity); \
void DATASTORE_IDENT(name__, free)(struct name__ *self); \
struct name__ DATASTORE_IDENT(name__, clone)(const struct name__ *self); \
void DATASTORE_IDENT(name__, shrink_to_fit)(struct name__ *self); \
void DATASTORE_IDENT(name__, reserve)(struct name__ *self, size_t new_capacity); \
void DATASTORE_IDENT(name__, push)(struct name__ *self, type__ value); \
void DATASTORE_IDENT(name__, pop)(struct name__ *self);

#define DATASTORE_VEC_TRAIT_TYPE(tag, tokens) DATASTORE_VEC_TRAIT_TYPE_##tag(tokens)
#define DATASTORE_VEC_TRAIT_TYPE_TYPE(tokens) tokens
#define DATASTORE_VEC_TRAIT_TYPE_FREE(tokens)
#define DATASTORE_VEC_TRAIT_TYPE_CLONE(tokens)

#define DATASTORE_VEC_TRAIT_FREE(tag, tokens) DATASTORE_VEC_TRAIT_FREE_##tag(tokens)
#define DATASTORE_VEC_TRAIT_FREE_TYPE(tokens)
#define DATASTORE_VEC_TRAIT_FREE_FREE(tokens) tokens
#define DATASTORE_VEC_TRAIT_FREE_CLONE(tokens)

#define DATASTORE_VEC_TRAIT_CLONE(tag, tokens) DATASTORE_VEC_TRAIT_CLONE_##tag(tokens)
#define DATASTORE_VEC_TRAIT_CLONE_TYPE(tokens)
#define DATASTORE_VEC_TRAIT_CLONE_FREE(tokens)
#define DATASTORE_VEC_TRAIT_CLONE_CLONE(tokens) tokens

#define DATASTORE_VEC_IMPL(trait__, name__) \
struct name__ DATASTORE_IDENT(name__, new)(size_t initial_capacity) \
{ \
	if (initial_capacity == 0) \
		return (struct name__){ \
			.data = NULL, \
			.capacity = initial_capacity, \
			.size = 0, \
		}; \
	return (struct name__){ \
		.data = malloc(sizeof(trait__(DATASTORE_VEC_TRAIT_TYPE)) * initial_capacity), \
		.capacity = initial_capacity, \
		.size = 0, \
	}; \
} \
void DATASTORE_IDENT(name__, free)(struct name__ *self) \
{ \
	assert(self->size <= self->capacity); \
	for (size_t i = 0; i < self->size; ++i) \
	{ \
		trait__(DATASTORE_VEC_TRAIT_TYPE) *val = &self->data[i]; \
		trait__(DATASTORE_VEC_TRAIT_FREE) \
	} \
	free(self->data); \
} \
struct name__ DATASTORE_IDENT(name__, clone)(const struct name__ *self) \
{ \
	assert(self->size <= self->capacity); \
	struct name__ clone = DATASTORE_IDENT(name__, new)(self->size); \
	for (size_t i = 0; i < self->size; ++i) \
	{ \
		trait__(DATASTORE_VEC_TRAIT_TYPE) *val = &self->data[i]; \
		trait__(DATASTORE_VEC_TRAIT_TYPE) *new = &clone.data[i]; \
		trait__(DATASTORE_VEC_TRAIT_CLONE) \
	} \
	clone.size = self->size; \
	return clone; \
} \
void DATASTORE_IDENT(name__, shrink_to_fit)(struct name__ *self) \
{ \
	assert(self->size <= self->capacity); \
	if (self->size == 0) \
	{ \
		self->capacity = 0; \
		free(self->data); \
		return; \
	} \
	self->data = realloc(self->data, self->size * sizeof(*self->data)); \
	self->capacity = self->size; \
} \
void DATASTORE_IDENT(name__, reserve)(struct name__ *self, size_t new_capacity) \
{ \
	assert(self->size <= self->capacity); \
	if (self->capacity >= new_capacity || new_capacity == 0) \
		return; \
	self->data = realloc(self->data, new_capacity * sizeof(*self->data)); \
	self->capacity = new_capacity; \
} \
void DATASTORE_IDENT(name__, push)(struct name__ *self, trait__(DATASTORE_VEC_TRAIT_TYPE) value) \
{ \
	assert(self->size <= self->capacity); \
	if (self->size < self->capacity) \
	{ \
		self->data[self->size++] = value; \
		return; \
	} \
	const size_t capacity = self->capacity \
		? (self->capacity * 2) \
		: 1; \
	self->data = realloc(self->data, capacity * sizeof(*self->data)); \
	self->capacity = capacity; \
} \
void DATASTORE_IDENT(name__, pop)(struct name__ *self) \
{ \
	assert(self->size <= self->capacity); \
	assert(self->size != 0); \
	--self->size; \
	trait__(DATASTORE_VEC_TRAIT_TYPE) *val = &self->data[self->size]; \
	trait__(DATASTORE_VEC_TRAIT_FREE) \
}

#endif // DATASTORE_VEC_H
