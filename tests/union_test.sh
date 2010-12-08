#!/bin/sh
DIRPATH=$(dirname "$0")
cat ${DIRPATH}/union_test_automata.txt | ${DIRPATH}/generic_test.sh union
