# simplest test of all: the package doesn't need anything additional
bash
# transitive addition: amd needs libgdbm, which needs a newer version of libc6
# than installed
# NB: libgdbm should refer to the libc6 update, but not amd itself, because
# its requirements are also satisfied by the old version
amd
# test with non-available dependency (1): any version
foo1
# test with non-available dependency (2): with version relation
foo2
# test with non-available dependency (3): with complex version relations
foo2a foo2b foo2c
# test with non-available dependency (4): with contradicting version relations
foo2d foo2e
# test with required file (1): pulls in bash for /bin/sh
foo3
# test with required file (2): needs non-standard /usr/bin/bar
# provided by foo3 (i.e., in the available set, too)
foo4
# test with required file (3): needs non-standard /sbin/ldconfig
# provided by libc6 (i.e., from different set)
foo5
