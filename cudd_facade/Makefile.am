# Process this file with automake to produce Makefile.in

CUDD_PATH = ./cudd

AM_CXXFLAGS = -I$(CUDD_PATH)/include

lib_LIBRARIES = libcudd_facade.a

libcudd_facade_a_SOURCES = \
		cudd_facade.cc \
		cudd_facade.hh

libcudd_facade_a_LIBADD = $(CUDD_PATH)/cudd/libcudd.a
