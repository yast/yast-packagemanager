#
# Makefile.am for packagemanager/src/query
#

AM_CPPFLAGS = -DY2LOG=\"packagemanager\" -Wall
# -DPARSER_DEBUG
AM_LDFLAGS = -L$(libdir)

# first look into source tree, then into $(includedir)
INCLUDES = -I$(top_srcdir)/src -I$(includedir)

noinst_LTLIBRARIES = liby2pmquery.la

liby2pmquery_la_SOURCES = QueryScanner.cc QueryParser.cc QueryError.cc \
  Query.cc querycodes.h lookuptable.h
liby2pmquery_la_LDFLAGS = -version-info 2:0

$(srcdir)/QueryScanner.cc:  $(srcdir)/lookuptable.h

EXTRA_DIST = lookuptable.h

clean-local:
	rm -f lookuptable.h
