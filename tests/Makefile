###############################################################################
#  Symbolic Finite Tree Automata Library
#
#  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
#
#  Description:
#    Makefile for tests.
#
###############################################################################


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
CFLAGS         += -Wold-style-cast
CFLAGS         += -fdiagnostics-show-option
CFLAGS         += -g
#CFLAGS         += -pg
#CFLAGS         += -O2
#CFLAGS         += -march=core2
#CFLAGS         += -mtune=core2
#CFLAGS         += -msse2

# Where to search for include files
INCLUDE_PATHS   =
INCLUDE_PATHS  += ../include
INCLUDE_PATHS  += $(CUDD_PATH)/include

# Compiler parameters for include files paths
INC             = $(patsubst %, -I%, $(INCLUDE_PATHS))

# Static library for tests
TESTS_LIB       = libtests.a

# Which libraries to link
LIBS            =
LIBS           += -lboost_unit_test_framework
LIBS           += -llog4cpp
LIBS           += -lloki

# Local object files
OBJ             = log_fixture.o

# CUDD Facade library
CUDD_FACADE_PATH = ../cudd_facade
CUDD_FACADE_LIB  = $(CUDD_FACADE_PATH)/libcudd_facade.a

# SFTA library
SFTA_PATH       = ../src
SFTA_LIB        = $(SFTA_PATH)/libsfta.a

# Static libraries
ST_LIBS         =
ST_LIBS        += $(CUDD_FACADE_LIB)
ST_LIBS        += $(SFTA_LIB)
ST_LIBS        += $(TESTS_LIB)

SRC_FILES       = $(wildcard *_test.cc)
EXE_FILES       = $(patsubst %.cc, %, $(SRC_FILES))

# Header files
HDR_FILES       =
HDR_FILES      += $(wildcard ../include/sfta/*)
HDR_FILES      += log_fixture.hh

DEL_FILES       = $(EXE_FILES) $(OBJ) $(TESTS_LIB)

# Archiver
AR              = ar

#Archiver flags
AR_CREATE_FLAGS = r
AR_EXTRACT_FLAGS= x

# Archiver indexer
RANLIB          = ranlib

###############################################################################
#                                 Rules                                       #
###############################################################################

.PHONY: all clean test

all: $(EXE_FILES)

$(CUDD_FACADE_LIB):
	cd $(CUDD_FACADE_PATH) ; make

$(SFTA_LIB):
	cd $(SFTA_PATH) ; make

$(TESTS_LIB): $(OBJ)
	$(AR) $(AR_CREATE_FLAGS) $@ $^
	$(RANLIB) $@

%.o: %.cc
	$(CC) -c $(CFLAGS) $(INC) -o $@ $<

%_test: %_test.cc $(HDR_FILES) $(ST_LIBS)
	$(CC) $(CFLAGS) $(INC) $(LIBS) -o $@ $< $(ST_LIBS)

test: all
	@echo ; \
	for i in ${EXE_FILES} ; do \
		echo "##################################### Test #####################################" ; \
		echo "# " $${i} ; \
		echo "################################################################################" ; \
		./$${i} ; \
		echo ; \
	done

clean:
	cd ../src ; make clean
	cd ../cudd_facade ; make distclean
	rm -f $(DEL_FILES)
