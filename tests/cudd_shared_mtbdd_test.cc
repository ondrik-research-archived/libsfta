#include <knihovna/cudd_shared_mtbdd.hh>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CuddSharedMTBDD
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(constructors)
{
	Knihovna::CUDDSharedMTBDD<unsigned, unsigned, unsigned, unsigned> c();
}

BOOST_AUTO_TEST_CASE(setters_and_getters_test)
{

	BOOST_CHECK(true);
	BOOST_CHECK(1 == 2);
}

BOOST_AUTO_TEST_CASE(serialization)
{
}
