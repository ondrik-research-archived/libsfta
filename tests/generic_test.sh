#!/bin/sh

DIRPATH=$(dirname "$0")
ECHO=/bin/echo

if [ $# -ne 1 ]
then
  ${ECHO} "You need to specify which operation to run!"
  exit 1
fi

case $1 in
  "union")
    OP_PRINT="union"
    OP_SFTA="--union"
    OP_REFLIB="union"
    ;;

  "intersection")
    OP_PRINT="intersection"
    OP_SFTA="--intersection"
    OP_REFLIB="intersection"
    ;;

  *)
    ${ECHO} "Operation $1 is unknown"
    exit 1
    ;;
esac

# Programs
SFTA=${DIRPATH}/../build/src/sfta
REFLIB=${DIRPATH}/../reflib/main

# Automata pool directory
AUT_DIR=${DIRPATH}/automata

# Create temporary files
SFTA_TMP=$(mktemp)
REFLIB_TMP=$(mktemp)

# Set the initial value of the result
result=0

# The green colour
green='\e[1;32m'
red='\e[1;31m'
endcolor='\e[0m'

${ECHO} "Testing ${OP_PRINT} of automata"

while read inputline ; do
  
  # Parse command-line arguments
  aut1=$(echo ${inputline} | cut -d' ' -f 1)
  aut2=$(echo ${inputline} | cut -d' ' -f 2)

  ${ECHO} -n "Testing    ${aut1} ${OP_PRINT} ${aut2}:          "

  aut1_file=${AUT_DIR}/${aut1}
  aut2_file=${AUT_DIR}/${aut2}

  # SFTA computation
  ${SFTA} ${OP_SFTA} ${aut1_file} ${aut2_file} > ${SFTA_TMP} || result=1

  # Reflib computation
  ${REFLIB} ${OP_REFLIB} ${aut1_file} ${aut2_file} > ${REFLIB_TMP} || result=1

  # Check if the result are equivalent (have equivalent language
  equivalent=$(${REFLIB} eq ${SFTA_TMP} ${REFLIB_TMP})

  # In case the automata are not equivalent
  if [ "${equivalent}" != "true" ]
  then
    result=1
    ${ECHO} -e "${red}FAILED${endcolor}"
  else
    ${ECHO} -e "${green}PASSED${endcolor}"
  fi
done

# Remove temporary files
rm ${SFTA_TMP}
rm ${REFLIB_TMP}

exit ${result}
