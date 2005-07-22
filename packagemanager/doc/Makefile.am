#
# Makefile.am for liby2/doc
#

SUBDIRS = autodocs media ui manager-ui Solver you

htmldir = $(docdir)

html_DATA =

updatemessage_DATA = yast2-packagemanager.rpmdb.configfiles
updatemessagedir = /usr/share/update-messages/en

EXTRA_DIST = $(html_DATA) README $(updatemessage_DATA)

%.ps: %.xml
	db2ps $<
