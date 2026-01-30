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
#ifndef DATASTORE_HASHSET_H
#define DATASTORE_HASHSET_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Sample trait__ X-macro
#define HS_STR(X) \
	X(KEY_TYPE, char*) \
	X(KEY_REF, const char*) \
	X(KEY_DELETE, { free(*key); }) \
	X(KEY_COPY, { copy = strdup(key); }) \
	X(KEY_HASH, { hash = hash_fn(key); }) \
	X(KEY_EQ, { eq = !strcmp(*key_a, *key_b); }) \
	X(CAPACITY, 256)

#define DATASTORE_HS_TRAIT_KEY_TYPE(tag, tokens) DATASTORE_HS_TRAIT_KEY_TYPE__##tag(tokens)
#define DATASTORE_HS_TRAIT_KEY_TYPE__KEY_TYPE(tokens) tokens
#define DATASTORE_HS_TRAIT_KEY_TYPE__KEY_REF(tokens)
#define DATASTORE_HS_TRAIT_KEY_TYPE__KEY_DELETE(tokens)
#define DATASTORE_HS_TRAIT_KEY_TYPE__KEY_COPY(tokens)
#define DATASTORE_HS_TRAIT_KEY_TYPE__KEY_HASH(tokens)
#define DATASTORE_HS_TRAIT_KEY_TYPE__KEY_EQ(tokens)
#define DATASTORE_HS_TRAIT_KEY_TYPE__CAPACITY(tokens)
#define DATASTORE_HS_TRAIT_KEY_REF(tag, tokens) DATASTORE_HS_TRAIT_KEY_REF__##tag(tokens)
#define DATASTORE_HS_TRAIT_KEY_REF__KEY_TYPE(tokens)
#define DATASTORE_HS_TRAIT_KEY_REF__KEY_REF(tokens) tokens
#define DATASTORE_HS_TRAIT_KEY_REF__KEY_DELETE(tokens)
#define DATASTORE_HS_TRAIT_KEY_REF__KEY_COPY(tokens)
#define DATASTORE_HS_TRAIT_KEY_REF__KEY_HASH(tokens)
#define DATASTORE_HS_TRAIT_KEY_REF__KEY_EQ(tokens)
#define DATASTORE_HS_TRAIT_KEY_REF__CAPACITY(tokens)
#define DATASTORE_HS_TRAIT_KEY_DELETE(tag, tokens) DATASTORE_HS_TRAIT_KEY_DELETE__##tag(tokens)
#define DATASTORE_HS_TRAIT_KEY_DELETE__KEY_TYPE(tokens)
#define DATASTORE_HS_TRAIT_KEY_DELETE__KEY_REF(tokens)
#define DATASTORE_HS_TRAIT_KEY_DELETE__KEY_DELETE(tokens) tokens
#define DATASTORE_HS_TRAIT_KEY_DELETE__KEY_COPY(tokens)
#define DATASTORE_HS_TRAIT_KEY_DELETE__KEY_HASH(tokens)
#define DATASTORE_HS_TRAIT_KEY_DELETE__KEY_EQ(tokens)
#define DATASTORE_HS_TRAIT_KEY_DELETE__CAPACITY(tokens)
#define DATASTORE_HS_TRAIT_KEY_COPY(tag, tokens) DATASTORE_HS_TRAIT_KEY_COPY__##tag(tokens)
#define DATASTORE_HS_TRAIT_KEY_COPY__KEY_TYPE(tokens)
#define DATASTORE_HS_TRAIT_KEY_COPY__KEY_REF(tokens)
#define DATASTORE_HS_TRAIT_KEY_COPY__KEY_DELETE(tokens)
#define DATASTORE_HS_TRAIT_KEY_COPY__KEY_COPY(tokens) tokens
#define DATASTORE_HS_TRAIT_KEY_COPY__KEY_HASH(tokens)
#define DATASTORE_HS_TRAIT_KEY_COPY__KEY_EQ(tokens)
#define DATASTORE_HS_TRAIT_KEY_COPY__CAPACITY(tokens)
#define DATASTORE_HS_TRAIT_KEY_HASH(tag, tokens) DATASTORE_HS_TRAIT_KEY_HASH__##tag(tokens)
#define DATASTORE_HS_TRAIT_KEY_HASH__KEY_TYPE(tokens)
#define DATASTORE_HS_TRAIT_KEY_HASH__KEY_REF(tokens)
#define DATASTORE_HS_TRAIT_KEY_HASH__KEY_DELETE(tokens)
#define DATASTORE_HS_TRAIT_KEY_HASH__KEY_COPY(tokens)
#define DATASTORE_HS_TRAIT_KEY_HASH__KEY_HASH(tokens) tokens
#define DATASTORE_HS_TRAIT_KEY_HASH__KEY_EQ(tokens)
#define DATASTORE_HS_TRAIT_KEY_HASH__CAPACITY(tokens)
#define DATASTORE_HS_TRAIT_KEY_EQ(tag, tokens) DATASTORE_HS_TRAIT_KEY_EQ__##tag(tokens)
#define DATASTORE_HS_TRAIT_KEY_EQ__KEY_TYPE(tokens)
#define DATASTORE_HS_TRAIT_KEY_EQ__KEY_REF(tokens)
#define DATASTORE_HS_TRAIT_KEY_EQ__KEY_DELETE(tokens)
#define DATASTORE_HS_TRAIT_KEY_EQ__KEY_COPY(tokens)
#define DATASTORE_HS_TRAIT_KEY_EQ__KEY_HASH(tokens)
#define DATASTORE_HS_TRAIT_KEY_EQ__KEY_EQ(tokens) tokens
#define DATASTORE_HS_TRAIT_KEY_EQ__CAPACITY(tokens)
#define DATASTORE_HS_TRAIT_CAPACITY(tag, tokens) DATASTORE_HS_TRAIT_CAPACITY__##tag(tokens)
#define DATASTORE_HS_TRAIT_CAPACITY__KEY_TYPE(tokens)
#define DATASTORE_HS_TRAIT_CAPACITY__KEY_REF(tokens)
#define DATASTORE_HS_TRAIT_CAPACITY__KEY_DELETE(tokens)
#define DATASTORE_HS_TRAIT_CAPACITY__KEY_COPY(tokens)
#define DATASTORE_HS_TRAIT_CAPACITY__KEY_HASH(tokens)
#define DATASTORE_HS_TRAIT_CAPACITY__KEY_EQ(tokens)
#define DATASTORE_HS_TRAIT_CAPACITY__CAPACITY(tokens) tokens

#define DATASTORE_HS_STATIC(trait__, name__) \
struct name__ { \
	uint8_t ctrl[trait__(DATASTORE_HS_TRAIT_CAPACITY)]; \
	trait__(DATASTORE_HS_TRAIT_KEY_TYPE) entries[trait__(DATASTORE_HS_TRAIT_CAPACITY)]; \
}; \
void name__##_init(struct name__ *self); \
void name__##_free(struct name__ *self); \
bool name__##_insert(struct name__ *self, trait__(DATASTORE_HS_TRAIT_KEY_REF) key); \
bool name__##_delete(struct name__ *self, trait__(DATASTORE_HS_TRAIT_KEY_REF) key); \
bool name__##_lookup(struct name__ *self, trait__(DATASTORE_HS_TRAIT_KEY_REF) key);

#define DATASTORE_HS_STATIC_IMPL(trait__, name__) \
void name__##_init(struct name__ *self) \
{ \
	memset(self->ctrl, 0x80, sizeof(self->ctrl)); \
	memset(self->entries, 0, sizeof(self->entries)); \
} \
void name__##_free(struct name__ *self) \
{ \
	for (size_t i = 0; i < trait__(DATASTORE_HS_TRAIT_CAPACITY); ++i) \
	{ \
		if (self->ctrl[i] == 0x80 || self->ctrl[i] == 0xFE) \
			continue; \
		trait__(DATASTORE_HS_TRAIT_KEY_TYPE) *key = &self->entries[i]; \
		trait__(DATASTORE_HS_TRAIT_KEY_DELETE) \
	} \
	memset(self->ctrl, 0x80, sizeof(self->ctrl)); \
	memset(self->entries, 0, sizeof(self->entries)); \
} \
bool name__##_insert(struct name__ *self, trait__(DATASTORE_HS_TRAIT_KEY_REF) key) \
{ \
	uint64_t hash; \
	{ \
		trait__(DATASTORE_HS_TRAIT_KEY_HASH); \
	} \
	const uint8_t f = hash & 0x7F; \
	const size_t idx = (size_t)hash % trait__(DATASTORE_HS_TRAIT_CAPACITY); \
	for (size_t probe = 0; probe < trait__(DATASTORE_HS_TRAIT_CAPACITY); ++probe) \
	{ \
		const size_t pos = (idx + probe) % trait__(DATASTORE_HS_TRAIT_CAPACITY); \
		const uint8_t c = self->ctrl[pos]; \
		if (c == 0x80 || c == 0xFE) \
		{ \
			trait__(DATASTORE_HS_TRAIT_KEY_TYPE) copy; \
			{ \
				trait__(DATASTORE_HS_TRAIT_KEY_COPY) \
			} \
			self->entries[pos] = copy; \
			self->ctrl[pos] = f; \
			return true; \
		} \
		if (c == f) \
		{ \
			bool eq; \
			{ \
				trait__(DATASTORE_HS_TRAIT_KEY_TYPE) *key_a = &self->entries[pos]; \
				const trait__(DATASTORE_HS_TRAIT_KEY_TYPE) *key_b = &key; \
				trait__(DATASTORE_HS_TRAIT_KEY_EQ) \
			} \
			if (eq) \
				return true; \
		} \
	} \
	return false; \
} \
bool name__##_delete(struct name__ *self, trait__(DATASTORE_HS_TRAIT_KEY_REF) key) \
{ \
	uint64_t hash; \
	{ \
		trait__(DATASTORE_HS_TRAIT_KEY_HASH); \
	} \
	const uint8_t f = hash & 0x7F; \
	const size_t idx = (size_t)hash % trait__(DATASTORE_HS_TRAIT_CAPACITY); \
	for (size_t probe = 0; probe < trait__(DATASTORE_HS_TRAIT_CAPACITY); ++probe) \
	{ \
		const size_t pos = (idx + probe) % trait__(DATASTORE_HS_TRAIT_CAPACITY); \
		const uint8_t c = self->ctrl[pos]; \
		if (c == 0x80) \
			return false; \
		if (c == f) \
		{ \
			bool eq; \
			{ \
				trait__(DATASTORE_HS_TRAIT_KEY_TYPE) *key_a = &self->entries[pos]; \
				const trait__(DATASTORE_HS_TRAIT_KEY_TYPE) *key_b = &key; \
				trait__(DATASTORE_HS_TRAIT_KEY_EQ) \
			} \
			if (eq) \
			{ \
				trait__(DATASTORE_HS_TRAIT_KEY_TYPE) *key = &self->entries[pos]; \
				trait__(DATASTORE_HS_TRAIT_KEY_DELETE) \
				self->ctrl[pos] = 0xFE; \
				return true; \
			} \
		} \
	} \
	return false; \
} \
bool name__##_lookup(struct name__ *self, trait__(DATASTORE_HS_TRAIT_KEY_REF) key) \
{ \
	uint64_t hash; \
	{ \
		trait__(DATASTORE_HS_TRAIT_KEY_HASH); \
	} \
	const uint8_t f = hash & 0x7F; \
	const size_t idx = (size_t)hash % trait__(DATASTORE_HS_TRAIT_CAPACITY); \
	for (size_t probe = 0; probe < trait__(DATASTORE_HS_TRAIT_CAPACITY); ++probe) \
	{ \
		const size_t pos = (idx + probe) % trait__(DATASTORE_HS_TRAIT_CAPACITY); \
		const uint8_t c = self->ctrl[pos]; \
		if (c == 0x80) \
			return false; \
		if (c == f) \
		{ \
			bool eq; \
			{ \
				trait__(DATASTORE_HS_TRAIT_KEY_TYPE) *key_a = &self->entries[pos]; \
				const trait__(DATASTORE_HS_TRAIT_KEY_TYPE) *key_b = &key; \
				trait__(DATASTORE_HS_TRAIT_KEY_EQ) \
			} \
			if (eq) \
			{ \
				return true; \
			} \
		} \
	} \
	return false; \
}

#endif // DATASTORE_HASHSET_H
