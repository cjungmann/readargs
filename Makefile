CFLAGS = -Wall -Werror -pedantic -m64
LIBFLAGS = -fPIC -shared -fvisibility=hidden

TARGET = libreadargs.so

# Test files do not belong in the library build
OBJ_FILES := $(filter-out test%.c, $(wildcard *.c))
OBJ_FILES := $(patsubst %.c, %.o, $(OBJ_FILES))

ifeq ($(MAKECMDGOALS),debug)
OBJ_FILES := $(patsubst %.o, %_debug.o, $(OBJ_FILES))
CFLAGS += -ggdb -DDEBUG
TARGET := $(subst .so,_debug.so, $(TARGET))
endif

LOCAL_LINK = -Wl,-R -Wl,. $(TARGET)

.PHONY: debug
debug : $(TARGET) $(patsubst %.c,%,$(wildcard test*.c))

all : $(TARGET)

# Make libraries
$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $@ $(OBJ_FILES)

%_debug.o %.o: %.c
	$(CC) $(CFLAGS) $(LIBFLAGS) -c -o $@ $<

# Make test programs that use the library
test%: test%.c
	$(CC) $(CFLAGS) -I. -o $@ $< -Wl,-R -Wl,. $(TARGET)

.PHONY: install
install:
	install -D --mode=755 libreadargs.so /usr/local/lib
	install -D --mode=755 readargs.h     /usr/local/include

.PHONY: uninstall
uninstall:
	rm -f /usr/local/lib/libreadargs.so
	rm -f /usr/local/include/readargs.h

.PHONY: clean
clean:
	@echo Fixing to $(call fix_source,"agents_debug.o")
	rm -f *.so
	rm -f *.o
	rm -f $(patsubst test%.c,test%,$(wildcard test*.c))
