BASEFLAGS = -Wall -Werror -m64
LIB_CFLAGS = ${BASEFLAGS} -I. -fPIC -shared -fvisibility=hidden
LIB_DBFLAGS = ${LIB_CFLAGS} -ggdb

# Allow program to link to local library
LOCAL_LINK = -Wl,-R -Wl,. libreadargs.so
LOCAL_LINK_D = -Wl,-R -Wl,. libreadargsd.so

CFLAGS = -Wall -Werror -m64 -ggdb -I. -fPIC -shared

MODULES = agents.o cache.o readargs.o

CC = gcc

libreadargs.so : $(patsubst %.c, %.o, $(wildcard *.c))
	$(CC) $(LIB_CFLAGS) -o $@ $^

%.o : %.c
	$(CC) -c ${LIB_CFLAGS} $< -o $@


# define install_man_pages
# endef

# define build_tests
# 	$(CC) ${BASEFLAGS} -ggdb -o test test.c ${LOCAL_LINK_D}
# endef

# debug:
# 	$(CC) ${LIB_DBFLAGS} -o libreadoptsd.so readopts.c
# 	$(call build_tests)

# install:
# 	install -D --mode=755 libreadargs.so /usr/lib
# 	install -D --mode=775 readargs.h     /usr/local/include
# 	# $(call install_man_pages)

# debug-install:
# 	$(CC) ${LIB_DBFLAGS} -o libreadoptsd.so readopts.c
# 	install -D --mode=755 libreadoptsd.so /usr/lib

uninstall:
	rm -f /usr/lib/libreadargs.so
	rm -f /usr/lib/libreadargsd.so
	rm -f /usr/local/include/readargs.h
	# rm -f /usr/share/man/man3/clargs.3.gz

clean:
	rm -f *.so
	rm -f *.o
	rm -f $(patsubt %, %.c, $(wildcard *.c))
