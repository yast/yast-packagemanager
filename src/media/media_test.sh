#!/bin/bash

#export Y2SLOG_DEBUG=1
export Y2SLOG_FILE=/dev/stdout

destdir=tmp

servers=("ftp://hewitt.suse.de:/suse-STABLE" "http://najar.suse.de/manual/")
files=("/apt/SuSE/STABLE-i386/examples/sources.list.FTP" "/LICENSE")

i=0;
while test "$i" -lt "${#servers[@]}";
do
	./media_test.prg ${servers[$i]} tmp ${files[$i]}
	i=$((i+1))
done
