# lsb requires /usr/bin/lpr which is provided by both, cups-client and lprng.
# Problem:
#   - only lprng gets lsb as referer
#   - remove list contains lsb despite /usr/bin/lpr is satisfied by cups-client
lsb cups-client lprng 
