BASEFLAGS = -Wall -Werror -m64
LIB_CFLAGS = ${BASEFLAGS} -I. -fPIC -shared -fvisibility=hidden
LIB_DBFLAGS = ${LIB_CFLAGS} -ggdb

# Allow program to link to local library
LOCAL_LINK = -Wl,-R -Wl,. libreadargs.so
LOCAL_LINK_D = -Wl,-R -Wl,. libreadargsd.so

CFLAGS = -Wall -Werror -m64 -ggdb -I. -fPIC -shared

MODULES = agents.o cache.o readargs.o

CC = gcc



libreadargs.so: $(MODULES)
	$(CC) ${LIB_CFLAGS} -o libreadargs.so $(MODULES)

agents.o: agents.c readargs.h
	$(CC) ${LIB_CFLAGS} -c -o agents.o agents.c

cache.o: cache.c readargs.h
	$(CC) ${LIB_CFLAGS} -c -o cache.o cache.c

readargs.o: readargs.c readargs.h
	$(CC) ${LIB_CFLAGS} -c -o readargs.o readargs.c

test:
	$(CC) ${BASEFLAGS} -ggdb -o test test.c ${LOCAL_LINK}


# define install_man_pages
# endef

# define build_tests
# 	$(CC) ${BASEFLAGS} -ggdb -o test test.c ${LOCAL_LINK_D}
# endef

# debug:
# 	$(CC) ${LIB_DBFLAGS} -o libreadoptsd.so readopts.c
# 	$(call build_tests)

install:
	install -D --mode=755 libreadargs.so /usr/lib
	install -D --mode=775 readargs.h     /usr/local/include
	# $(call install_man_pages)

# debug-install:
# 	$(CC) ${LIB_DBFLAGS} -o libreadoptsd.so readopts.c
# 	install -D --mode=755 libreadoptsd.so /usr/lib

uninstall:
	rm -f /usr/lib/libreadargs.so
	rm -f /usr/lib/libreadargsd.so
	rm -f /usr/local/include/readargs.h
	# rm -f /usr/share/man/man3/clargs.3.gz

clean:
	rm -f libreadargs.so
	rm -f cache.o readargs.o agents.o
	rm -f cache readargs 
	rm -f test
