## Process this file with automake to produce Makefile.in

TEST_LIBRARY = libtests.a

LDADD = ../cudd_facade/libcudd_facade.a \
			  ../src/libsfta.a \
				$(TEST_LIBRARY)

lib_LIBRARIES = $(TEST_LIBRARY)

libtests_a_SOURCES = log_fixture.cc

check_PROGRAMS = cudd_facade_test cudd_shared_mtbdd_cc_test cudd_shared_mtbdd_uv_test

cudd_facade_test_SOURCES = cudd_facade_test.cc

cudd_shared_mtbdd_cc_test_SOURCES = cudd_shared_mtbdd_cc_test.cc

cudd_shared_mtbdd_uv_test_SOURCES = cudd_shared_mtbdd_uv_test.cc

TESTS = $(check_PROGRAMS)
