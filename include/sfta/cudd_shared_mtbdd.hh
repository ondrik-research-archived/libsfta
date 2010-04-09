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
#include <vector>

// SFTA headers
#include <sfta/sfta.hh>
#include <sfta/abstract_shared_mtbdd.hh>
#include <sfta/cudd_facade.hh>
#include <sfta/convert.hh>


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
	typename Root,
	typename Leaf,
	class VariableAssignmentType,
	template <typename, typename> class LeafAllocator,
	template <typename, typename> class RootAllocator
>
class SFTA::CUDDSharedMTBDD
	: public AbstractSharedMTBDD
		<
			Root,
			Leaf,
			VariableAssignmentType
		>,
		protected LeafAllocator
		<
			Leaf,
			SFTA::Private::CUDDFacade::ValueType
		>,
		protected RootAllocator
		<
			Root,
			SFTA::Private::CUDDFacade::Node*
		>
{
public:    // Public data types

	typedef Leaf LeafType;
	typedef Root RootType;

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


	class ApplyFunctions
	{
	public:
		static typename LA::HandleType overwriteByRight(typename LA::HandleType /*lhs*/, typename LA::HandleType rhs, void* /*data*/)
		{
			return rhs;
		}

		static typename LA::HandleType projectByRight(typename LA::HandleType lhs, typename LA::HandleType rhs, void* /*data*/)
		{
			if (rhs == 0)
			{
				return 0;
			}
			else
			{
				return lhs;
			}
		}

		static typename LA::HandleType collectLeaves(typename LA::HandleType node, void* data)
		{
			std::vector<unsigned>& vec = *(static_cast<std::vector<unsigned>*>(data));

			if (node != 0)
			{
				vec.push_back(node);
			}

			return node;
		}
	};


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

	static const char* LOG_CATEGORY_NAME;

private:  // Private methods

	RootType createMTBDDForVariableAssignment(const VariableAssignmentType& vars, const LeafType& value)
	{
		CUDDFacade::ValueType leaf = LA::createLeaf(value);
		CUDDFacade::Node* node = cudd.AddConst(leaf);
		cudd.Ref(node);
		for (unsigned i = 0; i < vars.Size(); ++i)
		{
			CUDDFacade::Node* var = cudd.AddIthVar(i);
			cudd.Ref(var);
			CUDDFacade::Node* tmp = cudd.Times(node, var);
			cudd.Ref(tmp);
			cudd.RecursiveDeref(node);
			cudd.RecursiveDeref(var);
			node = tmp;
		}

		return RA::allocateRoot(node);
	}

	RootType createMTBDDForVariableProjection(const VariableAssignmentType& vars)
	{
		CUDDFacade::Node* node = cudd.AddConst(1);
		cudd.Ref(node);
		for (unsigned i = 0; i < vars.Size(); ++i)
		{
			CUDDFacade::Node* var = cudd.AddIthVar(i);
			cudd.Ref(var);
			CUDDFacade::Node* tmp = cudd.Times(node, var);
			cudd.Ref(tmp);
			cudd.RecursiveDeref(node);
			cudd.RecursiveDeref(var);
			node = tmp;
		}

		return RA::allocateRoot(node);
	}

public:   // Public methods

	/**
	 * @brief  Constructor
	 *
	 * The constructor of CUDDSharedMTBDD.
	 */
	CUDDSharedMTBDD() : cudd()
	{
		// TODO: fix this
		LA::createLeaf(0);
	}


	virtual void SetValue(const RootType& root, const VariableAssignmentType& asgn, const LeafType& value)
	{
		SFTA_LOGGER_DEBUG("Setting value at " + SFTA::Private::Convert::ToString(asgn)
			+ " to " + SFTA::Private::Convert::ToString(value));

		RootType mtbddAsgn = createMTBDDForVariableAssignment(asgn, value);
		CUDDFacade::ApplyCallbackParameters params(ApplyFunctions::overwriteByRight, static_cast<void*>(this));
		CUDDFacade::Node* res = cudd.Apply(RA::getHandleOfRoot(root), RA::getHandleOfRoot(mtbddAsgn), &params);

		// get rid of the old MTBDD
		cudd.RecursiveDeref(RA::getHandleOfRoot(root));

		// substitute the new MTBDD for the old one
		RA::changeHandleOfRoot(root, res);
	}


	virtual LeafType& GetValue(const RootType& root, const VariableAssignmentType& asgn)
	{
		RootType mtbddAsgn = createMTBDDForVariableProjection(asgn);
		CUDDFacade::ApplyCallbackParameters params(ApplyFunctions::projectByRight, static_cast<void*>(this));
		CUDDFacade::Node* res = cudd.Apply(RA::getHandleOfRoot(root), RA::getHandleOfRoot(mtbddAsgn), &params);

		std::vector<unsigned> vec;
		CUDDFacade::MonadicApplyCallbackParameters paramsMon(ApplyFunctions::collectLeaves, static_cast<void*>(&vec));
		cudd.MonadicApply(res, &paramsMon);


		std::string pokus = "";
		for (unsigned i = 0; i < vec.size(); ++i)
		{
			pokus += SFTA::Private::Convert::ToString(LA::getLeafOfHandle(vec[i])) + "; ";
		}

		SFTA_LOGGER_DEBUG("Elements in vector: "
			+ SFTA::Private::Convert::ToString(vec.size()));
		SFTA_LOGGER_DEBUG("Values of elements in vector: " + pokus);

		assert(vec.size() > 0);

		return LA::getLeafOfHandle(vec[0]);
	}


	virtual RootType Apply(const RootType& /*lhs*/, const RootType& /*rhs*/, const typename ParentClass::ApplyFunctionType& /*func*/)
	{
		return RootType();
	}


	virtual RootType CreateRoot()
	{
		CUDDFacade::Node* node = cudd.ReadBackground();
		cudd.Ref(node);

		return RA::allocateRoot(node);
	}


	virtual std::string Serialize() const
	{
		return "";
	}


	virtual void DumpToDotFile(const std::string& filename)
	{
		std::vector<RootType> roots = RA::getAllRoots();

		std::vector<SFTA::Private::CUDDFacade::Node*> nodes(roots.size());
		std::vector<std::string> rootNames(roots.size());

		for (unsigned i = 0; i < roots.size(); ++i)
		{
			nodes[i] = RA::getHandleOfRoot(i);
		}

		for (unsigned i = 0; i < roots.size(); ++i)
		{
			rootNames[i] = SFTA::Private::Convert::ToString(i);
		}

		std::vector<SFTA::Private::CUDDFacade::ValueType> sinks = LA::getAllHandles();

		std::vector<std::string> sinkNames;
		for (unsigned i = 0; i < sinks.size(); ++i)
		{
			sinkNames.push_back(SFTA::Private::Convert::ToString(LA::getLeafOfHandle(sinks[i])));
		}

		cudd.DumpDot(nodes, rootNames, sinkNames, filename);
	}


	virtual ~CUDDSharedMTBDD()
	{
		for (typename RA::Iterator it = RA::begin(); it != RA::end(); ++it)
		{	// traverse all roots
			if (*it != static_cast<CUDDFacade::Node*>(0))
			{	// in case the root has not been deleted
				cudd.RecursiveDeref(*it);
			}
		}
	}
};


// Setting the logging category name for Log4cpp
template
<
	typename Root,
	typename Leaf,
	class VariableAssignmentType,
	template <typename, typename> class LeafAllocator,
	template <typename, typename> class RootAllocator
>
const char* SFTA::CUDDSharedMTBDD<Root, Leaf, VariableAssignmentType, LeafAllocator, RootAllocator>::LOG_CATEGORY_NAME = "cudd_shared_mtbdd";

#endif
