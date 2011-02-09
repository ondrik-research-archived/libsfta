#!/bin/bash

if [ $# -ne 2 ]
then
  echo "Usage: `basename $0` file1 file2"
  exit 1
fi

tmp[1]=$(mktemp)
tmp[2]=$(mktemp)

tmp_sorted[1]=$(mktemp)
tmp_sorted[2]=$(mktemp)

cat $1 | tr ',' '\n' | sort > ${tmp[1]}
cat $2 | tr ',' '\n' | sort > ${tmp[2]}

for i in 1 2 ; do
  exec < ${tmp[i]}
  while read inputline; do
    state=$(echo -n ${inputline} | sed 's/\ *[\{\}]\ *//g' | cut -d ' ' -f 1 | sed 's/\ //g')
    echo -n ${state} "->[" >> ${tmp_sorted[i]}

    echo -n ${inputline} | sed 's/.*\[\(.*\)\].*/\1/' | tr ';' '\n' | sort | tr '\n' ';' | sed 's/\ //g' >> ${tmp_sorted[i]}

    echo "]" >> ${tmp_sorted[i]}
  done
done

diff ${tmp_sorted[1]} ${tmp_sorted[2]}
RESULT=$?

rm ${tmp[1]}
rm ${tmp[2]}
rm ${tmp_sorted[1]}
rm ${tmp_sorted[2]}

exit ${RESULT}
