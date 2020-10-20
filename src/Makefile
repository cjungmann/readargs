# According to guidelines in GNU Standards:
.SHELL=${/bin/env bash}
.SHELLFLAGS=-O extglob -c
.SUFFIXES:
.SUFFIXES: .c .o

DESTDIR = /usr

CFLAGS = -Wall -Werror -std=c99 -pedantic -m64 -fvisibility=hidden

DLFLAGS = -fPIC -shared
SLFLAGS = 

LIBFLAGS = -fPIC -shared -fvisibility=hidden

TARGET = libreadargs
DYTARGET = $(TARGET).so
STTARGET = $(TARGET).a

# Test files do not belong in the library build
OBJ_FILES := $(filter-out test%.c, $(wildcard *.c))
OBJ_FILES := $(patsubst %.c, %.o, $(OBJ_FILES))

ifeq ($(MAKECMDGOALS),debug)
OBJ_FILES := $(patsubst %.o, %_debug.o, $(OBJ_FILES))
CFLAGS += -g -ggdb -DDEBUG
DYTARGET := $(subst .so,_debug.so, $(DYTARGET))
STTARGET := $(subst .a,_debug.a, $(STTARGET))
endif

LOCAL_LINK = -Wl,-R -Wl,. $(DYTARGET)

define build_install_info
	makeinfo ../docs/readargs.txi
	gzip readargs.info
	mv readargs.info.gz /usr/share/info
	install-info --add-once /usr/share/info/readargs.info.gz /usr/share/info/dir
endef

define remove_info
	rm -rf /usr/share/info/readargs.info.gz
	install-info --delete /usr/share/info/readargs.info.gz /usr/share/info/dir
endef


all: $(STTARGET) $(DYTARGET)

.PHONY: debug
debug : $(DYTARGET) $(STTARGET) $(patsubst %.c,%,$(wildcard test*.c))

# Make libraries
$(STTARGET): $(OBJ_FILES)
	@echo Building the static library $(STTARGET)
	ar rcs $(STTARGET) $(OBJ_FILES)
	ranlib $(STTARGET)

$(DYTARGET): $(OBJ_FILES)
	@echo Building the shared library $(DYTARGET)
	$(CC) $(CFLAGS) $(DLFLAGS) -o $@ $(OBJ_FILES)

%_debug.o %.o: %.c readargs.h invisible.h
	$(CC) $(CFLAGS) $(LIBFLAGS) -c -o $@ $<

# Make test programs that use the library
test%: test%.c
	$(CC) $(CFLAGS) -I. -o $@ $< -Wl,-R -Wl,. $(DYTARGET)

.PHONY: install
install:
	install -D --mode=755 libreadargs.so $(DESTDIR)/lib
	install -D --mode=755 libreadargs.a  $(DESTDIR)/lib
	install -D --mode=755 readargs.h     $(DESTDIR)/include
	$(call build_install_info)

install-debug:
	install -D --mode=755 libreadargs_debug.so $(DESTDIR)/lib/libreadargs.so
	install -D --mode=755 libreadargs_debug.a  $(DESTDIR)/lib/libreadargs.a
	install -D --mode=755 readargs.h           $(DESTDIR)/include

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)/lib/libreadargs.so
	rm -f $(DESTDIR)/lib/libreadargs.a
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
	cd ../docs; makeinfo --html readargs.txi

.PHONY: pdf-docs
pdf-docs:
	cd ../docs; texi2pdf readargs.txi

.PHONY: clean
clean:
	@echo Fixing to $(call fix_source,"agents_debug.o")
	rm -f *.so
	rm -f *.o
	rm -f $(patsubst test%.c,test%,$(wildcard test*.c))
	rm -rf ../docs/readargs
	rm -f ../docs/!(*.txi)

