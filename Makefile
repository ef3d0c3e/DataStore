NAME := data_store
CC_GCC := gcc
CC_CLANG := clang-21
CFLAGS_COMMON := -Wall -Wextra -Wconversion -pedantic -std=c99 -ggdb
IFLAGS := -I./tests
LFLAGS :=

CFLAGS_GCC := $(CFLAGS_COMMON) -Wshadow -Wstrict-overflow=5 -Wformat=2 -Wcast-align -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -fstack-protector-strong -fanalyzer -ftrapv
CFLAGS_CLANG := $(CFLAGS_COMMON) -Xanalyzer


SOURCES := ./tests/tests.c ./tests/malloc.c
BINS :=

.PHONY: $(NAME)
$(NAME): all

# {{{ Vector
VECTOR_SOURCES := ./vector/main.c ./vector/vec_integer.c ./vector/vec_string.c
BINS += vector-test-gcc vector-test-clang

.PHONY: vector-test-gcc
vector-test-gcc: SOURCES += $(VECTOR_SOURCES)
vector-test-gcc:
	$(CC_GCC) $(CFLAGS_GCC) $(IFLAGS) -o $@ $(SOURCES) $(LFLAGS)

.PHONY: vector-test-clang
vector-test-clang: SOURCES += $(VECTOR_SOURCES)
vector-test-clang:
	$(CC_CLANG) $(CFLAGS_CLANG) $(IFLAGS) -o $@ $(SOURCES) $(LFLAGS)

.PHONY: vector-test
vector-test: vector-test-gcc vector-test-clang
# }}}

.PHONY: all
all: vector-test

.PHONY: docs
docs:
	@mkdir -p docs
	doxygen doxygen

.PHONY: clean
clean:
	$(RM) -r .build/

.PHONY: fclean
fclean: clean
	$(RM) $(BINS)

.PHONY: re
re: fclean all
