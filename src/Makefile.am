SUBDIRS = include libPkgDb libPkgDep

noinst_PROGRAMS = testinstall
testinstall_SOURCES = testinstall.cc
testinstall_LDADD = $(top_srcdir)/src/libPkgDb/libPkgDb.la $(top_srcdir)/src/libPkgDep/libPkgDep.la

# AM_CXXFLAGS = -DYAST2DIR=\"${yast2dir}\" -DPLUGINDIR=\"${plugindir}\" 	\
#	 -DEXECCOMPDIR=\"${execcompdir}\"

#lib_LTLIBRARIES = liby2.la
#liby2_la_SOURCES = \
#	ExternalDataSource.cc ExternalProgram.cc	\
#	Y2Component.cc					\
#	Y2ComponentBroker.cc Y2ComponentCreator.cc 	\
#	genericfrontend.cc pathsearch.cc
#liby2_la_LDFLAGS = -version-info 2:0
#liby2_la_LIBADD = -lutil -ldl

INCLUDES = -I$(srcdir)/include -I$(srcdir)/libPkgDep -I$(srcdir)/libPkgDb -I$(includedir)

LIBS = -ly2util
