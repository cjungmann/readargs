# An aid for ensuring an installed shared library can be found at runtime,
# this make file determines if the installation path is in ld.so.conf and,
# if not found, warns the user, offering an easy remedy in this makefile.

# Usage:
# Include this file after the default target, but before install.
# In your install target, add lines ${NEED_LD_SO_WARN1} and ${NEED_LD_SO_WARN2}
# They will only be displayed if the target directory IS NOT in ld.so.conf.
# 
# This makefile includes a rule for updating ld.so.conf, if the user do desires.
# The instructions for using it will be in ${NEED_LD_SO_WARN2} if needed.
#
# Example:
# all: my_project
#
# include make_need_ld_so.mk
#
# install:
#	install -D --mode=755 lib.h     ${PREFIX}/include
#	install -D --mode=775 liblib.so ${PREFIX}/lib
#	${NEED_LD_SO_WARN1}
#	${NEED_LD_SO_WARN2}
#
# That's it.

# See if the new path is needed
NEED_LD_SO != if grep -q ${PREFIX}/lib /etc/ld.so.conf; then echo 0; else echo 1; fi; [ 1 -eq 1 ]
NEED_LD_SO != nls=${NEED_LD_SO}; if [ $$nls -eq 1 ] && grep -q ${PREFIX}/lib /etc/ld.so.conf.d/*; then echo 0; else echo 1; fi; [ 1 -eq 1 ]

# Create two warning lines if the path is needed
NEED_LD_SO_WARN1 != nls=${NEED_LD_SO}; if [ $$nls -eq 1 ]; then echo \
@echo Your library may be invisible because \\\"${PREFIX}/lib\\\" is not in your path.; fi; [ 1 -eq 1 ]

NEED_LD_SO_WARN2 != nls=${NEED_LD_SO}; if [ $$nls -eq 1 ]; then echo \
@echo Type \\\"sudo make fix_ld_so_conf\\\" to add the directory to /etc/ld.so.conf.; fi; [ 1 -eq 1 ]


# Remedy target
fix_ld_so_conf:
	echo "${PREFIX}/lib" > /etc/ld.so.conf.d/usr_local_lib.conf
	ldconfig
