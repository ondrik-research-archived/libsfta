#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CUDDSharedMTBDD
#include <boost/test/unit_test.hpp>

#include <sfta/cudd_shared_mtbdd.hh>

template
<
	typename Leaf,
	typename Handle
>
struct MyLeafAllocator
{
public:
	typedef Leaf LeafType;
protected:
	~MyLeafAllocator() { }
};

template
<
	typename Root,
	typename Handle
>
struct MyRootAllocator;

template
<
	typename Handle
>
struct MyRootAllocator<unsigned, Handle>
{
private:
	std::vector<Handle> arr;
	size_t next_index;
protected:

	MyRootAllocator() : arr(INITIAL_VECTOR_SIZE), next_index(0)
	{
	}

	unsigned CreateRoot(Handle& handle)
	{
		if (next_index >= arr.size())
		{
			arr.resize(2 * arr.size());
		}

		arr[next_index] = handle;
		return next_index++;
	}

	~MyRootAllocator() { }

public:

	const static size_t INITIAL_VECTOR_SIZE = 8;
};


BOOST_AUTO_TEST_CASE(constructors)
{
	SFTA::AbstractSharedMTBDD<unsigned, unsigned, unsigned>* c = new SFTA::CUDDSharedMTBDD<unsigned, unsigned, unsigned, MyLeafAllocator, MyRootAllocator>();
	c->SetValue(0,1,2);
}

BOOST_AUTO_TEST_CASE(setters_and_getters_test)
{

}

BOOST_AUTO_TEST_CASE(serialization)
{
}
