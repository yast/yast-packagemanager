#
# Makefile.am for packagemanager/src/instsrc
#

SUBDIRS = include rpm

# first look into source tree, then into $(includedir)
INCLUDES = $(addprefix -I$(top_srcdir)/src/,	\
		include		\
		pkg/include	\
		sel/include	\
		you/include	\
		inst/include	\
		inst/rpm/include\
		media/include	\
		libPkgDep/include	\
	   ) -I$(includedir) -I$(includedir)/y2util/qt

noinst_LTLIBRARIES = liby2pminstsrc.la

liby2pminstsrc_la_SOURCES = \
	InstSrcManager.cc 		\
	\
	InstSrc.cc 			\
	InstData.cc 			\
	InstSrcError.cc			\
	InstSrcDescr.cc 		\
	InstSrcData.cc			\
	InstSrcDataUL.cc		\
	PMULPackageDataProvider.cc	\
	PMULSelectionDataProvider.cc	\
	\
	InstTarget.cc			\
	InstTargetError.cc


liby2pminstsrc_la_LIBADD = rpm/liby2pminstrpmsrc.la
