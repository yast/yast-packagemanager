#!/bin/bash

export Y2SLOG_DEBUG=1

SCRIPT=./test_rpmdb.sh

ROOT=`mktemp -qd /tmp/rpmdbtest.XXXXXX`
if [ $? -ne 0 ]; then
    echo "$0: Can't create temp dir, exiting..."
    exit 1
fi

trap "rm -rf $ROOT" EXIT

echo "testing initdb"
$SCRIPT --root $ROOT install rpms/rpm-3.0.6-1.i386.rpm
if [ ! "(" "$?" = 3 -o "$?" = 0 ")" ]; then
    echo "initdb failed"
    exit 1
fi

cd $ROOT/var/lib/rpm
for file in conflictsindex.rpm groupindex.rpm packages.rpm requiredby.rpm fileindex.rpm nameindex.rpm providesindex.rpm triggerindex.rpm; do
    echo -n "check for $file ... "
    if [ ! -f $file ]; then
	echo failed
	exit 1
    else
	echo ok
    fi
done
