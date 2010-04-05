



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
CFLAGS         += -g

# Where to search for include files
INCLUDE_PATHS   =
INCLUDE_PATHS  += ../include
INCLUDE_PATHS  += $(CUDD_PATH)/include

# Compiler parameters for include files paths
INC             = $(patsubst %, -I%, $(INCLUDE_PATHS))

# Which libraries to link
LIBS            =
LIBS           += -lboost_unit_test_framework
LIBS           += -llog4cpp

# Static libraries
ST_LIBS         =
ST_LIBS        += ../cudd_facade/libcudd_facade.a

SRC_FILES       = $(wildcard *.cc)
EXE_FILES       = $(patsubst %.cc, %, $(SRC_FILES))
HDR_FILES       = $(wildcard ../include/sfta/*)

DEL_FILES       = $(EXE_FILES)

###############################################################################
#                                 Rules                                       #
###############################################################################

.PHONY: all clean test

all: cudd_facade $(EXE_FILES)

cudd_facade:
	cd ../cudd_facade ; make

%: %.cc $(HDR_FILES) $(ST_LIBS)
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
	rm -f $(DEL_FILES)
