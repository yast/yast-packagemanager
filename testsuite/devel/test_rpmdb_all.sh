#!/bin/bash
# part of yast2 packagemanager

# Author: Ludwig Nussel <lnussel@suse.de>
# Maintainer: Ludwig Nussel <lnussel@suse.de>

# Purpose: - initialize new root
#          - install fake rpm package
#          - install dummy packages
#          - remove dummy packages
#          - rebuild database
#          - check integrity of broken1 and broken2, compare output
#          - check version of dummy package
#
#          *** must be run as root ***

export Y2SLOG_DEBUG=1
#export Y2SLOG_FILE=/dev/stderr
export Y2SLOG_FILE=test_rpmdb_all.log

SCRIPT=./test_rpmdb

ROOT=`mktemp -qd /tmp/rpmdbtest.XXXXXX`
if [ $? -ne 0 ]; then
    echo "$0: Can't create temp dir, exiting..."
    exit 1
fi

echo "test root is $ROOT"

trap "rm -rf $ROOT" EXIT

echo "testing initdb and installation of rpm"
$SCRIPT --root $ROOT install rpms/rpm-3.0.6-1.i386.rpm
if [ ! "(" "$?" = 3 -o "$?" = 0 ")" ]; then
    echo "initdb failed"
    exit 1
fi

(cd $ROOT/var/lib/rpm
for file in conflictsindex.rpm groupindex.rpm packages.rpm requiredby.rpm fileindex.rpm nameindex.rpm providesindex.rpm triggerindex.rpm; do
    echo -n "check for $file ... "
    if [ ! -f $file ]; then
	echo failed
	exit 1
    else
	echo ok
    fi
done)

echo "install a package"
$SCRIPT --root $ROOT install rpms/dummy-1-1.i386.rpm
if [ ! "(" "$?" = 3 -o "$?" = 0 ")" ]; then
    echo "installation failed"
    exit 1
fi

echo "remove a package"
$SCRIPT --root $ROOT remove dummy
if [ "$?" != 0 ]; then
    echo "removal failed"
    exit 1
fi

sync

echo "rebuilddb"
$SCRIPT --root $ROOT rebuilddb
if [ "$?" != 0 ]; then
    echo "rebuilddb failed"
    exit 1
fi

echo "check package broken1"
$SCRIPT check rpms/broken1-1-1.i386.rpm > $ROOT/broken1.checklog
if [ ! "$?" = 0 ]; then
    echo "check failed"
    exit 1
fi

if ! diff -q $ROOT/broken1.checklog broken1.checklog; then
    echo check failed
    exit 1;
fi
echo "ok"

echo "check package broken2"
$SCRIPT check rpms/broken2-1-1.i386.rpm > $ROOT/broken2.checklog
if [ ! "$?" = 0 ]; then
    echo "check failed"
    exit 1
fi

if ! diff -q $ROOT/broken2.checklog broken2.checklog; then
    echo check failed
    exit 1;
fi
echo "ok"



echo "check version of package dummy"
$SCRIPT checkversion 2-1 rpms/dummy-1-1.i386.rpm  > $ROOT/dummy1.checkversionlog
if [ ! "$?" = 0 ]; then
    echo "check failed"
    exit 1
fi

if ! diff -q $ROOT/dummy1.checkversionlog dummy1.checkversionlog; then
    echo check failed
    exit 1;
fi
echo "ok"


