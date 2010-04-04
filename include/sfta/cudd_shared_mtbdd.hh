/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for CUDDSharedMTBDD class.
 *
 *****************************************************************************/

#ifndef _CUDD_SHARED_MTBDD_HH_
#define _CUDD_SHARED_MTBDD_HH_

// Standard library headers
#include <cassert>

// SFTA headers
#include <sfta/abstract_shared_mtbdd.hh>


namespace SFTA
{
	template <typename RootType,
		class VariableAssignmentType,
		class LeafAllocator>
	class CUDDSharedMTBDD;
}


/**
 * @brief   Class implementing CUDD-based shared multi-terminal BDD
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Class that implements shared multi-terminal BDD based on CUDD package
 * (namely using the SFTA::Private::CUDDFacade interface). The class properly
 * manages its resources. 
 *
 * @see  SFTA::Private::CUDDFacade
 *
 * @tparam  RootType                The type for the root of a MTBDD. Is used
 *                                  to reference the root.
 * @tparam  VariableAssignmentType  The type that is used for representation
 *                                  of Boolean variable assignment, i.e.
 *                                  representation of a path in the BDD.
 * @tparam  LeafAllocator           The allocator that is used to allocate
 *                                  memory for leafs and (in case it is
 *                                  necessary) provide binding between the
 *                                  leafs and handles stored in the MTBDD. The
 *                                  allocator should behave in such a way,
 *                                  that for leafs that are equal should
 *                                  generate handles that are also equal.
 */
template <typename RootType,
	class VariableAssignmentType,
	class LeafAllocator>
class SFTA::CUDDSharedMTBDD
	: public AbstractSharedMTBDD<RootType,
		VariableAssignmentType,
		LeafAllocator>
{
public:   // Public data types

	/**
	 * @brief  The type of the parent class
	 *
	 * The data type of the parent class with correct template instantiation.
	 */
	typedef AbstractSharedMTBDD<RootType,
		VariableAssignmentType,
		LeafAllocator>
	ParentClass;


private:  // Private data members

	int smeti_;


public:   // Public methods

	/**
	 * @brief  Constructor
	 *
	 * The constructor of CUDDSharedMTBDD.
	 */
	CUDDSharedMTBDD() : smeti_(0)
	{
		//assert(false);

	}


	virtual void SetValue(const RootType& /*root*/, const VariableAssignmentType& /*position*/, const typename LeafAllocator::LeafType& /*value*/)
	{
	}


	virtual typename LeafAllocator::LeafType& GetValue(const RootType& /*root*/, const VariableAssignmentType& /*position*/)
	{
		return reinterpret_cast<typename LeafAllocator::LeafType&>(smeti_);
	}


	virtual RootType Apply(const RootType& /*lhs*/, const RootType& /*rhs*/, const ApplyFunctionType& /*func*/)
	{
		return RootType();
	}


	virtual RootType CreateRoot()
	{
		return RootType();
	}


	virtual string Serialize() const
	{
		return "";
	}


	virtual ~CUDDSharedMTBDD()
	{
	}
};

#endif
