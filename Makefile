.SHELL: ${/usr/bin/env bash}
.OBJDIR: ./
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
LIB_SOURCE != ls -1 src/*.c | grep -v 'src/test_'
LIB_MODULES != ls -1 src/*.c | grep -v 'src/test_' | sed -e 's/\.c/\.o/g' -e 's|src\/|obj\/|g'
TEST_SOURCE != ls -1 src/*.c | grep 'src/test_'
TEST_MODULES != ls -1 src/*.c | grep 'src/test_' | sed -e 's/\.c/\.o/g' -e 's|src\/|obj\/|g'

# Recipe variables defining how to do things
BUILD_INFO_RECIPE = makeinfo docs/readargs.txi

INSTALL_INFO_RECIPE = gzip readargs.info; \
	mv readargs.info.gz /usr/share/info/readargs.info.gz; \
	install-info --add-once /usr/share/info/readargs.info.gz /usr/share/info/dir

REMOVE_INFO_RECIPE = rm -f /usr/share/info/readargs.info.gz; \
	install-info --delete /usr/share/info/readargs.info.gz /usr/share/info/dir

# Macro variables set according to environment (if certain progams are available)
BUILD_INFO != which makeinfo > /dev/null 2>/dev/null; \
	if [ "$$?" -eq 0 ] ; \
	then echo $(BUILD_INFO_RECIPE); \
	else echo ""; \
	fi;
INSTALL_INFO != if [ -f readargs.info ] ; \
	then echo $(INSTALL_INFO_RECIPE); \
	fi;
REMOVE_INFO != if [ which makeinfo > /dev/null 2>/dev/null ] ;  \
	then echo $(REMOVE_INFO_RECIPE); \
	else echo ""; \
	fi;



all: obj $(TARGET_SHARED) $(TARGET_STATIC)

$(TARGET_SHARED): $(LIB_MODULES)
	@echo LIB_SOURCE = $(LIB_SOURCE)
	echo LIB_MODULES = $(LIB_MODULES)
	@echo Building the shared library $(TARGET_SHARED)
	$(CC) $(CFLAGS) $(DLFLAGS) -o $@ $(LIB_MODULES)
	$(BUILD_INFO)

$(TARGET_STATIC): $(LIB_MODULES)
	@echo Building the shared library $(TARGET_STATIC)
	$(CC) $(CFLAGS) $(DLFLAGS) -o $@ $(LIB_MODULES)

obj/%.o: src/%.c src/readargs.h src/invisible.h
	$(CC) $(CFLAGS) $(LIBFLAGS) -c -o $@ $<

# Make the obj directory if necessary
obj:
	mkdir obj

.PHONY: install
install:
	install -D --mode=755 libreadargs.so $(DESTDIR)/lib
	install -D --mode=755 libreadargs.a  $(DESTDIR)/lib
	install -D --mode=755 src/readargs.h $(DESTDIR)/include
	$(INSTALL_INFO)

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)/lib/libreadargs.so
	rm -f $(DESTDIR)/lib/libreadargs.a
	rm -f $(DESTDIR)/lib/readargs.h
	rm -f /usr/share/info/readargs.info.gz
	$(REMOVE_INFO)

test_re:
	@echo LIB_SOURCE = $(LIB_SOURCE)
	@echo LIB_MODULES = $(LIB_MODULES)
	@echo TEST_SOURCE = $(TEST_SOURCE)
	@echo TEST_MODULES = $(TEST_MODULES)
	@echo
	@echo BUILD_INFO = $(BUILD_INFO)
	@echo INSTALL_INFO = $(INSTALL_INFO)
	@echo REMOVE_INFO = $(REMOVE_INFO)

.PHONY: clean
clean:
	rm -rf obj
	rm -rf *.a
	rm -fr *.so
	rm -rf readargs.info

.PHONY: install-docs
install-docs:
	$(BUILD_INFO)
	$(INSTALL_INFO)

.PHONY: uninstall-docs
	$(REMOVE_INFO)

