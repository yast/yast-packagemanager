# installed p1 directly conflicts with candidate p2
p2
# installed smail conflicts with candidate sendmail via provided MTA
sendmail
# candidate bar conflicts with installed foo (the difference is that the
# conflict is reported at candidate again)
bar
# conflict by upgrade (1): installed smail conflicts with newer version of
# libident
libident-dev
# conflict by upgrade (2): installed smail depends on an older version of libc6
libc6
# test3 conflicts with test2, on which test1 depends; so remove-to-solve-
# conflict must contain test1 and test2
test3
# test4 also conflicts with test2, but itself provides test2 so that the
# requirement of test1 is still satisfied; remove-to-solve-conflict should be
# only test2
test4
# solve conflict by upgrade (1): candidate baz conflicts with installed
# version of foo, but an upgrade of foo is available
baz
# solve conflict by upgrade (2): installed foo2 conflicts with baz21 provided
# by candidate baz2; there's an upgrade for foo2 that doesn't conflict with
# baz21 anymore (only with baz22 still)
baz2
# solve conflict by upgrade (3): same as above, but upgrade of foo3 still
# conflicts, so this should fail
baz3
# solve conflict by upgrade (4): now an conflict-by-upgrade must be solved:
# installed requirerer depends on required < 2.0 and this should be updated
# to 2.0-1. But if also upgrading requirerer, the conflict vanishes.
required
# try the above harder: this time, the new requirerer cannot be installed
# because it dependency required2a is unresolvable
required2
# eek... this time a downgrade is available :-)
required3
# typical Debian libc5->libc6 scenario: new libfoog conflicts with
# older versions of libfoo, but can coexist with newer ones. The -dev
# package needs the libc6 ('g') lib, the -dev pkg for the libc5 lib
# has been renamed to -altdev. The whole upgrade is triggered by an
# update of foouser, which has changed its requirement from libfoo to libfoog.
foouser
