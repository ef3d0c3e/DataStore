#ifndef TESTS_H
#define TESTS_H

#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>

typedef struct
{
	int id_filter;
	uint32_t random_state;
	int passed;
	int total;

	/** exceptions */
	const char* err_msg;
	jmp_buf err_hook;
} test_runner;

typedef struct
{
	const char* name;
	void (*runner)(test_runner*);
} unit_test;

extern test_runner* g_runner;

#define TEST(name__, ...)                                                                        \
	{                                                                                            \
		total__ += 1;                                                                            \
		if (runner__->id_filter == -1 || runner__->id_filter == total__) {                       \
			int status__ = 0;                                                                    \
			do {                                                                                 \
				if (setjmp(runner__->err_hook) == 0) {                                           \
					__VA_ARGS__                                                                  \
				} else {                                                                         \
					fprintf(stderr, "%s\n", runner__->err_msg);                                  \
					break;                                                                       \
				}                                                                                \
				status__ = 1;                                                                    \
			} while (0);                                                                         \
			passed__ += status__;                                                                \
			fprintf(stderr,                                                                      \
			        "#%d%s%s: %s\033[0m\n",                                                      \
			        total__,                                                                     \
			        name__ != NULL ? " " : "",                                                   \
			        name__ != NULL ? name__ : "",                                                \
			        &"\033[32mOK\0\033[31mFAIL"[!status__ * 8]);                                 \
		}                                                                                        \
	}

#define TESTS(label__, ...)                                                                      \
	static void __test_##label__##eval(test_runner* runner__)                                    \
	{                                                                                            \
		if (runner__->id_filter == -1)                                                           \
			fprintf(stderr, "> Running \033[0;34m%s\033[0;0m tests...\n", #label__);             \
		int passed__ = 0;                                                                        \
		int total__ = 0;                                                                         \
		{                                                                                        \
			__VA_ARGS__                                                                          \
		}                                                                                        \
		if (runner__->id_filter == -1)                                                           \
			fprintf(stderr,                                                                      \
			        "\033[0;34m%s\033[0;0m tests [%d/%d passed]\n",                              \
			        #label__,                                                                    \
			        passed__,                                                                    \
			        total__);                                                                    \
		runner__->passed += passed__;                                                            \
		runner__->total += total__;                                                              \
	}                                                                                            \
	const unit_test test_##label__ = {                                                           \
		.name = #label__,                                                                        \
		.runner = __test_##label__##eval,                                                        \
	};

#define ASSERT(cond)                                                                             \
	if (!(cond)) {                                                                               \
		fprintf(stderr, "%s#%d: Assertion failed: `%s`\n", __FILE__, __LINE__, #cond);           \
		break;                                                                                   \
	}

void
run_tests(const char* filter, int id_filter, unit_test* tests, size_t ntest);

void*
iso_malloc(size_t size);
void*
iso_realloc(void* ptr, size_t new_size);
void
iso_free(void* ptr);

#endif // TESTS_H
