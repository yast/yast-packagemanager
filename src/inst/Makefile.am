#
# Makefile.am for packagemanager/src/instsrc
#

SUBDIRS = include rpm

# first look into source tree, then into $(includedir)
INCLUDES = $(addprefix -I$(top_srcdir)/src/,	\
		include	\
		pkg/include	\
		sel/include	\
		you/include	\
		inst/include	\
		inst/rpm/include	\
		media/include	\
	   ) -I$(includedir) -I$(QTDIR)/include

noinst_LTLIBRARIES = liby2pminstsrc.la

liby2pminstsrc_la_SOURCES =	\
	InstSrcError.cc \
	InstTarget.cc

#	\
#	InstSrc.cc \
#	InstSrcData.cc \
#	DataOldSuSE.cc \
#	InstSrcDescr.cc \
#	InstSrcManager.cc
liby2pminstsrc_la_LIBADD = rpm/liby2pminstrpmsrc.la
