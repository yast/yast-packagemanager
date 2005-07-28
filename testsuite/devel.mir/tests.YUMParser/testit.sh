#!/bin/bash
for testcase in '*.test.xml'
do
    mode="${testcase#*-}"
    mode="${testcase%%-*}"
    ref="$testcase.ref"
    ../YUMtest "$mode" "$testcase" > "$testcase.out"
    echo "Exit Code: $?" >> "$testcode.out"
    if diff -q "$ref" "$testcase.out" > /dev/null
    then
        echo "pass $testcase"
    else
        echo "fail $testcase"
    fi
done

