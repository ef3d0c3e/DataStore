/* Copyright © 2026 Lino Gamba

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the “Software”), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
#ifndef DATASTORE_VEC_H
#define DATASTORE_VEC_H

#include <stdlib.h>
#include <assert.h>

/**
 * @file vector.h
 * @defgroup Vector DATASTORE_VEC: An efficient, single-header, generic dynamic array iplementation
 *
 * @brief An efficient, single-header, generic dynamic array iplementation
 *
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
 * **Macro `DATASTORE_VEC(type, name)`**: Define a new vector type
 *  - `type`: Type stored in the vector
 *  - `name`: Name of the vector type
 *
 * **Macro `DATASTORE_VEC_IMPL(type_trait, name)`**: Implements methods for a corresponding vector
 * type
 *  - `type_trait`: Type-trait X-macro, see @ref trait_type "Trait Type"
 *  - `name`: Name of the corresponding vector type
 *
 * **Macro `DATASTORE_VEC_IMPL_S(type_trait, name, settings)`**: Implements methods for a
 * corresponding vector type, takes additional settings
 *  - `type_trait`: Type-trait X-macro, see @ref trait_type "Trait Type"
 *  - `name`: Name of the corresponding vector type
 *  - `settings`: X-macro to specify the vector settings for the allocator and behavior, see
 *  \ref advanced_usage "Advanced Usage"
 *
 * The resulting vector struct will look like this:
 * @code{.c}
 * struct vec_name {
 *     vec_type *data;
 *     size_t capacity;
 *     size_t size;
 * }
 * @endcode
 * Where `vec_name` and `vec_type` are the parameters passed to macro @ref DATASTORE_VEC
 *
 * @anchor trait_type
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
 * **For primitives**: `int`, `float`, `char`, etc.
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
 * **For types that need special handling**:
 *
 * @code{.c}
 * #define STR_TRAIT(X) \
 * 	X(TYPE, char*) \
 * 	X(FREE, { free(*val); }) \
 * 	X(CLONE, { *new = strdup(*val); })
 * @endcode
 *
 * @anchor advanced_usage
 * # Advanced Usage
 *
 * You can customize the vector's allocator and behavior by using the @ref DATASTORE_VEC_IMPL_S
 * macro.
 *
 * Here are the default settings:
 * @code{.c}
 * #define DATASTORE_VEC_SETTINGS_DEFAULT(X) \
 *     X(NEW, { ptr = malloc(size); if (!ptr) abort(); }) \
 *     X(REALLOC, { ptr = realloc(ptr, size); if (!ptr) abort(); }) \
 *     X(FREE, { free(ptr); }) \
 *     X(GROW, { new_capacity = capacity != 0 ? (capacity * 2) : 1; })
 * @endcode
 *
 * - `NEW` is the allocator used when calling `vec_new(size_t size)`. It should allocate a buffer
 *   that can hold at least `size` bytes, and return it inside `ptr`. `ptr` is a non type-erased
 *   that will become the vector's `data`.
 * - `REALLOC` is the reallocator, used when changing the capacity of the internal buffer.
 *   It is used by `push`, `shrink_to_fit` and `reserve`. It should reallocate `ptr` to be able
 *   to hold at least `size` bytes, and return the new buffer inside `ptr`.
 * - `FREE` is the deallocator, used when freeing data from the vector. It should free the buffer
 *   `ptr`.
 * - `GROW` is the vector's growth strategy. It should return a value in `new_capacity` that is
 *   strictly greater than `capacity`. By defaults it doubles length, which guarantees O(1)
 *   `push` operations. Note that you may want to increase the default size from `1` to a larger
 *   value.
 *
 * Checking for allocation errors it up to you.
 *
 * # Exposed methods
 *
 * The following methods are exposed and can be used on all DataStore's vector types:
 * - `vec new(size_t initial_capacity)`: Create a new vector with an initial capacity
 *   - `initial_capacity`: Number of elements the newly created vector can
 *   contain before needing to `realloc`.
 * - `void free(struct vec *self)`: Free memory taken by the vector by calling the
 *   `FREE` function defined in the type trait for every elements. Then the vector
 *   itself is `free`d.
 *   - `self`: instance
 * - `vec clone(const struct vec *self)`: Make a carbon copy of the vector. This method
 *   will call the `CLONE` function defined in the type trait for every elements.
 *   - `self`: instance
 * - `void shrink_to_fit(struct vec *self)`: Request removal of unused capacity.
 *  This method may or may not reduce the space occupied by the vector's data (depending on
 *  `malloc` implementations).
 *   - `self`: instance
 * - `void reserve(struct vec *self, size_t new_capacity)`: Request additional capacity.
 *   This method ensure the vector can hold at least `new_capacity` elements.
 *   - `self`: instance
 *   - `new_capacity`: minimum number of elements the vector must be able to hold without calling
 *   `realloc`
 * - `void push(struct vec *self, type val)`: Append element to the vector.
 *   - `self`: instance
 *   - `val`: new element to append
 * - `void pop(struct vec *self)`: Remove the last element of the vector.
 *   Before calling this method, you must make sure the vector is not empty.
 *   - `self`: instance
 *
 * Each method must be prefixed by the name of the vector type + `_`.
 *
 * # Examples
 *
 * **Concatenate two strings**
 * @code{.c}
 * #include <vector.h>
 *
 * // Vector of chars
 * #define CHAR_TRAIT(X) \
 *     X(TYPE, char) \
 *     X(FREE, {}) \
 *     X(CLONE, { *new = *val; })
 *
 * DATASTORE_VEC(char, stringbuf)
 * DATASTORE_VEC_IMPL(CHAR_TRAIT, stringbuf)
 *
 * typedef struct stringbuf stringbuf;
 *
 * // Concatenate two null-terminated strings
 * char *concat(const char *a, const char *b)
 * {
 *    stringbuf sb = stringbuf_new(256);
 *    for (size_t i = 0; a[i]; ++i)
 *        stringbuf_push(&sb, a[i]);
 *    for (size_t i = 0; b[i]; ++i)
 *        stringbuf_push(&sb, b[i]);
 *    stringbuf_push(&sb, '\0');
 *    return sb.data;
 * }
 * @endcode
 *
 * **Matrix manipulation**
 * @code{.c}
 * #include <vector.h>
 *
 * // Vector of floats
 * #define FLT_TRAIT(X) \
 * 	X(TYPE, float) \
 * 	X(FREE, {}) \
 * 	X(CLONE, { *new = *val; })
 * DATASTORE_VEC(float, vec)
 * DATASTORE_VEC_IMPL(FLT_TRAIT, vec)
 * typedef struct vec vec;
 * 
 * // Matrix of floats
 * #define MAT_TRAIT(X) \
 * 	X(TYPE, struct vec) \
 * 	X(FREE, { vec_free(val); }) \
 * 	X(CLONE, { *new = vec_clone(val); })
 * DATASTORE_VEC(struct vec, mat)
 * DATASTORE_VEC_IMPL(MAT_TRAIT, mat)
 * typedef struct mat mat;
 * 
 * int main()
 * {
 * 	vec c1 = vec_new(2);
 * 	vec_push(&c1, 3.5f);
 * 	vec_push(&c1, -1.8f);
 * 
 * 	vec c2 = vec_new(2);
 * 	vec_push(&c2, 1.f);
 * 	vec_push(&c2, 2.7f);
 * 
 * 	mat m1 = mat_new(2);
 * 	mat_push(&m1, c1);
 * 	mat_push(&m1, c2);
 * 
 * 	mat m2 = mat_clone(&m1);
 * 	mat_free(&m1);
 * 	mat_free(&m2);
 * }
 * @endcode
 */

#ifndef DATASTORE_CONCAT_
	#define DATASTORE_CONCAT_(x, y) x##y
#endif
#ifndef DATASTORE_CONCAT
	#define DATASTORE_CONCAT(x, y) DATASTORE_CONCAT_(x, y)
#endif

#ifndef DATASTORE_IDENT
	#define DATASTORE_IDENT(x, y) DATASTORE_CONCAT(x, DATASTORE_CONCAT(_, y))
#endif

#ifndef DATASTORE_MAYBE_UNUSED
	#define DATASTORE_MAYBE_UNUSED(expr) do { (void)(expr); } while (0)
#endif

/**
 * @brief Vector type definition and methods declaration
 *
 * This macro defines the vector type and declare it's methods
 *
 * @param type__ Underlying type of the vector
 * @param name__ Name of the vector type
 */
#define DATASTORE_VEC(type__, name__) \
struct name__ \
{ \
	type__ *data; \
	size_t capacity; \
	size_t size; \
}; \
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

/**
 * @brief Default settings for the vector type
 */
#define DATASTORE_VEC_SETTINGS_DEFAULT(X) \
	X(NEW, { ptr = malloc(size); if (!ptr) abort(); }) \
	X(REALLOC, { ptr = realloc(ptr, size); if (!ptr) abort(); }) \
	X(FREE, { free(ptr); }) \
	X(GROW, { new_capacity = capacity != 0 ? (capacity * 2) : 1; })

#define DATASTORE_VEC_SETTINGS_NEW(tag, tokens) DATASTORE_VEC_SETTINGS_NEW_##tag(tokens)
#define DATASTORE_VEC_SETTINGS_NEW_NEW(tokens) tokens
#define DATASTORE_VEC_SETTINGS_NEW_REALLOC(tokens)
#define DATASTORE_VEC_SETTINGS_NEW_FREE(tokens)
#define DATASTORE_VEC_SETTINGS_NEW_GROW(tokens)

#define DATASTORE_VEC_SETTINGS_REALLOC(tag, tokens) DATASTORE_VEC_SETTINGS_REALLOC_##tag(tokens)
#define DATASTORE_VEC_SETTINGS_REALLOC_NEW(tokens)
#define DATASTORE_VEC_SETTINGS_REALLOC_REALLOC(tokens) tokens
#define DATASTORE_VEC_SETTINGS_REALLOC_FREE(tokens)
#define DATASTORE_VEC_SETTINGS_REALLOC_GROW(tokens)

#define DATASTORE_VEC_SETTINGS_FREE(tag, tokens) DATASTORE_VEC_SETTINGS_FREE_##tag(tokens)
#define DATASTORE_VEC_SETTINGS_FREE_NEW(tokens)
#define DATASTORE_VEC_SETTINGS_FREE_REALLOC(tokens)
#define DATASTORE_VEC_SETTINGS_FREE_FREE(tokens) tokens
#define DATASTORE_VEC_SETTINGS_FREE_GROW(tokens)

#define DATASTORE_VEC_SETTINGS_GROW(tag, tokens) DATASTORE_VEC_SETTINGS_GROW_##tag(tokens)
#define DATASTORE_VEC_SETTINGS_GROW_NEW(tokens)
#define DATASTORE_VEC_SETTINGS_GROW_REALLOC(tokens)
#define DATASTORE_VEC_SETTINGS_GROW_FREE(tokens)
#define DATASTORE_VEC_SETTINGS_GROW_GROW(tokens) tokens

/**
 * @brief Vector methods implementation
 *
 * This macro defines the common methods for vector types
 *
 * @param trait__ Vector type-trait for the underlying type, see @ref trait_type "Trait Type"
 * @param name__ Name of the vector, must match the name passed to @ref DATASTORE_VEC
 * @param settings__ Custom settings for the vector, see @ref advanced_usage "Advanced Usage"
 */
#define DATASTORE_VEC_IMPL_S(trait__, name__, settings__) \
struct name__ DATASTORE_IDENT(name__, new)(size_t initial_capacity) \
{ \
	if (initial_capacity == 0) \
		return (struct name__){ \
			.data = NULL, \
			.capacity = initial_capacity, \
			.size = 0, \
		}; \
	trait__(DATASTORE_VEC_TRAIT_TYPE) *ptr; \
	const size_t size = sizeof(trait__(DATASTORE_VEC_TRAIT_TYPE)) * initial_capacity; \
	settings__(DATASTORE_VEC_SETTINGS_NEW) \
	return (struct name__){ \
		.data = ptr, \
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
		DATASTORE_MAYBE_UNUSED(val); \
		trait__(DATASTORE_VEC_TRAIT_FREE) \
	} \
	trait__(DATASTORE_VEC_TRAIT_TYPE) *ptr = self->data; \
	settings__(DATASTORE_VEC_SETTINGS_FREE) \
	self->data = NULL; \
	self->capacity = 0; \
	self->size = 0; \
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
		trait__(DATASTORE_VEC_TRAIT_TYPE) *ptr = self->data; \
		settings__(DATASTORE_VEC_SETTINGS_FREE) \
		self->data = NULL; \
		self->capacity = 0; \
		return; \
	} \
	trait__(DATASTORE_VEC_TRAIT_TYPE) *ptr = self->data; \
	const size_t size = self->size * sizeof(*ptr); \
	settings__(DATASTORE_VEC_SETTINGS_REALLOC) \
	self->data = ptr; \
	self->capacity = self->size; \
} \
void DATASTORE_IDENT(name__, reserve)(struct name__ *self, size_t new_capacity) \
{ \
	assert(self->size <= self->capacity); \
	if (self->capacity >= new_capacity || new_capacity == 0) \
		return; \
	trait__(DATASTORE_VEC_TRAIT_TYPE) *ptr = self->data; \
	const size_t size = new_capacity * sizeof(*ptr); \
	settings__(DATASTORE_VEC_SETTINGS_REALLOC) \
	self->data = ptr; \
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
	const size_t capacity = self->capacity; \
	size_t new_capacity; \
	settings__(DATASTORE_VEC_SETTINGS_GROW) \
	assert(new_capacity > self->size); \
	trait__(DATASTORE_VEC_TRAIT_TYPE) *ptr = self->data; \
	const size_t size = new_capacity * sizeof(*ptr); \
	settings__(DATASTORE_VEC_SETTINGS_REALLOC) \
	self->data = ptr; \
	self->capacity = new_capacity; \
	self->data[self->size++] = value; \
} \
void DATASTORE_IDENT(name__, pop)(struct name__ *self) \
{ \
	assert(self->size <= self->capacity); \
	assert(self->size != 0); \
	--self->size; \
	trait__(DATASTORE_VEC_TRAIT_TYPE) *val = &self->data[self->size]; \
	DATASTORE_MAYBE_UNUSED(val); \
	trait__(DATASTORE_VEC_TRAIT_FREE) \
}

/**
 * @brief Vector methods implementation
 *
 * This macro defines the common methods for vector types.
 * It will call @ref DATASTORE_VEC_IMPL_S, with @ref DATASTORE_VEC_SETTINGS_DEFAULT.
 *
 * @param trait__ Vector type-trait for the underlying type, see @ref trait_type "Trait Type"
 * @param name__ Name of the vector, must match the name passed to @ref DATASTORE_VEC
 */
#define DATASTORE_VEC_IMPL(trait__, name__) \
	 DATASTORE_VEC_IMPL_S(trait__, name__, DATASTORE_VEC_SETTINGS_DEFAULT)

/** @endgroup Vector */

#endif // DATASTORE_VEC_H
