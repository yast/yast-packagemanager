#
# Makefile.am for packagemanager/testsuite
#
SUBDIRS = devel.ma devel.cschum devel

AUTOMAKE_OPTIONS = dejagnu

PACKAGE = packagemanager

INCLUDES= $(addprefix -I$(top_srcdir)/src/,	\
		include	\
		pkg/include	\
		sel/include	\
		you/include	\
		inst/include	\
		inst/rpm/include\
		media/include	\
		query/include	\
		libPkgDep/include \
	   ) -I$(includedir)
