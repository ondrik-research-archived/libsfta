## Process this file with automake to produce Makefile.in

SFTA_LIBRARY = libsfta.a

LDADD = ../cudd_facade/libcudd_facade.a \
				$(SFTA_LIBRARY)

lib_LIBRARIES = $(SFTA_LIBRARY)

libsfta_a_SOURCES = \
	convert.cc \
	fake_file.cc \
	formula_parser.cc

bin_PROGRAMS = sfta

sfta_SOURCES = sfta.cc
