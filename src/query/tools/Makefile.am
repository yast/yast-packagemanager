#
# Makefile.am for packagemanager/src/query/tools
#

noinst_PROGRAMS = maketable
noinst_DATA = lookuptable.h

maketable_SOURCES = maketable.c keywords.h

lookuptable.h: maketable ./keywords.h
	./maketable -o $@
	mv $@ ../src
