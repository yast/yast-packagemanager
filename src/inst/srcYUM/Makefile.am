#
# Makefile.am for packagemanager/src/instsrc
#

SUBDIRS = schema

AM_CPPFLAGS = -DY2LOG=\"packagemanager\" -Wall
AM_LDFLAGS = -L$(libdir)

# first look into source tree, then into $(includedir)
INCLUDES = -I$(top_srcdir)/src -I$(oldincludedir)/libxml2 -I$(includedir)

noinst_LTLIBRARIES = liby2pmsrcYUM.la

liby2pmsrcYUM_la_SOURCES =		\
	YUMParserData.cc		\
	YUMGroupParser.cc		\
	YUMPrimaryParser.cc		\
	YUMRepomdParser.cc		\
	YUMFileListParser.cc		\
	YUMOtherParser.cc		\
	\
	InstSrcDataYUM.cc		\
	\
	YUMImpl.cc YUMImpl.h		\
	YUMRepodata.cc YUMRepodata.h	\
	YUMPackageDataProvider.cc	\
	  YUMPackageDataProvider.h	\
	  YUMPackageDataProviderPtr.h

liby2pmsrcYUM_la_LIBADD = -lxml2

