BEGIN {
  print "digraph treeautomaton {";
  print "node [shape=circle];";
}

$1 == "States" {
  for (i = 2; i <= NF; ++i)
  {
    states[$i] = 0;
  }
}

($1 == "Final") && ($2 == "States") {
  for (i = 3; i <= NF; ++i)
  {
    states[$i] = 1;
  }
}

$1 == "Transitions" {
  transitions = 1;
  # print states
  for (i in states) {
    printf("q%d", i);
    if (states[i] == 1) {
      # for final states
      printf("[peripheries=2]");
    }

    printf(";\n");
  }

  next;
}

transitions == 1 {
  split($0, array, " -> ");
  lhs = array[1];
  lhsToken = lhs;
  gsub(/,\ /, "_", lhsToken);
  gsub(/\(/, "_", lhsToken);
  gsub(/\)/, "", lhsToken);
  rhs = array[2];
  rhsStates = substr(rhs, index(rhs, "{") + 1,
    index(rhs, "}") - index(rhs,"{") -1);
  split(rhsStates, rhsArray, ", ")
  for (i in rhsArray) {
    symbol = substr(lhs, 0, index(lhs, "(") - 1);
    sequence = substr(lhs, index(lhs, "(") + 1,
      index(lhs, ")") - index(lhs, "(") - 1);

    split(sequence, sequenceStates, ", ");

    recordString = "";
    for (j in sequenceStates) {
      if (j != 1) {
        recordString = recordString "|";
      }
    }

    transName = sprintf("x%s_%s", lhsToken, rhsArray[i]);
    printf("%s [shape=record,label=\"%s\"];\n", transName, recordString);

    for (j in sequenceStates) {
      printf("q%d -> %s [label=%d];\n", sequenceStates[j], transName, j);
    }

    printf("%s -> q%d [label=%s];\n", transName, rhsArray[i], symbol);
  }
}

END {
  print "}";
}
