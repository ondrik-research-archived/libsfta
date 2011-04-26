# This is an Awk script that takes a file with finite tree automaton in
# symbolic Timbuk format and transforms it into regular Timbuk format

# for operations
/Ops/ {
  $1=""
  printf("Ops ")
  split($0, operations)
  for (i in operations) {
    symbol = operations[i]
    split(symbol, symbolArr, ":")
    sym = symbolArr[1]
    arity = symbolArr[2]

    if (symbolSize == "") {
      symbolSize = length(sym)
    }
    else if (length(sym) != symbolSize) {
      printf("Invalid symbol size!\n") > "/dev/stderr"
      exit 1
    }

    printSymbolicWithString(sym, ":" arity " ")
  }

  next
}

/->/ {
  input = $0
  gsub(/ /, "", input)

  ind = index(input, "(")
  if (ind == 0) {
    ind = index(input, "-")
    if (ind == 0) {
      printf("Fatal error!\n") > "/dev/stderr"
      exit 1
    }
  }

  symbol = substr(input, 1, ind-1)
  rest = substr(input, ind)
  if (length(symbol) != symbolSize) {
    printf("Fatal error: size don't match!\n") > "/dev/stderr"
    exit 1
  }

  printSymbolicWithString(symbol, rest "\n");

  next
}

{
  print
}


function arrayToString(arra, sz) {
  str = "f"
  for (i = 1; i <= sz; ++i)
  {
    str = str arra[i]
  }

  return str
}


function printArrayWithString(symbolArray, sz, stringAfter) {
  printf(arrayToString(symbolArray, sz) stringAfter)
}

function printSymbolicWithString(str, stringAfter) {
  gsub(/[01X]/, "& ", str)
  split(str, symbolArray, " ")

  arrLen = 0;
  for (j = 1; j <= symbolSize; ++j) {
    if (symbolArray[j] == "X") {
      ++arrLen
      arr[arrLen] = j
    }
  }

  for (k = 1; k <= symbolSize; ++k) {
    if (symbolArray[k] == "X") {
      symbolArray[k] = "0"
    }
  }

  digIndex = arrLen;
  while (digIndex > 0) {
    printArrayWithString(symbolArray, symbolSize, stringAfter)

    digIndex = arrLen;
    while ((digIndex >= 0) && (symbolArray[arr[digIndex]] == "1")) {
      --digIndex;
    }
    
    if (digIndex > 0) {
      symbolArray[arr[digIndex]] = "1";
      while (++digIndex <= arrLen) {
        symbolArray[arr[digIndex]] = "0";
      }
    }
  }
}
