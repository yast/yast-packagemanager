#!/bin/bash

#export Y2SLOG_DEBUG=1
export Y2SLOG_FILE=/dev/stdout

http_proxy="http://najar.suse.de:3128/"
ftp_proxy=$http_proxy
export http_proxy ftp_proxy

destdir=tmp

servers=("ftp://hewitt.suse.de:/suse-STABLE" "http://najar.suse.de/manual/;proxyuser=heinz;proxypassword=ketchup")
files=("/apt/SuSE/STABLE-i386/examples/sources.list.FTP" "/LICENSE")
#cd:///suse /mnt /setup/descr/info

i=0;
while test "$i" -lt "${#servers[@]}";
do
	./media_test.prg ${servers[$i]} tmp ${files[$i]}
	i=$((i+1))
done
