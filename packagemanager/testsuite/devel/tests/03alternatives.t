# use a fixed default list for the test
CLEAR_DEFAULTS
DEFINE_DEFAULTS mta smail sendmail exim
DEFINE_DEFAULTS nntp nntp inn-nntp
DEFINE_DEFAULTS bar bar1 bar2
DEFINE_DEFAULTS news-transport inn leafnode cnews specialnews
# there are three alternatives for mta, one plain, one with additional
# dependencies, and one with a conflict
ALTHANDLING=ask-always mutt
# mta has defaults, the first one is smail (w/ add. dependency)
ALTHANDLING=ask-no-default mutt
# c-compiler has no default
ALTHANDLING=ask-no-default devutils
# first alternative for nntp has a conflict, so 2nd alt. should be
# selected; also: mixed real/virtual packages as providers
ALTHANDLING=ask-no-default inews
# In the mta case, sendmail is the only one which has no conflict and no
# additional dependencies, so it should be selected
ALTHANDLING=auto-always mutt
# for c-compiler, the result is non-deterministic, since both alternatives
# have no add. dep.s
ALTHANDLING=auto-always devutils
# for inews, the result should again be the conflict-free inn-nntp
ALTHANDLING=auto-always inews
# barref needs bar, for which the defaults are bar1 and bar2; however,
# bar1 doesn't really provide bar, and bar2 isn't available at all.
# (Only bar10 and bar11 provide bar). So bar10 should be selected
# (bar11 has a conflict).
ALTHANDLING=auto-no-default barref
# like above, but now in ask-if-no-default mode; since none of the
# default works, an error should be generated
ALTHANDLING=ask-no-default barref
# now all three alternatives are (self-conflicting) candidates; inn should be
# selected, because it's the first default and is an alternatives (though it
# additionally requires bar1).
ALTHANDLING=ask-no-default cnews inn leafnode
# like, above, but the simple alternative cnews should be selected now
ALTHANDLING=auto-always cnews inn leafnode
# like, above, but generate user-please-handle-this result
ALTHANDLING=ask-always cnews inn leafnode
