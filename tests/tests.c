#include "tests.h"
#include <string.h>

test_runner* g_runner = NULL;

void
run_tests(const char* filter, int id_filter, unit_test* tests, size_t ntest)
{
	test_runner runner;
	memset(&runner, 0, sizeof(runner));
	runner.id_filter = id_filter;
	g_runner = &runner;
	for (size_t i = 0; i < ntest; ++i) {
		if (filter && !strstr(tests[i].name, filter))
			continue;
		tests[i].runner(&runner);
	}
	g_runner = NULL;
}
