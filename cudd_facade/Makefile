###############################################################################
#  Symbolic Finite Tree Automata Library
#
#  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
#
#  Description:
#    Makefile for the CUDD facade.
#
###############################################################################


# The name of the output library
LIBRARY_NAME    = cudd_facade
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
CFLAGS         += -pg

# Path to CUDD library
CUDD_PATH       = ../../cudd

# Where to search for include files
INCLUDE_PATHS   =
INCLUDE_PATHS  += ../include
INCLUDE_PATHS  += $(CUDD_PATH)/include

# Compiler parameters for include files paths
INC             = $(patsubst %, -I%, $(INCLUDE_PATHS))

# Static libraries
ST_LIBS         =
ST_LIBS        += $(CUDD_PATH)/cudd/libcudd.a
ST_LIBS        += $(CUDD_PATH)/epd/libepd.a
ST_LIBS        += $(CUDD_PATH)/mtr/libmtr.a
ST_LIBS        += $(CUDD_PATH)/st/libst.a
ST_LIBS        += $(CUDD_PATH)/util/libutil.a
ST_LIBS        += $(CUDD_PATH)/dddmp/libdddmp.a

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

.PHONY: all clean distclean cudd

cudd:
	cd $(CUDD_PATH) ; make

$(LIB): $(OBJ_FILES) cudd
	TMP_DIR=`mktemp -d` ; \
	cp $(OBJ_FILES) $${TMP_DIR} ; \
	cp $(ST_LIBS) $${TMP_DIR} ; \
	( \
		cd $${TMP_DIR} ; \
		for i in *.a ; do \
			$(AR) $(AR_EXTRACT_FLAGS) $${i} ; \
		done ; \
		rm *.a ; \
		$(AR) $(AR_CREATE_FLAGS) $@ *.o ; \
		rm *.o ; \
	) ; \
	cp $${TMP_DIR}/$@ . ; \
	rm -rf $${TMP_DIR} 
	$(RANLIB) $@

%.o: %.cc $(HDR_FILES)
	$(CC) $(CFLAGS) $(INC) -o $@ $<

all: $(LIB)

clean:
	rm -f $(DEL_FILES)

distclean: clean
	rm -f $(LIB)
