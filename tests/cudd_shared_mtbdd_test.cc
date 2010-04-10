/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Test suite for CUDDSharedMTBDD class.
 *
 *****************************************************************************/

// SFTA headers
#include <sfta/cudd_shared_mtbdd.hh>
using SFTA::AbstractSharedMTBDD;
using SFTA::CUDDSharedMTBDD;

// Boost headers
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CUDDSharedMTBDD
#include <boost/test/unit_test.hpp>

// testing headers
#include "log_fixture.hh"


/******************************************************************************
 *                                User classes                                *
 ******************************************************************************/

template
<
	typename Leaf,
	typename Handle
>
struct MyLeafAllocator
{
public:
	typedef Leaf LeafType;
	typedef Handle HandleType;

private:

	typedef std::map<HandleType, LeafType> LeafContainer;

	LeafContainer asoc_arr_;

	HandleType next_index_;

protected:

	MyLeafAllocator()
		: asoc_arr_(), next_index_(0)
	{
	}

	HandleType createLeaf(const LeafType& leaf)
	{
		asoc_arr_[next_index_] = leaf;
		++next_index_;
		return next_index_ - 1;
	}

	LeafType& getLeafOfHandle(const HandleType& handle)
	{
		// try to find given leaf
		typename LeafContainer::iterator it = asoc_arr_.find(handle);
		if (it == asoc_arr_.end())
		{	// in case it couldn't be found
			throw std::runtime_error("Trying to access leaf \""
				+ SFTA::Private::Convert::ToString(handle) + "\" that is not managed.");
		}

		return it->second;
	}

	const LeafType& getLeafOfHandle(const HandleType& handle) const
	{
		// try to find given leaf
		typename LeafContainer::const_iterator it = asoc_arr_.find(handle);
		if (it == asoc_arr_.end())
		{	// in case it couldn't be found
			throw std::runtime_error("Trying to access leaf \""
				+ SFTA::Private::Convert::ToString(handle) + "\" that is not managed.");
		}

		return it->second;
	}

	std::vector<HandleType> getAllHandles() const
	{
		std::vector<HandleType> result;

		for (typename std::map<HandleType, LeafType>::const_iterator it = asoc_arr_.begin(); it != asoc_arr_.end(); ++it)
		{
			result.push_back(it->first);
		}

		return result;
	}

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
public:
	typedef Handle HandleType;
	typedef unsigned RootType;

private:

	typedef std::map<unsigned, HandleType> HandleContainer;

public:
	typedef typename HandleContainer::iterator Iterator;

private:

	HandleContainer arr_;
	size_t next_index_;

protected:

	MyRootAllocator() : arr_(), next_index_(0)
	{
	}

	unsigned allocateRoot(const HandleType& handle)
	{
		arr_[next_index_] = handle;
		++next_index_;
		return next_index_ - 1;
	}

	const HandleType& getHandleOfRoot(unsigned root) const
	{
		// try to find given root
		typename HandleContainer::const_iterator it = arr_.find(root);
		if (it == arr_.end())
		{	// in case it couldn't be found
			assert(false);
			throw std::runtime_error("Trying to access root \""
				+ SFTA::Private::Convert::ToString(root) + "\" that is not managed.");
		}

		return it->second;
	}

	void changeHandleOfRoot(unsigned root, const HandleType& handle)
	{
		// try to find given root
		typename HandleContainer::iterator it = arr_.find(root);
		if (it == arr_.end())
		{	// in case it couldn't be found
			throw std::runtime_error("Trying to change value of root \""
				+ SFTA::Private::Convert::ToString(root) + "\" that is not managed.");
		}

		it->second = handle;
	}

	std::vector<unsigned> getAllRoots() const
	{
		std::vector<RootType> res(0);

		for (typename HandleContainer::const_iterator it = arr_.begin();
			it != arr_.end(); ++it)
		{
			res.push_back(it->first);
		}

		return res;
	}

	std::vector<HandleType> getAllRootHandles() const
	{
		std::vector<HandleType> res(0);

		for (typename HandleContainer::const_iterator it = arr_.begin();
			it != arr_.end(); ++it)
		{
			res.push_back(it->second);
		}

		return res;
	}

	void eraseRoot(unsigned root)
	{
		// try to find given root
		typename HandleContainer::iterator it = arr_.find(root);
		if (it == arr_.end())
		{	// in case it couldn't be found
			throw std::runtime_error("Trying to erase root \""
				+ SFTA::Private::Convert::ToString(root) + "\" that is not managed.");
		}

		arr_.erase(it);
	}

	~MyRootAllocator() { }

public:

	const static size_t INITIAL_VECTOR_SIZE = 8;
};


struct MyVariableAssignment
{
private:

	unsigned vars_;

public:
	enum
	{
		ONE,
		ZERO,
		DONT_CARE
	};

	MyVariableAssignment(unsigned value)
		: vars_(value)
	{}


	inline int GetIthVariableValue(unsigned i) const
	{
		if (!((vars_ >> i) & 1))
		{
			return ZERO;
		}
		else
		{
			return ONE;
		}
	}

	size_t Size() const
	{
		//return 8*sizeof(unsigned);
		return 8;
	}

	std::string ToString() const
	{
		return SFTA::Private::Convert::ToString(vars_);
	}

	friend std::ostream& operator<<(std::ostream& os, const MyVariableAssignment asgn)
	{
		return (os << asgn.ToString());
	}
};


/******************************************************************************
 *                                  Fixtures                                  *
 ******************************************************************************/

class CUDDSharedMTBDDFixture : public LogFixture
{
private:

	CUDDSharedMTBDDFixture(const CUDDSharedMTBDDFixture& fixture);
	CUDDSharedMTBDDFixture& operator=(const CUDDSharedMTBDDFixture& rhs);

protected:

	typedef AbstractSharedMTBDD<unsigned, unsigned, MyVariableAssignment> ASMTBDD;

	ASMTBDD* mtbdd;

public:

	CUDDSharedMTBDDFixture()
		: mtbdd(static_cast<ASMTBDD*>(0))
	{
		mtbdd = new SFTA::CUDDSharedMTBDD<unsigned, unsigned, MyVariableAssignment,
			MyLeafAllocator, MyRootAllocator>();
	}

	~CUDDSharedMTBDDFixture()
	{
		delete mtbdd;
	}

};


/******************************************************************************
 *                              Start of testing                              *
 ******************************************************************************/

BOOST_FIXTURE_TEST_SUITE(suite, CUDDSharedMTBDDFixture)

BOOST_AUTO_TEST_CASE(setters_and_getters_test)
{
	unsigned root = mtbdd->CreateRoot();

	unsigned value_added = 7;

	MyVariableAssignment asgn(5);
	mtbdd->SetValue(root, asgn, value_added);

	unsigned value_read = mtbdd->GetValue(root, asgn);


	BOOST_CHECK(value_added == value_read);

	value_added = 15;
	asgn = 3;
	mtbdd->SetValue(root, asgn, value_added);
	mtbdd->DumpToDotFile("pokus.dot");
	value_read = mtbdd->GetValue(root, asgn);

	BOOST_CHECK(value_added == value_read);

	value_added = 7;
	asgn = 5;
	value_read = mtbdd->GetValue(root, asgn);
	BOOST_CHECK(value_added == value_read);
}

BOOST_AUTO_TEST_CASE(serialization)
{
}

BOOST_AUTO_TEST_SUITE_END()
