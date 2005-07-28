#!/bin/bash
# goes through each testcase in this directory
# see README
for testcase in *.test.xml
do
    mode="${testcase#*-}"
    mode="${mode%%-*}"
    ref="$testcase.ref"
    err_ref="$testcase.err.ref"

    ../YUMtest "$mode" < "$testcase" > "$testcase.out" 2>"$testcase.err.tmp"
    echo "Exit Code: $?" >> "$testcase.out"

    # remove date and host name from logs
    cut -b 36- < "$testcase.err.tmp" > "$testcase.err.out"
    rm "$testcase.err.tmp"

    if diff -q "$ref" "$testcase.out" > /dev/null \
       && diff -q "$err_ref" "$testcase.err.out" > /dev/null
    then
        echo "pass $testcase"
    else
        echo "fail $testcase"
    fi
done


