CLEAR_DEFAULTS
DEFINE_DEFAULTS jdk-runtime jdk-shared jdk-static
# install some misc packages from the avail set
dosemu dviutils fetchmail freeciv ghostscript
# ...and some others
inn isdn4k-utils kdbg
# these two conflict with each other
ALTHANDLING=auto-no-default jdk-shared jdk-static
