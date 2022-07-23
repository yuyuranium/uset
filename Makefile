CC := $(shell which gcc)
PWD := $(shell pwd)

DEBUG ?= 0

ifeq ($(DEBUG), 0)
		OPT := -O3
else
		OPT := -O0 -g
endif

CFLAGS = -Wall -Iinclude $(OPT)
LDFLAGS = -L. -luset

.PHONY: clean all so ar test

all: uset.o ar

ar: uset.o
	@echo "Making static library..."
	ar rcs libuset.a uset.o

test: test_ar.out
	@echo "Testing ar..."
	./test_ar.out

test_ar.out: src/test.c ar
	@echo "Compiling $<..."
	$(CC) $< $(CFLAGS) $(LDFLAGS) -o $@

uset.o: src/uset.c include/uset.h
	@echo "Compiling $<..."
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -rf *.o *.out *.so *.a
