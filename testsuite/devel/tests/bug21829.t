# lsb requires /usr/bin/lpr which is provided by both, cups-client and lprng.
# Problem:
#   - only lprng gets lsb as referer
#   - remove list contains lsb despite /usr/bin/lpr is satisfied by cups-client
lsb lprng cups-client 
# 2
lsb cups-client
# 3
lsb lprng
# 4
lsb cups
# 5
lsb
