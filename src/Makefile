###############################################################################
#  Symbolic Finite Tree Automata Library
#
#  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
#
#  Description:
#    Makefile for SFTA source files.
#
###############################################################################


# The name of the output library
LIBRARY_NAME    = sfta
LIB             = lib$(LIBRARY_NAME).a

# Compiler
CC              = g++

# Compiler flags
CFLAGS          =
CFLAGS         += -c
CFLAGS         += -std=c++98
CFLAGS         += -pedantic-errors
CFLAGS         += -Wextra
CFLAGS         += -Wall
CFLAGS         += -Wfloat-equal
CFLAGS         += -Wctor-dtor-privacy
CFLAGS         += -Weffc++
CFLAGS         += -Woverloaded-virtual
CFLAGS         += -finline-functions
CFLAGS         += -g

# Where to search for include files
INCLUDE_PATHS   =
INCLUDE_PATHS  += ../include

# Compiler parameters for include files paths
INC             = $(patsubst %, -I%, $(INCLUDE_PATHS))

# Static libraries
ST_LIBS         =

SRC_FILES=$(wildcard *.cc)
OBJ_FILES=$(patsubst %.cc, %.o, $(SRC_FILES))
HDR_FILES=$(wildcard *.hh)

DEL_FILES=$(OBJ_FILES)

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

.PHONY: all clean distclean

$(LIB): $(OBJ_FILES)
	$(AR) $(AR_CREATE_FLAGS) $@ $^
	$(RANLIB) $@

%.o: %.cc $(HDR_FILES)
	$(CC) $(CFLAGS) $(INC) -o $@ $<

all: $(LIB)

clean:
	rm -f $(DEL_FILES)

distclean: clean
	rm -f $(LIB)
