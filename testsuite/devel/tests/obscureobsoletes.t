# 1 foo2 obsoletes foo and foo-devel. foo-devel requires foo. No problem since
# foo-devel is obsoleted
foo2
# 2 same thing but lets see what happens if packages are protected
foo2 @foo @foo-devel
# 3 same but this time via obsoletion of provides
foo2old
# 4 barold replaces bar via provides. bar-devel gets broken
barold
# 5 barold2 obsoletes bar2 which is needed by bar2-devel. bar2-devel however also
# provides barold2 and is therefore replaced => no problem
barold2
# 6 same thing but lets see what happens if packages are protected
barold2 @bar2 @bar2-devel
# 7 aaa obsoletes bbb, should raise problem
aaa bbb
# 8 as above but bbb is required by ccc
aaa ccc
# 9 aaa required by ddd, bbb required by ccc
ddd ccc
# 10 fff obsoletes eee. should raise conflict
eee fff
# 11 same but via deps
ggg hhh
