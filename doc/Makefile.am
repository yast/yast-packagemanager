#
# Makefile.am for liby2/doc
#

SUBDIRS = autodocs media

htmldir = $(docdir)

html_DATA =

EXTRA_DIST = $(html_DATA) README

%.ps: %.xml
	db2ps $<
