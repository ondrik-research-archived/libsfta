## Process this file with automake to produce Makefile.in

TEST_LIBRARY = libtests.a

LDADD = ../cudd_facade/libcudd_facade.a \
			  ../src/libsfta.a \
				$(TEST_LIBRARY)

lib_LIBRARIES = $(TEST_LIBRARY)

libtests_a_SOURCES = log_fixture.cc

bin_PROGRAMS = cudd_facade_test

cudd_facade_test_SOURCES = cudd_facade_test.cc

