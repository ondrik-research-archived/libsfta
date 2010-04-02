



# Compiler settings
CC=g++
CFLAGS=-std=c++98 -pedantic-errors -Wextra -Wall -Wfloat-equal \
       -Wctor-dtor-privacy -Weffc++ -Woverloaded-virtual -finline-functions

INCLUDE_PATH=../include

LIBS=-lboost_unit_test_framework

SRC_FILES=$(wildcard *.cc)
EXE_FILES=$(patsubst %.cc, %, $(SRC_FILES))
HDR_FILES=$(wildcard ../include/knihovna/*)

DEL_FILES=$(EXE_FILES)

###############################################################################
#                                 Rules                                       #
###############################################################################

.PHONY: all clean

all: $(EXE_FILES)

%: %.cc $(HDR_FILES)
	$(CC) $(CFLAGS) -I$(INCLUDE_PATH) $(LIBS) -o $@ $<

clean:
	rm -f $(DEL_FILES)
