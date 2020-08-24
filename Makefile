# According to guidelines in GNU Standards:
SHELL=/bin/bash
.SHELLFLAGS=-O extglob -c
.SUFFIXES:
.SUFFIXES: .c .o

DESTDIR = /usr

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

define build_install_info
	makeinfo docs/readargs.txi
	gzip readargs.info
	mv readargs.info.gz /usr/share/info
	install-info --add-once /usr/share/info/readargs.info.gz /usr/share/info/dir
endef

define remove_info
	rm -rf /usr/share/info/readargs.info.gz
	install-info --delete /usr/share/info/readargs.info.gz /usr/share/info/dir
endef


.PHONY: debug
debug : $(TARGET) $(patsubst %.c,%,$(wildcard test*.c))

all : $(TARGET)

# Make libraries
$(TARGET): $(OBJ_FILES)
	@echo SRC Directory is $(srcdir) and DESTDIR is $(DESTDIR)
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $@ $(OBJ_FILES)

%_debug.o %.o: %.c readargs.h invisible.h
	$(CC) $(CFLAGS) $(LIBFLAGS) -c -o $@ $<

# Make test programs that use the library
test%: test%.c
	$(CC) $(CFLAGS) -I. -o $@ $< -Wl,-R -Wl,. $(TARGET)

.PHONY: install
install:
	install -D --mode=755 libreadargs.so $(DESTDIR)/lib
	install -D --mode=755 readargs.h     $(DESTDIR)/include
	$(call build_install_info)

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)/lib/libreadargs.so
	rm -f $(DESTDIR)/include/readargs.h
	rm -f /usr/share/info/readargs.info.gz
	$(call remove_info)

.PHONY: install-docs
install-docs:
	$(call build_install_info)

.PHONY: uninstall-docs
uninstall-docs:
	$(call remove_info)

.PHONY: html-docs
html-docs:
	cd docs; makeinfo --html readargs.txi

.PHONY: pdf-docs
pdf-docs:
	cd docs; texi2pdf readargs.txi

.PHONY: clean
clean:
	@echo Fixing to $(call fix_source,"agents_debug.o")
	rm -f *.so
	rm -f *.o
	rm -f $(patsubst test%.c,test%,$(wildcard test*.c))
	rm -rf docs/readargs
	rm docs/!(*.txi)

