#include "tests.h"
#include <string.h>

void
run_tests(const char* filter,
          int id_filter,
          unit_test* tests,
          size_t ntest)
{
	test_runner runner;
	runner.id_filter = id_filter;
	for (size_t i = 0; i < ntest; ++i)
	{
		if (filter && !strstr(tests[i].name, filter))
			continue;
		tests[i].runner(&runner);
	}
}
