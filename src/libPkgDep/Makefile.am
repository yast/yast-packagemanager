## Process this file with automake to produce Makefile.in

SUBDIRS = include

AM_CPPFLAGS = -DY2LOG=\"packagemanager\" -Wall
AM_LDFLAGS = -L$(libdir)

noinst_LTLIBRARIES = libPkgDep.la

# first look into source tree, then into $(includedir)
INCLUDES = $(addprefix -I$(top_srcdir)/src/,	\
		include	\
		pkg/include	\
		sel/include	\
		you/include	\
		inst/include	\
		inst/rpm/include\
		media/include	\
		libPkgDep/include \
	   ) -I$(includedir) -I$(QTDIR)/include


libPkgDep_la_SOURCES = install.cc remove.cc consistency.cc alternatives.cc \
		       upgrade.cc results.cc output.cc utils.cc \
		       PkgSet.cc InstallOrder.cc

# Local Variables:
# tab-width: 4
# End:
