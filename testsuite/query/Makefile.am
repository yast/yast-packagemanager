#
# Makefile.am for packagemanager/testsuite
#

AUTOMAKE_OPTIONS = dejagnu
PACKAGE = query

INCLUDES = $(addprefix -I$(top_srcdir)/src/,	\
		include			\
		query/include		\
		pkg/include		\
		sel/include		\
		you/include		\
		inst/include		\
		inst/rpm/include	\
		media/include		\
		libPkgDep/include \
	   ) -I$(includedir)

## ###################################################################

EXTRA_PROGRAMS = scanner parser

scanner_SOURCES = scanner.cc
scanner_LDADD = $(top_builddir)/src/liby2pm.la -ly2util
scanner.cc: $(top_builddir)/src/query/tools/keywords.h

parser_SOURCES = parser.cc
parser_LDADD = $(top_builddir)/src/liby2pm.la -ly2util

clean-local:
	rm -f matchword
	rm -f *.log *.bak *.sum *.out *.exp

## ###################################################################

ALL: $(EXTRA_PROGRAMS)
