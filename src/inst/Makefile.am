#
# Makefile.am for packagemanager/src/instsrc
#

SUBDIRS = include rpm

AM_CPPFLAGS = -DY2LOG=\"packagemanager\" -Wall

# first look into source tree, then into $(includedir)
INCLUDES = $(addprefix -I$(top_srcdir)/src/,	\
		include		\
		pkg/include	\
		sel/include	\
		you/include	\
		inst/include	\
		inst/rpm/include\
		media/include	\
		libPkgDep/include\
	   ) -I$(includedir)

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
	ULSelectionParser.cc		\
	ULPackagesParser.cc		\
	PMULPackageDataProvider.cc	\
	PMULSelectionDataProvider.cc	\
	InstSrcDataPLAIN.cc		\
	PMPLAINPackageDataProvider.cc	\
	\
	InstTarget.cc			\
	InstTargetError.cc		\
	InstTargetSelDB.cc		\
	InstTargetProdDB.cc		\
	UpdateInfParser.cc

liby2pminstsrc_la_LIBADD = rpm/liby2pminstrpmsrc.la
