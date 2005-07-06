#
# Makefile.am for packagemanager/src/instsrc
#

#SUBDIRS = rpm

AM_CPPFLAGS = -DY2LOG=\"packagemanager\" -Wall
AM_LDFLAGS = -L$(libdir)

# first look into source tree, then into $(includedir)
INCLUDES = -I$(top_srcdir)/src -I$(includedir)

noinst_LTLIBRARIES = liby2pmsrcYUM.la

liby2pmsrcYUM_la_SOURCES =
#	InstSrcDataYUM.cc	\
	PMYUMPackageDataProvider.cc
noinst_HEADERS = InstSrcDataYUM.cc LibXMLHelper.cc PMYUMPackageDataProvider.cc \
	test-YUMPrimaryData test-YUMPrimaryData.cc test-YUMRepomdData.cc YUMDependency.cc \
	YUMPrimaryData.cc YUMPrimaryDataIterator.cc YUMPrimaryDataPtr.cc YUMRepomdData.cc \
	YUMRepomdDataIterator.cc YUMRepomdDataPtr.cc
