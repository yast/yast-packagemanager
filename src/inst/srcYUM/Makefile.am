#
# Makefile.am for packagemanager/src/instsrc
#

#SUBDIRS = rpm

AM_CPPFLAGS = -DY2LOG=\"packagemanager\" -Wall
AM_LDFLAGS = -L$(libdir) -lxml2

# first look into source tree, then into $(includedir)
INCLUDES = -I$(top_srcdir)/src -I$(oldincludedir)/libxml2 -I$(includedir) 

noinst_LTLIBRARIES = liby2pmsrcYUM.la

liby2pmsrcYUM_la_SOURCES =		\
	LibXMLHelper.cc			\
	YUMParserData.cc		\
	YUMGroupParser.cc		\
	YUMPrimaryParser.cc		\
	YUMRepomdParser.cc		\
	YUMFileListParser.cc		\
	YUMOtherParser.cc
#	InstSrcDataYUM.cc
#	PMYUMPackageDataProvider.cc

# liby2pmsrcyum_la_LIBADD = rpm/liby2pmsrcYUM.la

schemafilesdir =  $(schemadir)/packagemanager
schemafiles_DATA = $(wildcard *.rng)
EXTRA_DIST = $(schemafiles_DATA)
