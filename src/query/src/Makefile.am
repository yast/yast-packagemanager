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
		inst/include		\
		inst/rpm/include	\
		media/include		\
	   ) -I$(includedir) -I$(QTDIR)/include

lib_LTLIBRARIES = liby2query.la

liby2query_la_SOURCES = PMQueryScanner.cc PMQueryParser.cc PMQueryError.cc PMQuery.cc

$(srcdir)/lookuptable.h: $(top_srcdir)/src/query/tools/keywords.h
	$(top_srcdir)/src/query/tools/maketable -o $@

$(srcdir)/PMQueryScanner.cc:  $(srcdir)/lookuptable.h
liby2query_la_LDFLAGS = -version-info 2:0

clean-local:
	rm -f lookuptable.h
