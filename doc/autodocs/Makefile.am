#
# Makefile.am for packagemanager/doc/autodocs
#
htmldir = $(docdir)/autodocs

html_DATA = index.html $(wildcard *.html *.png) doxygen.css @RPMNAME@.tag

CLEANFILES = $(html_DATA) doxygen.log

#EXTRA_DIST = $(html_DATA)

SRC_INCLUDEDIRS =	$(addprefix $(top_srcdir)/src/,	\
				include			\
				pkg/include		\
				sel/include		\
				you/include		\
				inst/include		\
				inst/rpm/include	\
				media/include		\
				libPkgDep/include	\
			)

SRC_INCLUDEHEADER =	$(wildcard	\
				$(addsuffix /*.h, $(SRC_INCLUDEDIRS))		\
				$(addsuffix /y2pm/*.h, $(SRC_INCLUDEDIRS))	\
			)

# liby2util is in a different rpm, refer to the installed location
# No install-data-local necessary
DOC=/usr/share/doc/packages
index.html: $(SRC_INCLUDEHEADER)
	@ybindir@/ydoxygen PROJECT_NAME=@RPMNAME@ \
		"TAGFILES=\
		\"$(DOC)/liby2util/autodocs/liby2util.tag = \
		  $(DOC)/liby2util/autodocs\" \
		"
