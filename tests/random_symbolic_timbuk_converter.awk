# Awk script that substitutes each symbol in a Timbuk-format finite tree
# automaton for a symbolic symbol in the form of {0, 1, X}^n, where n is the
# length of the symbolic symbol

BEGIN {
  bitsize = ARGV[1]
  ARGV[1] = ""
  if (bitsize == 0) {
    printf("Fatal error: need to specify bit size as ARGV[1]!\n") > "/dev/stderr"
    exit 1
  }

  # Length of prefix of each symbol corresponding to arity
  arityPrefixLength = 2

  if (arityPrefixLength > bitsize) {
    printf("Fatal error: arity prefix longer than size!\n") > "/dev/stderr"
    exit 1
  }

  normalSize = bitsize - arityPrefixLength

  srand()
}

# for operations
/Ops/ {
  $1=""
  split($0, operations)
  for (i in operations) {
    symbol = operations[i]
    split(symbol, symbolArr, ":")
    sym = symbolArr[1]
    arity = symbolArr[2]
    newSymbol = generate_random_symbol(arity)
    translationTable[sym] = newSymbol
    arityTable[sym] = arity
  }

  printf("Ops ")
  for (i in translationTable) {
    printf("%s:%d ", translationTable[i], arityTable[i])
  }

  next
}

/->/ {
  pos = match($0, /[^0-9a-zA-Z]/)
  if (pos == 0) {
    printf("Invalid format of line:\n%s", $0) > "/dev/stderr"
    exit 1
  }

  symbol = substr($0, 1, pos-1)
  rest = substr($0, pos, length())

  if (!(symbol in translationTable)) {
    printf("Invalid symbol: %s\n", symbol) > "/dev/stderr"
    exit 1
  }

  print translationTable[symbol] rest

  next
}

{
  print
}

function randint(n) {
   return int(n * rand())
 }


function generate_random_symbol(arity) {
  str = ""
  maxarity = 1
  for (i = 1; i <= arityPrefixLength; ++i) {
    maxarity *= 2
  }

  --maxarity

  if (arity > maxarity) {
    printf("Arity too big: %d (maximum is %d)\n", arity, maxarity) > "/dev/stderr"
    exit 1
  }

  for (i = 1; i <= arityPrefixLength; ++i) {
    str = str (arity % 2)
    arity = int(arity/2)
  }

  for (i = 1; i <= normalSize; ++i) {
    r = randint(3)
    if (r == 2) {
      str = str "X"
    }
    else {
      str = str r
    }
  }

  return str
}
