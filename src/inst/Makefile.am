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
	InstSrc.cc 	\
	InstSrcError.cc	\
	InstSrcDescr.cc \
	InstSrcData.cc	\
	/
	InstTarget.cc	\
	InstTargetError.cc

liby2pminstsrc_la_LIBADD = rpm/liby2pminstrpmsrc.la
