.SHELL=${/bin/env bash}
# .SHELLFLAGS=-O extglob -c
.SUFFIXES:
.SUFFIXES: .c .o

DESTDIR = /usr

CFLAGS = -Wall -Werror -std=c99 -pedantic -m64 -fvisibility=hidden

DLFLAGS = -fPIC -shared
LIBFLAGS = -fPIC -shared -fvisibility=hidden

TARGET = libreadargs
TARGET_SHARED = $(TARGET).so
TARGET_STATIC = $(TARGET).a

#### Select .c files for compilation

# GNU-specific, fails on BSD
# OBJ_FILES := $(filter-out src/test%.c, $(wildcard *.c))
# OBJ_FILES := $(patsubst %.c, %.o, $(OBJ_FILES))

# BSD-specific, fails on GNU (abandoned before fully-working)
# LIB_FILES = $(OBJ_FILES:C/test/proof/g)

# Cross-platform, using sed.  Works to omit test_*.c
# LIB_FILES != ls -1 src/*.c | sed -E 's|src/test_[^ ]+| |g'
# Abandoned after many attempts.  Realized that grep was better tool.
# TEST_FILES != ls -1 src/*.c | sed -E 's|src/\(?!test_\)[^ ]+| |g'

# Cross-platform, using grep
LIB_SOURCE != ls -1 src/*.c | grep -v 'src/test_'
LIB_MODULES != ls -1 src/*.c | grep -v 'src/test_' | sed -e 's/\.c/\.o/g' -e 's|src\/|obj\/|g'
TEST_SOURCE != ls -1 src/*.c | grep 'src/test_'
TEST_MODULES != ls -1 src/*.c | grep 'src/test_' | sed -e 's/\.c/\.o/g' -e 's|src\/|obj\/|g'


all: $(TARGET_SHARED) $(TARGET_STATIC)

$(TARGET_SHARED): $(LIB_MODULES)
	@echo LIB_SOURCE = $(LIB_SOURCE)
	echo LIB_MODULES = $(LIB_MODULES)
	@echo Building the shared library $(TARGET_SHARED)
	$(CC) $(CFLAGS) $(DLFLAGS) -o $@ $(LIB_MODULES)

$(TARGET_STATIC): $(LIB_MODULES)
	@echo Building the shared library $(TARGET_STATIC)
	$(CC) $(CFLAGS) $(DLFLAGS) -o $@ $(LIB_MODULES)

obj/%.o: src/%.c src/readargs.h src/invisible.h
	$(CC) $(CFLAGS) $(LIBFLAGS) -c -o $@ $<

test_re:
	@echo LIB_SOURCE = $(LIB_SOURCE)
	@echo LIB_MODULES = $(LIB_MODULES)
	@echo TEST_SOURCE = $(TEST_SOURCE)
	@echo TEST_MODULES = $(TEST_MODULES)
