#include <knihovna/cudd_facade.hh>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CUDDFacade
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(constructors)
{
	Knihovna::Private::CUDDFacade cudd;
}

BOOST_AUTO_TEST_CASE(setters_and_getters_test)
{
}

BOOST_AUTO_TEST_CASE(serialization)
{
}
