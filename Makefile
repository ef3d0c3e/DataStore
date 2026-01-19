NAME := data_store
CC := gcc
CFLAGS := -Wall -Wextra -Wconversion -pedantic -std=c99 -ggdb
IFLAGS := -I./tests
LFLAGS :=

SOURCES := ./tests/tests.c

.PHONY: vector-test
vector-test: SOURCES += ./vector/tests.c
vector-test:
	$(CC) $(CFLAGS) $(IFLAGS) -o $@ $(SOURCES) $(LFLAGS)

.PHONY: $(NAME)
$(NAME): all

# All
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
	$(RM) vector-test

.PHONY: re
re: fclean all
