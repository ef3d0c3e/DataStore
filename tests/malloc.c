#include "tests.h"
#include <stdlib.h>

// All malloc/realloc calls need to go through these
void *iso_malloc(size_t size)
{
	if (size == 0)
	{
		g_runner->err_msg = "Attempted to malloc(0)";
		longjmp(g_runner->err_hook, 1);
	}
	return malloc(size);
}

void *iso_realloc(void *ptr, size_t new_size)
{
	if (new_size == 0 && ptr != NULL)
	{
		g_runner->err_msg = "Attempted to realloc(ptr != NULL, 0)";
		longjmp(g_runner->err_hook, 1);
	}
	return realloc(ptr, new_size);
}

void iso_free(void *ptr)
{
	free(ptr);
}
