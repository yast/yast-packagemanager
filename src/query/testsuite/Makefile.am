#
# Makefile.am for packagemanager/testsuite
#

AUTOMAKE_OPTIONS = dejagnu
PACKAGE = query

INCLUDES = $(addprefix -I$(top_srcdir)/src/,	\
		include			\
		query/include		\
		pkg/include		\
		inst/include		\
		inst/rpm/include	\
		media/include		\
	   ) -I$(includedir) -I$(QTDIR)/include

noinst_PROGRAMS = scanner parser

scanner_SOURCES = scanner.cc
scanner_LDADD = ../src/PMQueryScanner.o
scanner.cc: ../tools/keywords.h

parser_SOURCES = parser.cc
parser_LDADD = ../src/PMQueryParser.o ../src/PMQueryScanner.o

clean-local:
	rm -f matchword
	rm -f *.log *.bak *.sum
