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
#include <algorithm>

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

	/**
	 * @brief  Type of a leaf
	 *
	 * Type of a leaf of MTBDD.
	 */
	typedef Leaf LeafType;


	/**
	 * @brief  Type of a root
	 *
	 * Type of a root of MTBDD.
	 */
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


	/**
	 * @brief  The type of the Convert class
	 *
	 * The type of the Convert class.
	 */
	typedef SFTA::Private::Convert Convert;


	/**
	 * @brief  The type for an array of variables
	 *
	 * The type for storage of an array of variables.
	 */
	typedef std::vector<CUDDFacade::Node*> VariableArrayType;


	/**
	 * @brief  The type for an array of roots
	 *
	 * The type that represents an array of roots.
	 */
	typedef std::vector<typename RA::RootType> RootArray;


	/**
	 * @brief  The type for an array of root handles
	 *
	 * The type that represents an array of root handles.
	 */
	typedef std::vector<typename RA::HandleType> RootHandleArray;


	/**
	 * @brief  The type for an array of leaf handles
	 *
	 * The type that represents an array of leaf handles.
	 */
	typedef std::vector<typename LA::HandleType> LeafHandleArray;


	/**
	 * @brief  Wrapper for Apply callback function parameters
	 *
	 * This structure is a wrapper that contains all necessary parameters that
	 * are needed for the Apply callback to work. In fact, it serves as an
	 * envelope that we send to ourselves.
	 */
	struct ApplyCallbackParameters
	{
	public:   // Public data members

		/**
		 * @brief  Pointer to the operation
		 *
		 * Pointer to the function that carries out the operation on leave nodes of
		 * the MTBDD.
		 */
		typename ParentClass::ApplyFunctionType Op;


		/**
		 * @brief  MTBDD that provides the context
		 *
		 * MTBDD which is used to provide the context, in which the operation
		 * takes place.
		 */
		CUDDSharedMTBDD* SharedMTBDD;

	public:   // Public methods

		/**
		 * @brief  Constructor
		 *
		 * The constructor of the structure.
		 */
		ApplyCallbackParameters(typename ParentClass::ApplyFunctionType op,
			CUDDSharedMTBDD* bdd)
			: Op(op), SharedMTBDD(bdd)
		{ }

	};


	/**
	 * @brief   Static class with apply functions for MTBDD
	 *
	 * Static class that is used to provide a separate namespace for apply
	 * functions for MTBDD. Note that a namespace cannot be declared inside
	 * a class in C++, that is why a static class is used.
	 */
	class ApplyFunctions
	{
	private:

		// private constructors etc. that disable creating an instance of the
		// class
		ApplyFunctions();
		ApplyFunctions(const ApplyFunctions&);
		ApplyFunctions& operator=(const ApplyFunctions&);
		~ApplyFunctions();


	public:

		/**
		 * @brief  Overwrites not-0 nodes with their right equivalent
		 *
		 * This is an Apply operation that substitutes each sink node in the
		 * left-hand side MTBDD by the respective sink node in the right-hand side
		 * MTBDD in case it is not bottom.
		 *
		 * @see  SFTA::Private::CUDDFacade::Apply()
		 *
		 * @param[in]  lhs   Left-hand side MTBDD sink node
		 * @param[in]  rhs   Right-hand side MTBDD sink node
		 * @param[in]  data  Pointer to data (not used, pass 0 pointer)
		 *
		 * @returns  Sink node after the operation
		 */
		static typename LA::HandleType overwriteByRight(
			const typename LA::HandleType& lhs, const typename LA::HandleType& rhs,
			void* data)
		{
			// Assertions
			assert(data == data);  // just to make the compiler happy

			return (rhs == BOTTOM)? lhs : rhs;
		}


		/**
		 * @brief  Creates a projection according to the right MTBDD
		 *
		 * This is an Apply operation that creates a projection according to the
		 * right-hand side MTBDD. Places where there is a zero in the right-hand
		 * side MTBDD are removed (that is bottom is assigned to them).
		 *
		 * @see  SFTA::Private::CUDDFacade::Apply()
		 *
		 * @param[in]  lhs   Left-hand side MTBDD sink node
		 * @param[in]  rhs   Right-hand side MTBDD sink node
		 * @param[in]  data  Pointer to data (not used)
		 *
		 * @returns  Sink node after the operation
		 */
		static typename LA::HandleType projectByRight(
			const typename LA::HandleType& lhs, const typename LA::HandleType& rhs,
			void* data)
		{
			// Assertions
			assert(data == data);  // just to make the compiler happy

			return (rhs == BOTTOM)? BOTTOM : lhs;
		}


		/**
		 * @brief  Collects values from the leaves of a MTBDD
		 *
		 * This is a monadic Apply operation that collects non-bottom values from
		 * all leaves into a container.
		 *
		 * @see  SFTA::Private::CUDDFacade::MonadicApply()
		 *
		 * @param[in]  node  MTBDD's sink node
		 * @param[in]  data  Pointer to SFTA::CUDDSharedMTBDD::LeafHandleArray
		 *
		 * @returns  Unchanged node value
		 */
		static typename LA::HandleType collectLeaves(
			const typename LA::HandleType& node, void* data)
		{
			// Assertions
			assert(static_cast<LeafHandleArray*>(data)
				!= static_cast<LeafHandleArray*>(0));

			// cast the data to proper data type
			LeafHandleArray& leaves = *(static_cast<LeafHandleArray*>(data));

			if (node != BOTTOM)
			{	// in case we haven't hit the bottom
				leaves.push_back(node);
			}

			return node;
		}


		/**
		 * @brief  Performs generic Apply operation
		 *
		 * This is an Apply operation that performs generic Apply operation on two
		 * MTBDDs. Which Apply operation to be performed is given by the @c data
		 * parameter, which contains ApplyCallbackParameters with pointer to the
		 * function and the MTBDD which is to serve as the context for the
		 * operation.
		 *
		 * @see  ApplyCallbackParameters
		 * @see  Apply()
		 *
		 * @param[in]  lhs   Left-hand side MTBDD sink node
		 * @param[in]  rhs   Right-hand side MTBDD sink node
		 * @param[in]  data  Pointer to ApplyCallbackParameters
		 *
		 * @returns  Result of the operation
		 */
		static typename LA::HandleType genericApply(
			const typename LA::HandleType& lhs,
			const typename LA::HandleType& rhs, void* data)
		{
			// Assertions
			assert(static_cast<ApplyCallbackParameters*>(data)
				!= static_cast<ApplyCallbackParameters*>(0));

			// extract low-level callback parameters
			ApplyCallbackParameters& params =
				*(static_cast<ApplyCallbackParameters*>(data));

			// get the MTBDD
			CUDDSharedMTBDD& mtbdd = *(params.SharedMTBDD);

			SFTA_LOGGER_DEBUG("Performing generic apply on "
				+ Convert::ToString(mtbdd.LA::getLeafOfHandle(lhs))
				+ " and " + Convert::ToString(mtbdd.LA::getLeafOfHandle(rhs)));

			// perform the operation
			typename LA::LeafType res = params.Op(
				mtbdd.LA::getLeafOfHandle(lhs), mtbdd.LA::getLeafOfHandle(rhs));

			// create a leaf and return its handle
			return mtbdd.LA::createLeaf(res);
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
	CUDDFacade cudd_;


	/**
	 * @brief  The name of the Log4cpp category for logging
	 *
	 * The name of the Log4cpp category used for logging messages from this
	 * class.
	 */
	static const char* LOG_CATEGORY_NAME;


	/**
	 * @brief  Array of variables
	 *
	 * An array that stores saved variables so that they don't need be always
	 * recreated.
	 */
	VariableArrayType varArray_;


	/**
	 * @brief  Array of negated variables
	 *
	 * An array that stores saved negated variables so that they don't need be
	 * always recreated.
	 */
	VariableArrayType varArrayNot_;

	/**
	 * @brief  The value of the bottom of the MTBDD
	 *
	 * The value which denotes the @em bottom of the MTBDD.
	 */
	static const typename LA::HandleType BOTTOM;


private:  // Private methods

	/**
	 * @brief  Gets the i-th variable
	 *
	 * Returns the @f$i@f$-th variable of the MTBDD.
	 *
	 * @see  getIthVariableNot()
	 *
	 * @param[in]  i  The index of the variable
	 *
	 * @returns  The variable
	 */
	CUDDFacade::Node* getIthVariable(unsigned i)
	{
		while (i >= varArray_.size())
		{	// while we do not have large enough collection of variables
			CUDDFacade::Node* node = cudd_.AddIthVar(i);
			cudd_.Ref(node);
			varArray_.push_back(node);
		}

		return varArray_[i];
	}


	/**
	 * @brief  Gets complement of the i-th variable
	 *
	 * Returns complement of the @f$i@f$-th variable of the MTBDD.
	 *
	 * @see  getIthVariable()
	 *
	 * @param[in]  i  The index of the variable
	 *
	 * @returns  Complement of the variable
	 */
	CUDDFacade::Node* getIthVariableNot(unsigned i)
	{
		while (i >= varArrayNot_.size())
		{	// while we do not have large enough collection of negated variables

			// get proper variable
			CUDDFacade::Node* node = getIthVariable(i);
			// and complement it
			node = cudd_.AddCmpl(node);
			cudd_.Ref(node);
			varArrayNot_.push_back(node);
		}

		return varArrayNot_[i];
	}


	/**
	 * @brief  Creates a new MTBDD for a variable assignment
	 *
	 * Creates a new MTBDD in the shared MTBDD for given variable assignment
	 * @c vars = @f$(x_1, x_2, \dots, x_n)@f$ equal to given value and returns
	 * the root of the MTBDD.
	 *
	 * @param[in]  vars   Variable assignment
	 * @param[in]  value  Value for given variable assignment
	 *
	 * @returns  Root of created MTBDD
	 */
	RootType createMTBDDForVariableAssignment(
		const VariableAssignmentType& vars, const LeafType& value)
	{
		CUDDFacade::ValueType leaf = LA::createLeaf(value);
		CUDDFacade::Node* node = cudd_.AddConst(leaf);
		cudd_.Ref(node);

		for (unsigned i = 0; i < vars.Size(); ++i)
		{	// for all variables
			CUDDFacade::Node* var = static_cast<CUDDFacade::Node*>(0);
			if (vars.GetIthVariableValue(i) == VariableAssignmentType::ONE)
			{	// in case the variable has value '1'
				var = getIthVariable(i);
			}
			else if (vars.GetIthVariableValue(i) == VariableAssignmentType::ZERO)
			{	// in case the variable has value '0'
				var = getIthVariableNot(i);
			}
			else if (vars.GetIthVariableValue(i)
				== VariableAssignmentType::DONT_CARE)
			{	// in case the variable is not in the assignment
				continue;
			}
			else
			{	// in case something else occured
				throw std::runtime_error("Invalid variable assignment type returned "
					"by VariableAssignmentType::GetIthVariableValue()!");
			}

			CUDDFacade::Node* tmp = cudd_.Times(node, var);
			cudd_.Ref(tmp);
			cudd_.RecursiveDeref(node);
			node = tmp;
		}

		return RA::allocateRoot(node);
	}


	/**
	 * @brief  Creates a projection MTBDD for a variable assignment
	 *
	 * Creates a new MTBDD in the shared MTBDD for a projection according to
	 * given variable assignment @c vars = @f$(x_1, x_2, \dots, x_n)@f$. This
	 * MTBDD has value @c 1 in sink nodes which are to be part of the
	 * projection.
	 *
	 * @param[in]  vars   Variable assignment
	 *
	 * @returns  Root of projection MTBDD
	 */
	RootType createMTBDDForVariableProjection(const VariableAssignmentType& vars)
	{
		CUDDFacade::Node* node = cudd_.AddConst(1);
		cudd_.Ref(node);

		for (unsigned i = 0; i < vars.Size(); ++i)
		{	// for all variables
			CUDDFacade::Node* var = static_cast<CUDDFacade::Node*>(0);
			if (vars.GetIthVariableValue(i) == VariableAssignmentType::ONE)
			{	// in case the variable has value '1'
				var = getIthVariable(i);
			}
			else if (vars.GetIthVariableValue(i) == VariableAssignmentType::ZERO)
			{	// in case the variable has value '0'
				var = getIthVariableNot(i);
			}
			else if (vars.GetIthVariableValue(i)
				== VariableAssignmentType::DONT_CARE)
			{	// in case the variable is not in the assignment
				continue;
			}
			else
			{	// in case something else occured
				throw std::runtime_error("Invalid variable assignment type returned "
					"by VariableAssignmentType::GetIthVariableValue()!");
			}

			CUDDFacade::Node* tmp = cudd_.Times(node, var);
			cudd_.Ref(tmp);
			cudd_.RecursiveDeref(node);
			node = tmp;
		}

		return RA::allocateRoot(node);
	}


	/**
	 * @brief  Erases a root
	 *
	 * Erases given root and dereferences proper MTBDD
	 *
	 * @param[in]  root  The root of the MTBDD to be erased
	 */
	void eraseRoot(RootType root)
	{
		// First, for every leaf, call proper release function

		// create parameters of monadic Apply function that pass correct callback
		// function and this object to provide context
		CUDDFacade::MonadicApplyCallbackParameters paramsMon(
			LA::leafReleaser, static_cast<void*>(this));
		// carry out the monadic Apply operation
		CUDDFacade::Node* monRes = cudd_.MonadicApply(RA::getHandleOfRoot(root),
			&paramsMon);

		// remove temporary MTBDDs
		cudd_.RecursiveDeref(monRes);

		cudd_.RecursiveDeref(RA::getHandleOfRoot(root));
		RA::eraseRoot(root);
	}


public:   // Public methods

	/**
	 * @brief  Constructor
	 *
	 * The constructor of CUDDSharedMTBDD.
	 */
	CUDDSharedMTBDD() : cudd_(), varArray_(0), varArrayNot_(0)
	{
		// set the bottom
		LA::setBottom(LeafType());
	}


	/**
	 * @brief  Sets the value of a sink node
	 *
	 * This method sets the value of a sink node of given MTBDD at variable
	 * assignment denoted by @c asgn to given value.
	 *
	 * @see  GetValue()
	 *
	 * @param[in]  root   Root of the MTBDD in which the value is to be changed
	 * @param[in]  asgn   Variable assignment determining the sink node
	 * @param[in]  value  Value to which the sink node is to be changed
	 */
	virtual void SetValue(const RootType& root,
		const VariableAssignmentType& asgn, const LeafType& value)
	{
		SFTA_LOGGER_DEBUG("Setting value at " + Convert::ToString(asgn)
			+ " to " + Convert::ToString(value));

		// create the MTBDD with given value at given position
		RootType mtbddAsgn = createMTBDDForVariableAssignment(asgn, value);
		// create parameters of Apply function that pass correct callback function
		CUDDFacade::ApplyCallbackParameters params(
			ApplyFunctions::overwriteByRight, static_cast<void*>(this));
		// carry out the Apply operation
		CUDDFacade::Node* res = cudd_.Apply(RA::getHandleOfRoot(root),
			RA::getHandleOfRoot(mtbddAsgn), &params);

		// remove the temporary MTBDD
		eraseRoot(mtbddAsgn);

		// get rid of the old MTBDD for the function
		cudd_.RecursiveDeref(RA::getHandleOfRoot(root));

		// substitute the new MTBDD for the old one
		RA::changeHandleOfRoot(root, res);
	}


	virtual typename ParentClass::LeafContainer GetValue(const RootType& root,
		const VariableAssignmentType& asgn)
	{
		SFTA_LOGGER_DEBUG("Reading value at " + Convert::ToString(asgn));

		// create the projection MTBDD for given variable assignment
		RootType mtbddAsgn = createMTBDDForVariableProjection(asgn);
		// create parameters of Apply function that pass correct callback function
		CUDDFacade::ApplyCallbackParameters params(
			ApplyFunctions::projectByRight, static_cast<void*>(this));
		// carry out the Apply operation
		CUDDFacade::Node* res = cudd_.Apply(RA::getHandleOfRoot(root),
			RA::getHandleOfRoot(mtbddAsgn), &params);

		// remove the temporary MTBDD
		eraseRoot(mtbddAsgn);

		// create container for handles of leaves that are at the position
		LeafHandleArray leavesHandles(0);
		// create parameters of monadic Apply function that pass correct callback
		// function and container for handles of leaves
		CUDDFacade::MonadicApplyCallbackParameters paramsMon(
			ApplyFunctions::collectLeaves, static_cast<void*>(&leavesHandles));
		// carry out the monadic Apply operation
		CUDDFacade::Node* monRes = cudd_.MonadicApply(res, &paramsMon);

		// remove temporary MTBDDs
		cudd_.RecursiveDeref(res);
		cudd_.RecursiveDeref(monRes);

		typename ParentClass::LeafContainer leaves(leavesHandles.size());
		for (unsigned i = 0; i < leavesHandles.size(); ++i)
		{	// for each leaf handle
			leaves[i] = &LA::getLeafOfHandle(leavesHandles[i]);
		}

		return leaves;
	}


	/**
	 * @brief  \copybrief  SFTA::AbstractSharedMTBDD::Apply()
	 *
	 * \copydetails  SFTA::AbstractSharedMTBDD::Apply()
	 */
	virtual RootType Apply(const RootType& lhs, const RootType& rhs, const typename ParentClass::ApplyFunctionType& func)
	{
		// Assertions
		assert(func != static_cast<typename ParentClass::ApplyFunctionType>(0));

		// create parameters of high-level Apply function that pass correct
		// callback function and this object as the context
		ApplyCallbackParameters params(func, this);
		// create parameters of low-level Apply function that pass correct
		// callback function and abovementioned parameters for the high-level
		// Apply function
		CUDDFacade::ApplyCallbackParameters cuddFacadeParams(
			ApplyFunctions::genericApply, static_cast<void*>(&params));
		// carry out the Apply operation
		CUDDFacade::Node* res = cudd_.Apply(RA::getHandleOfRoot(lhs),
			RA::getHandleOfRoot(rhs), &cuddFacadeParams);

		return RA::allocateRoot(res);
	}


	virtual RootType CreateRoot()
	{
		CUDDFacade::Node* node = cudd_.ReadBackground();
		cudd_.Ref(node);

		return RA::allocateRoot(node);
	}


	virtual std::string Serialize() const
	{
		// TODO:
		return "";
	}


	virtual void DumpToDotFile(const std::string& filename) const
	{
		// the array of roots
		RootArray roots = RA::getAllRoots();
		// the array of root nodes
		std::vector<CUDDFacade::Node*> nodes(roots.size());
		// the array of root names
		std::vector<std::string> rootNames(roots.size());

		for (unsigned i = 0; i < roots.size(); ++i)
		{	// insert all root nodes and respective names
			nodes[i] = RA::getHandleOfRoot(roots[i]);
			rootNames[i] = Convert::ToString(roots[i]);
		}

		// array of sink nodes
		std::vector<CUDDFacade::ValueType> sinks = LA::getAllHandles();
		// find the maximum element of sinks
		CUDDFacade::ValueType maxSink =
			*std::max_element(sinks.begin(), sinks.end());
		// array of sink node names
		std::vector<std::string> sinkNames(maxSink + 1);

		for (unsigned i = 0; i < sinks.size(); ++i)
		{	// insert all sink nodes' names
			sinkNames[sinks[i]] = Convert::ToString(sinks[i]) + " : " + Convert::ToString(LA::getLeafOfHandle(sinks[i]));
		}

		// create the Dot file
		cudd_.DumpDot(nodes, rootNames, sinkNames, filename);
	}


	virtual ~CUDDSharedMTBDD()
	{
		RootHandleArray roots = RA::getAllRootHandles();
		for (typename RootHandleArray::const_iterator it = roots.begin();
			it != roots.end(); ++it)
		{	// traverse all roots
			cudd_.RecursiveDeref(*it);
		}

		for (VariableArrayType::iterator it = varArray_.begin();
			it != varArray_.end(); ++it)
		{	// traverse all variables and dereference them
			cudd_.RecursiveDeref(*it);
		}

		for (VariableArrayType::iterator it = varArrayNot_.begin();
			it != varArrayNot_.end(); ++it)
		{	// traverse all negated variables and dereference them
			cudd_.RecursiveDeref(*it);
		}
	}
};


// Setting the logging category name for Log4cpp
template
<
	typename R,
	typename L,
	class VAT,
	template <typename, typename> class LA,
	template <typename, typename> class RA
>
const char* SFTA::CUDDSharedMTBDD<R, L, VAT, LA, RA>::LOG_CATEGORY_NAME
	= "cudd_shared_mtbdd";


// Setting the bottom value
template
<
	typename R,
	typename L,
	class VAT,
	template <typename, typename> class LA,
	template <typename, typename> class RA
>
const typename SFTA::CUDDSharedMTBDD<R, L, VAT, LA, RA>::LA::HandleType
	SFTA::CUDDSharedMTBDD<R, L, VAT, LA, RA>::BOTTOM = 0;


#endif
