#! /bin/bash
#
PRG=test_rpmdb

if [ ! -x ~ma/bin/sy2l.tcl ]; then
	exec ./$PRG
fi

LPIPE=/tmp/Y2LOGFILE
test -p "$LPIPE" || {
        test -e "$LPIPE" && { echo "No pipe '$LPIPE' exists"; exit 1; }
        mkfifo $LPIPE
}
if ! fuser $LPIPE >/dev/null; then
        ~ma/bin/sy2l.tcl $LPIPE &
fi

export Y2SLOG_FILE=/dev/stderr
export Y2SLOG_DEBUG=1
# export Y2DEBUG=1
./$PRG "$@" 2> $LPIPE
