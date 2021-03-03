PREFIX ?= /usr/local

DEBUG_FLAGS != if [ "${debug}" ] && [ "${debug}" -eq 1 ]; then echo "-ggdb"; fi

CFLAGS = -Wall -Werror -std=c99 -pedantic -m64 -fvisibility=hidden ${DEBUG_FLAGS}

DLFLAGS = -fPIC

CFLAGS += ${DLFLAGS}

TARGET = libreadargs
TARGET_SHARED = ${TARGET}.so
TARGET_STATIC = ${TARGET}.a

#### Select .c files for compilation
LIB_SOURCE != ls -1 src/*.c | grep -v 'src/test_'
LIB_MODULES != ls -1 src/*.c | grep -v 'src/test_' | sed -e 's/\.c/\.o/g'
TEST_SOURCE != ls -1 src/*.c | grep 'src/test_'
TEST_MODULES != ls -1 src/*.c | grep 'src/test_' | sed -e 's/\.c/\.o/g'

# Anticipate 
INFO_AVAIL != if which info >/dev/null; then echo 1; else echo 0; fi
INFO_DIR != a=${INFO_AVAIL}; if [ "$$a" -ne 0 ]; then ID=$$( info -w info ); echo $${ID%/*}; fi
INFO_READARGS != a=${INFO_AVAIL}; if [ "$$a" -ne 0 ]; then info -w readargs; fi

# Recipe variables defining how to do things
BUILD_INFO_RECIPE = makeinfo docs/readargs.txi; gzip readargs.info

INSTALL_INFO_RECIPE = mv readargs.info.gz ${INFO_DIR}; \
	install-info --add-once ${INFO_DIR}/readargs.info.gz ${INFO_DIR}/dir

REMOVE_INFO_RECIPE = install-info --delete ${INFO_READARGS} ${INFO_DIR}/dir; \
	rm -f ${INFO_READARGS}

# Macro variables set according to environment (if certain progams are available)
BUILD_INFO != which makeinfo > /dev/null 2>/dev/null; \
	if [ "$$?" -eq 0 ] && [ -d "${INFO_DIR}" ]; \
	then echo "${BUILD_INFO_RECIPE}"; \
	else echo ""; \
	fi;
INSTALL_INFO != if [ -f readargs.info.gz ] ; \
	then echo "${INSTALL_INFO_RECIPE}"; \
	fi;
REMOVE_INFO != if [ -f ${INFO_READARGS} ] ;  \
	then echo "${REMOVE_INFO_RECIPE}"; \
	else echo ""; \
	fi;


all: ${TARGET_SHARED} ${TARGET_STATIC}

${TARGET_SHARED}: $(LIB_MODULES)
	@echo Building the shared library ${TARGET_SHARED}
	${CC} ${CFLAGS} --shared -o $@ ${LIB_MODULES}
	${BUILD_INFO}

${TARGET_STATIC}: ${LIB_MODULES}
	@echo Building the static library ${TARGET_STATIC}
	${CC} ${CFLAGS} --shared -o $@ ${LIB_MODULES}

%.o: %.c src/readargs.h src/invisible.h
	${CC} ${CFLAGS} -c -o $@ $<

.PHONY: install
install:
	install -D --mode=755 src/readargs.h ${PREFIX}/include
	install -D --mode=755 libreadargs.so ${PREFIX}/lib
	install -D --mode=755 libreadargs.a  ${PREFIX}/lib
	ldconfig ${PREFIX}/lib
	${INSTALL_INFO}

.PHONY: uninstall
uninstall:
	rm -f ${PREFIX}/lib/readargs.h
	rm -f ${PREFIX}/lib/libreadargs.so
	rm -f ${PREFIX}/lib/libreadargs.a
	ldconfig ${PREFIX}/lib
	${REMOVE_INFO}

.PHONY: test_re
test_re:
	@echo INFO_DIR = ${INFO_DIR}
	@echo INFO_READARGS = ${INFO_READARGS}
	@echo LIB_SOURCE = ${LIB_SOURCE}
	@echo LIB_MODULES = ${LIB_MODULES}
	@echo TEST_SOURCE = ${TEST_SOURCE}
	@echo TEST_MODULES = ${TEST_MODULES}
	@echo
	@echo BUILD_INFO_RECIPE = "${BUILD_INFO_RECIPE}"
	@echo BUILD_INFO = "${BUILD_INFO}"
	@echo INSTALL_INFO = "${INSTALL_INFO}"
	@echo REMOVE_INFO = "${REMOVE_INFO}"

# Unconditional build/install.  Allow error messages
# warn inappropriate use of this target.
.PHONY: install-docs
install-docs:
	${BUILD_INFO_RECIPE}
	${INSTALL_INFO_RECIPE}

# Unconditional removal.  Allow error messages
# warn inappropriate use of this target.
.PHONY: uninstall-docs
uninstall-docs:
	${REMOVE_INFO_RECIPE}

.PHONY: clean
clean:
	rm -rf src/*.o
	rm -rf *.a
	rm -fr *.so
	rm -rf readargs.info*



























