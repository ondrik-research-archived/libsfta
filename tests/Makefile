



# Compiler
CC              = g++

# Compiler flags
CFLAGS          =
CFLAGS         += -std=c++98
CFLAGS         += -pedantic-errors
CFLAGS         += -Wextra
CFLAGS         += -Wall
CFLAGS         += -Wfloat-equal
CFLAGS         += -Wctor-dtor-privacy
CFLAGS         += -Weffc++
CFLAGS         += -Woverloaded-virtual
CFLAGS         += -finline-functions

# Where to search for include files
INCLUDE_PATHS   =
INCLUDE_PATHS  += ../include
INCLUDE_PATHS  += ../../diplomka_programy/cudd/include

# Compiler parameters for include files paths
INC             = $(patsubst %, -I%, $(INCLUDE_PATHS))

# Which libraries to link
LIBS =
LIBS +=-lboost_unit_test_framework

SRC_FILES=$(wildcard *.cc)
EXE_FILES=$(patsubst %.cc, %, $(SRC_FILES))
HDR_FILES=$(wildcard ../include/knihovna/*)

DEL_FILES=$(EXE_FILES)

###############################################################################
#                                 Rules                                       #
###############################################################################

.PHONY: all clean test

all: $(EXE_FILES)

%: %.cc $(HDR_FILES)
	$(CC) $(CFLAGS) $(INC) $(LIBS) -o $@ $<

test: all
	@for i in ${EXE_FILES} ; do \
		./$${i} ; \
	done

clean:
	rm -f $(DEL_FILES)
