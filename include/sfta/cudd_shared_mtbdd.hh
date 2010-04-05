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
#include <sfta/cudd_facade.hh>


namespace SFTA
{
	template
	<
		typename RootType,
		typename LeafType,
		class VariableAssignmentType,
		template <typename, typename> class LeafAllocator,
		template <typename, typename> class RootAllocator
	>
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
 * @tparam  LeafType                The type of a leaf of a MTBDD.
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
 * @tparam  RootAllocator           The allocator for root nodes of MTBDDs.
 */
template
<
	typename RootType,
	typename LeafType,
	class VariableAssignmentType,
	template <typename, typename> class LeafAllocator,
	template <typename, typename> class RootAllocator>
class SFTA::CUDDSharedMTBDD
	: public AbstractSharedMTBDD
		<
			RootType,
			LeafType,
			VariableAssignmentType
		>,
		protected LeafAllocator
		<
			LeafType,
			SFTA::Private::CUDDFacade::ValueType
		>,
		protected RootAllocator
		<
			RootType,
			SFTA::Private::CUDDFacade::Node*
		>
{
private:   // Private data types

	/**
	 * @brief  The type of the parent class
	 *
	 * The data type of the parent class with correct template instantiation.
	 */
	typedef AbstractSharedMTBDD
	<
		RootType,
		LeafType,
		VariableAssignmentType
	>
	ParentClass;


	/**
	 * @brief  The type of the leaf allocator
	 *
	 * The type of the leaf allocator class with correct template instantiation.
	 */
	typedef LeafAllocator
	<
		LeafType,
		SFTA::Private::CUDDFacade::ValueType
	>
	LA;


	/**
	 * @brief  The type of the root allocator
	 *
	 * The type of the root allocator class with correct template instantiation.
	 */
	typedef RootAllocator
	<
		RootType,
		SFTA::Private::CUDDFacade::Node*
	>
	RA;


	/**
	 * @brief  The type of the CUDD facade
	 *
	 * The type of the CUDD facade.
	 */
	typedef SFTA::Private::CUDDFacade CUDDFacade;


private:  // Private data members

	/**
	 * @brief  Interface to CUDD
	 *
	 * Interface to CUDD. Object of this class encapsulates one CUDD manager and
	 * its associated MTBDDs.
	 *
	 * @see SFTA::Private::CUDDFacade
	 */
	SFTA::Private::CUDDFacade cudd;


public:   // Public methods

	/**
	 * @brief  Constructor
	 *
	 * The constructor of CUDDSharedMTBDD.
	 */
	CUDDSharedMTBDD() : cudd()
	{ }


	virtual void SetValue(const RootType& /*root*/, const VariableAssignmentType& /*position*/, const LeafType& /*value*/)
	{
	}


	virtual LeafType& GetValue(const RootType& /*root*/, const VariableAssignmentType& /*position*/)
	{
		static int smeti;
		return reinterpret_cast<LeafType&>(smeti);
	}


	virtual RootType Apply(const RootType& /*lhs*/, const RootType& /*rhs*/, const typename ParentClass::ApplyFunctionType& /*func*/)
	{
		return RootType();
	}


	virtual RootType CreateRoot()
	{
		CUDDFacade::Node* node = cudd.ReadBackground();
		cudd.Ref(node);

		return RA::AllocateRoot(node);
	}


	virtual std::string Serialize() const
	{
		return "";
	}


	virtual ~CUDDSharedMTBDD()
	{
		for (typename RA::Iterator it = RA::Begin(); it != RA::End(); ++it)
		{	// traverse all roots
			cudd.RecursiveDeref(*it);
		}
	}
};

#endif
