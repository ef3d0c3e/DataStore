#ifndef TESTS_H
#define TESTS_H

#include <stdint.h>
#include <stdio.h>

typedef struct
{
	int id_filter;
	uint32_t random_state;
	int passed;
	int total;
} test_runner;

typedef struct
{
	const char* name;
	void (*runner)(test_runner*);
} unit_test;

#define TEST(...)                                                                                \
	{                                                                                            \
		if (runner__->id_filter == -1 || runner__->id_filter == total__ + 1) {                   \
			int status__ = 0;                                                                    \
			total__ += 1;                                                                        \
			do {                                                                                 \
				{ __VA_ARGS__ };                                                                 \
				status__ = 1;                                                                    \
			} while (0);                                                                         \
			passed__ += status__;                                                                \
			fprintf(stderr,                                                                      \
			        "#%d: %s\033[0m\n",                                                          \
			        total__,                                                                     \
			        "\033[32mOK\0\033[31mFAIL" + (!status__ * 8));                               \
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
	static const unit_test test_##label__ = {                                                    \
		.name = #label__,                                                                        \
		.runner = __test_##label__##eval,                                                        \
	};

#define ASSERT(cond)                                                                             \
	if (!(cond)) {                                                                               \
		fprintf(stderr, "%s#%d: Assertion failed: `%s`\n", __FILE__, __LINE__, #cond);     \
		break;                                                                                   \
	}

void
run_tests(const char* filter, int id_filter, unit_test* tests, size_t ntest);

#endif // TESTS_H
