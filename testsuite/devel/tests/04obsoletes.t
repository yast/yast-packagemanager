# foo is installed, but obsoleted by candidate foonew; so fooc finds no target
# for its conflict
foonew fooc
# same with bar* packages: now barnew provides bar (in version MAX_VERS), so
# now the conflict target is provided
barnew barc
# same with baz* packages, but now the conflict is only on older versions of
# baz, so no conflict takes place
baznew bazc
# libfoo2 obsoletes but not provides libfoo, so it break the dependency of
# something
libfoo2
# libbar2 obsoletes and provides libbar, so someother's dependency (though
# versioned) is still ok
libbar2
# libbaz2 obsoletes and provides libbaz, so it breaks the dependency of
# somemore.; however, somemore can be upgraded so that the conflict is solved
libbaz2
# Aiee... candidate B obsoletes candidate A; A shouldn't appear in the output
# list 
A B
# should also work in different order
B C
