#
# Makefile.am for packagemanager/src/query/tools
#

AM_CPPFLAGS = -DY2LOG=\"packagemanager\" -Wall

noinst_PROGRAMS = maketable
noinst_DATA = lookuptable.h

INCLUDES = -I$(top_srcdir)/src/query/include
maketable_SOURCES = maketable.c keywords.h

lookuptable.h: maketable ./keywords.h
	./maketable -o $@
	cp $@ ../src
