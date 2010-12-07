#!/bin/sh

# Programs
SFTA=../build/src/sfta
REFLIB=../reflib/main
ECHO=/bin/echo

# Automata pool directory
AUT_DIR=./automata

# Create temporary files
SFTA_TMP=$(mktemp)
REFLIB_TMP=$(mktemp)

# Set the initial value of the result
result=0

# The green colour
green='\e[1;32m'
red='\e[1;31m'
endcolor='\e[0m'

${ECHO} "Testing union of automata"

while read inputline ; do
  
  # Parse command-line arguments
  aut1=$(echo ${inputline} | cut -d' ' -f 1)
  aut2=$(echo ${inputline} | cut -d' ' -f 2)

  ${ECHO} -n "Testing    ${aut1} union ${aut2}:  "

  aut1_file=${AUT_DIR}/${aut1}
  aut2_file=${AUT_DIR}/${aut2}

  # SFTA computation
  ${SFTA} --union ${aut1_file} ${aut2_file} > ${SFTA_TMP} || result=1

  # Reflib computation
  ${REFLIB} union ${aut1_file} ${aut2_file} > ${REFLIB_TMP} || result=1

  # Check if the result are equivalent (have equivalent language
  equivalent=$(${REFLIB} eq ${SFTA_TMP} ${REFLIB_TMP})

  # In case the automata are not equivalent
  if [ ${equivalent} != true ]
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
