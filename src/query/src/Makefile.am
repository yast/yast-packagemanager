#
# Makefile.am for packagemanager/src/query
#

AM_CXXFLAGS = -DY2LOG=\"packagemanager\"
# -DPARSER_DEBUG

# first look into source tree, then into $(includedir)
INCLUDES = $(addprefix -I$(top_srcdir)/src/,	\
		include			\
		query/include		\
		pkg/include		\
		sel/include		\
		you/include		\
		inst/include		\
		inst/rpm/include	\
		media/include		\
                libPkgDep/include       \
	   ) -I$(includedir)

noinst_LTLIBRARIES = liby2pmquery.la

liby2pmquery_la_SOURCES = QueryScanner.cc QueryParser.cc QueryError.cc Query.cc
liby2pmquery_la_LDFLAGS = -version-info 2:0

$(srcdir)/QueryScanner.cc:  $(srcdir)/lookuptable.h

EXTRA_DIST = lookuptable.h

clean-local:
	rm -f lookuptable.h
