/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for CUDDSharedMTBDD class.
 *
 *****************************************************************************/

#ifndef _SFTA_CUDD_SHARED_MTBDD_HH_
#define _SFTA_CUDD_SHARED_MTBDD_HH_

// Standard library headers
#include <cassert>
#include <vector>
#include <algorithm>

// SFTA headers
#include <sfta/sfta.hh>
#include <sfta/abstract_shared_mtbdd.hh>
#include <sfta/cudd_facade.hh>
#include <sfta/convert.hh>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename Root,
		typename Leaf,
		class VariableAssignment,
		template <typename, typename, class> class LeafAllocator,
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
 * @tparam  Root                The type for the root of a MTBDD. Is used to
 *                              reference the root.
 * @tparam  Leaf                The type of a leaf of a MTBDD.
 * @tparam  VariableAssignment  The type that is used for representation of
 *                              Boolean variable assignment, i.e.
 *                              representation of a path in the BDD.
 * @tparam  LeafAllocator       The allocator that is used to allocate
 *                              leaves and (in case it is necessary) provide
 *                              binding between the leaves and handles stored
 *                              in the MTBDD. The allocator should behave in
 *                              such a way, that for leaves that are equal
 *                              should generate handles that are also equal.
 * @tparam  RootAllocator       The allocator for root nodes of MTBDDs.
 */
GCC_DIAG_OFF(effc++)
template
<
	typename Root,
	typename Leaf,
	class VariableAssignment,
	template <typename, typename, class> class LeafAllocator,
	template <typename, typename> class RootAllocator
>
class SFTA::CUDDSharedMTBDD
	: public AbstractSharedMTBDD
		<
			Root,
			Leaf,
			VariableAssignment
		>,
		protected LeafAllocator
		<
			Leaf,
			SFTA::Private::CUDDFacade::ValueType,
			SFTA::Private::CUDDFacade::AbstractMonadicApplyFunctor
		>,
		protected RootAllocator
		<
			Root,
			SFTA::Private::CUDDFacade::Node*
		>
{
GCC_DIAG_ON(effc++)
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


	/**
	 * @brief  Type of variable assignment
	 *
	 * Type of variable assignment.
	 */
	typedef VariableAssignment VariableAssignmentType;


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
	 * @brief  Type of variable name
	 *
	 * The data type for variable name.
	 */
	typedef typename ParentClass::VariableType VariableType;


	/**
	 * @brief  Type of description
	 *
	 * The data type for MTBDD description
	 */
	typedef typename ParentClass::DescriptionType DescriptionType;


	/**
	 * @brief  Type of variable renaming functor
	 *
	 * The data type for class of variable renaming functor.
	 */
	typedef typename ParentClass::AbstractVariableRenamingFunctorType
		AbstractVariableRenamingFunctorType;


	/**
	 * @brief  Type of variable predicate functor
	 *
	 * The data type for class of variable predicate functor.
	 */
	typedef typename ParentClass::AbstractVariablePredicateFunctorType
		AbstractVariablePredicateFunctorType;


	/**
	 * @brief  Type of Apply functor
	 *
	 * The data type for class of Apply functor.
	 */
	typedef typename ParentClass::AbstractApplyFunctorType
		AbstractApplyFunctorType;


	/**
	 * @brief  Type of ternary Apply functor
	 *
	 * The data type for class of ternary Apply functor.
	 */
	typedef typename ParentClass::AbstractTernaryApplyFunctorType
		AbstractTernaryApplyFunctorType;


	/**
	 * @brief  Type of monadic Apply functor
	 *
	 * The data type for class of monadic Apply functor.
	 */
	typedef typename ParentClass::AbstractMonadicApplyFunctorType
		AbstractMonadicApplyFunctorType;


public:    // Public data types


	/**
	 * @brief  The container type for leaves
	 *
	 * The type that serves as a container of several leaves. This type is used
	 * by the GetValue() method.
	 *
	 * @see  GetValue()
	 */
	typedef typename ParentClass::LeafContainer LeafContainer;


private:   // Private data types


	/**
	 * @brief  The type of the leaf allocator
	 *
	 * The type of the leaf allocator class with correct template instantiation.
	 */
	typedef LeafAllocator
	<
		LeafType,
		SFTA::Private::CUDDFacade::ValueType,
		SFTA::Private::CUDDFacade::AbstractMonadicApplyFunctor
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
	 * @brief  The type for an array of roots
	 *
	 * The type that represents an array of roots.
	 */
	typedef std::vector<typename RA::RootType> RootArray;


	/**
	 * @brief  Generic Apply functor
	 *
	 * Apply functor that can generically carry out an arbitrary operation
	 * defined on higher level, i.e. using the leaves of CUDDSharedMTBDD
	 */
	class GenericApplyFunctor : public CUDDFacade::AbstractApplyFunctor
	{
	private:

		/**
		 * The MTBDD in which is the operation carried out.
		 */
		CUDDSharedMTBDD* mtbdd_;

		/**
		 * The higher level operation functor.
		 */
		typename ParentClass::AbstractApplyFunctorType* func_;

	private:

		/**
		 * @brief  Copy constructor
		 *
		 * Copy constructor.
		 *
		 * @param[in]  func  Copied functor
		 */
		GenericApplyFunctor(const GenericApplyFunctor& func);


		/**
		 * @brief  Assignment operator
		 *
		 * Assignment operator.
		 *
		 * @param[in]  func  Assigned value
		 *
		 * @returns  New value
		 */
		GenericApplyFunctor& operator=(const GenericApplyFunctor& func);

	public:

		/**
		 * @brief  Constructor
		 *
		 * Constructor of the functor.
		 *
		 * @param[in]  mtbdd  The MTBDD in which the operation is carried out
		 * @param[in]  func   The functor performing the higher level operation
		 */
		GenericApplyFunctor(CUDDSharedMTBDD* mtbdd,
			typename ParentClass::AbstractApplyFunctorType* func)
			: mtbdd_(mtbdd), func_(func)
		{
			// Assertions
			assert(mtbdd != static_cast<CUDDSharedMTBDD*>(0));
			assert(func
				!= static_cast<typename ParentClass::AbstractApplyFunctorType*>(0));
		}


		/**
		 * @brief  The operation
		 *
		 * The operation of the functor. It calls the higher level operation and
		 * correctly handles new leaves.
		 *
		 * @param[in]  lhs  Left-hand side leaf
		 * @param[in]  rhs  Right-hand side leaf
		 *
		 * @returns  Result leaf
		 */
		virtual CUDDFacade::ValueType operator()(const CUDDFacade::ValueType& lhs,
			const CUDDFacade::ValueType& rhs)
		{
			// perform the operation
			typename LA::LeafType res = (*func_)(
				mtbdd_->LA::getLeafOfHandle(lhs), mtbdd_->LA::getLeafOfHandle(rhs));

			// create a leaf and return its handle
			return mtbdd_->LA::createLeaf(res);
		}
	};


	/**
	 * @brief  Generic ternary Apply functor
	 *
	 * Ternary Apply functor that can generically carry out an arbitrary
	 * operation defined on higher level, i.e. using the leaves of
	 * CUDDSharedMTBDD
	 */
	class GenericTernaryApplyFunctor
		: public CUDDFacade::AbstractTernaryApplyFunctor
	{
	private:

		/**
		 * The MTBDD in which is the operation carried out.
		 */
		CUDDSharedMTBDD* mtbdd_;

		/**
		 * The higher level operation functor.
		 */
		typename ParentClass::AbstractTernaryApplyFunctorType* func_;

	private:

		/**
		 * @brief  Copy constructor
		 *
		 * Copy constructor.
		 *
		 * @param[in]  func  Copied functor
		 */
		GenericTernaryApplyFunctor(const GenericTernaryApplyFunctor& func);


		/**
		 * @brief  Assignment operator
		 *
		 * Assignment operator.
		 *
		 * @param[in]  func  Assigned value
		 *
		 * @returns  New value
		 */
		GenericTernaryApplyFunctor& operator=(const GenericTernaryApplyFunctor& func);

	public:

		/**
		 * @brief  Constructor
		 *
		 * Constructor of the functor.
		 *
		 * @param[in]  mtbdd  The MTBDD in which the operation is carried out
		 * @param[in]  func   The functor performing the higher level operation
		 */
		GenericTernaryApplyFunctor(CUDDSharedMTBDD* mtbdd,
			typename ParentClass::AbstractTernaryApplyFunctorType* func)
			: mtbdd_(mtbdd), func_(func)
		{
			// Assertions
			assert(mtbdd != static_cast<CUDDSharedMTBDD*>(0));
			assert(func
				!= static_cast<typename ParentClass::AbstractTernaryApplyFunctorType*>(0));
		}


		/**
		 * @brief  The operation
		 *
		 * The operation of the functor. It calls the higher level operation and
		 * correctly handles new leaves.
		 *
		 * @param[in]  lhs  Left-hand side leaf
		 * @param[in]  mhs  Middle-hand side leaf
		 * @param[in]  rhs  Right-hand side leaf
		 *
		 * @returns  Result leaf
		 */
		virtual CUDDFacade::ValueType operator()(const CUDDFacade::ValueType& lhs,
			const CUDDFacade::ValueType& mhs, const CUDDFacade::ValueType& rhs)
		{
			// perform the operation
			typename LA::LeafType res = (*func_)(mtbdd_->LA::getLeafOfHandle(lhs),
				mtbdd_->LA::getLeafOfHandle(mhs), mtbdd_->LA::getLeafOfHandle(rhs));

			// create a leaf and return its handle
			return mtbdd_->LA::createLeaf(res);
		}
	};


	/**
	 * @brief  Generic monadic Apply functor
	 *
	 * Monadic Apply functor that can generically carry out an arbitrary
	 * operation defined on higher level, i.e. using the leaves of
	 * CUDDSharedMTBDD
	 */
	class GenericMonadicApplyFunctor
		: public CUDDFacade::AbstractMonadicApplyFunctor
	{
	private:

		/**
		 * The MTBDD in which is the operation carried out.
		 */
		CUDDSharedMTBDD* mtbdd_;

		/**
		 * The higher level operation functor.
		 */
		typename ParentClass::AbstractMonadicApplyFunctorType* func_;

	private:

		/**
		 * @brief  Copy constructor
		 *
		 * Copy constructor.
		 *
		 * @param[in]  func  Copied functor
		 */
		GenericMonadicApplyFunctor(const GenericMonadicApplyFunctor& func);


		/**
		 * @brief  Assignment operator
		 *
		 * Assignment operator.
		 *
		 * @param[in]  func  Assigned value
		 *
		 * @returns  New value
		 */
		GenericMonadicApplyFunctor& operator=(const GenericMonadicApplyFunctor& func);

	public:

		/**
		 * @brief  Constructor
		 *
		 * Constructor of the functor.
		 *
		 * @param[in]  mtbdd  The MTBDD in which the operation is carried out
		 * @param[in]  func   The functor performing the higher level operation
		 */
		GenericMonadicApplyFunctor(CUDDSharedMTBDD* mtbdd,
			typename ParentClass::AbstractMonadicApplyFunctorType* func)
			: mtbdd_(mtbdd), func_(func)
		{
			// Assertions
			assert(mtbdd != static_cast<CUDDSharedMTBDD*>(0));
			assert(func
				!= static_cast<typename ParentClass::AbstractMonadicApplyFunctorType*>(0));
		}


		/**
		 * @brief  The operation
		 *
		 * The operation of the functor. It calls the higher level operation and
		 * correctly handles new leaves.
		 *
		 * @param[in]  val  The value
		 *
		 * @returns  Result leaf
		 */
		virtual CUDDFacade::ValueType operator()(const CUDDFacade::ValueType& val)
		{
			// perform the operation
			typename LA::LeafType res = (*func_)(mtbdd_->LA::getLeafOfHandle(val));

			// create a leaf and return its handle
			return mtbdd_->LA::createLeaf(res);
		}
	};


	/**
	 * @brief  Projection Apply functor
	 *
	 * Apply functor that makes a projection of the left-hand side MTBDD with
	 * respect to the right-hand side MTBDD.
	 */
	class ProjectByRightApplyFunctor : public CUDDFacade::AbstractApplyFunctor
	{
	public:

		/**
		 * @brief  The operation
		 *
		 * The method that performs the projection operation.
		 *
		 * @param[in]  lhs  Left-hand side leaf
		 * @param[in]  rhs  Right-hand side leaf
		 *
		 * @returns  Result leaf
		 */
		virtual CUDDFacade::ValueType operator()(const CUDDFacade::ValueType& lhs,
			const CUDDFacade::ValueType& rhs)
		{
			return (rhs == LA::BOTTOM)? LA::BOTTOM : lhs;
		}
	};


	/**
	 * @brief  Overwriting Apply functor
	 *
	 * Apply functor that overwrites values in an MTBDD with non-bottom values
	 * in the right-hand side MTBDD.
	 */
	class OverwriteByRightApplyFunctor : public CUDDFacade::AbstractApplyFunctor
	{
	public:

		/**
		 * @brief  The operation
		 *
		 * The method that performs the overwriting operation.
		 *
		 * @param[in]  lhs  Left-hand side leaf
		 * @param[in]  rhs  Right-hand side leaf
		 *
		 * @returns  Result leaf
		 */
		virtual CUDDFacade::ValueType operator()(const CUDDFacade::ValueType& lhs,
			const CUDDFacade::ValueType& rhs)
		{
			return (rhs == LA::BOTTOM)? lhs : rhs;
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
	inline CUDDFacade::Node* getIthVariable(size_t i)
	{
		CUDDFacade::Node* node = cudd_.AddIthVar(i);
		cudd_.Ref(node);
		return node;
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
	inline CUDDFacade::Node* getIthVariableNot(size_t i)
	{
		CUDDFacade::Node* node = getIthVariable(i);
		CUDDFacade::Node* cmplNode = cudd_.AddCmpl(node);
		cudd_.Ref(cmplNode);
		cudd_.RecursiveDeref(node);

		return cmplNode;
	}


	/**
	 * @brief  Gets node for variable assignment value
	 *
	 * Gets node at index @f$i@f$ for given variable assignment.
	 *
	 * @param[in]  i    Index of the node
	 * @param[in]  val  Value of the variable assignment
	 *
	 * @returns  Node for given variable assignment
	 */
	inline CUDDFacade::Node* getNodeForVariableAssignmentValue(size_t i, char val)
	{
		switch (val)
		{
			case VariableAssignmentType::ONE:
				return getIthVariable(i);
			case VariableAssignmentType::ZERO:
				return getIthVariableNot(i);
			case VariableAssignmentType::DONT_CARE:
				return static_cast<CUDDFacade::Node*>(0);
			default:
				throw std::runtime_error("Invalid variable assignment type passed "
					" to getNodeForVariableAssignmentValue()!");
		}
	}


	/**
	 * @brief  Creates a new MTBDD for a variable assignment
	 *
	 * Creates a new MTBDD in the shared MTBDD for given variable assignment @c
	 * vars = @f$(x_1, x_2, \dots, x_n)@f$ equal to the given value and returns
	 * the root of the MTBDD.
	 *
	 * @param[in]  vars   Variable assignment
	 * @param[in]  value  Value for given variable assignment
	 *
	 * @returns  The root of the created MTBDD
	 */
	CUDDFacade::Node* createMTBDDForVariableAssignment(
		const VariableAssignmentType& vars, const LeafType& value)
	{
		assert(vars.VariablesCount() <= GetMaxSize());

		CUDDFacade::ValueType leaf = LA::createLeaf(value);
		CUDDFacade::Node* node = cudd_.AddConst(leaf);
		cudd_.Ref(node);

		for (size_t i = 0; i < vars.VariablesCount(); ++i)
		{	// for all variables
			CUDDFacade::Node* var = static_cast<CUDDFacade::Node*>(0);
			if ((var = getNodeForVariableAssignmentValue(i,
				vars.GetIthVariableValue(i))) !=
				static_cast<typename CUDDFacade::Node*>(0))
			{
				CUDDFacade::Node* oldNode = node;
				node = cudd_.Times(node, var);
				cudd_.Ref(node);
				cudd_.RecursiveDeref(oldNode);
				cudd_.RecursiveDeref(var);
			}
		}

		return node;
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
	CUDDFacade::Node* createMTBDDForVariableProjection(
		const VariableAssignmentType& vars)
	{
		assert(vars.VariablesCount() <= GetMaxSize());

		CUDDFacade::Node* node = cudd_.AddConst(1);
		cudd_.Ref(node);

		for (size_t i = 0; i < vars.VariablesCount(); ++i)
		{	// for all variables
			CUDDFacade::Node* var = static_cast<CUDDFacade::Node*>(0);
			if ((var = getNodeForVariableAssignmentValue(i,
				vars.GetIthVariableValue(i))) !=
				static_cast<typename CUDDFacade::Node*>(0))
			{
				CUDDFacade::Node* oldNode = node;
				node = cudd_.Times(node, var);
				cudd_.Ref(node);
				cudd_.RecursiveDeref(oldNode);
				cudd_.RecursiveDeref(var);
			}
		}

		return node;
	}

	/**
	 * @brief  Correctly removes CUDD root
	 *
	 * This method correctly removes a CUDD root (including calling release
	 * functor for all leaves).
	 *
	 * @param[in]  root  CUDD root
	 */
	void eraseCUDDRoot(CUDDFacade::Node* root)
	{
		// Assertions
		assert(root != static_cast<CUDDFacade::Node*>(0));

		// carry out the monadic Apply operation
		CUDDFacade::Node* monRes = cudd_.MonadicApply(root, LA::getLeafReleaser());
		cudd_.Ref(monRes);

		// remove both MTBDDs
		cudd_.RecursiveDeref(monRes);
		cudd_.RecursiveDeref(root);
	}

	void getNodeDescription(CUDDFacade::Node* node, VariableAssignmentType asgn,
		DescriptionType& desc) const
	{
		// Assertions
		assert(node != static_cast<CUDDFacade::Node*>(0));

		if (cudd_.IsNodeConstant(node))
		{
			LeafType leaf = LA::getLeafOfHandle(cudd_.GetNodeValue(node));

			if (!desc.insert(std::make_pair(asgn, leaf)).second)
			{	// in case the index already has a value
				throw std::logic_error("Inserting to an already existing index!");
			}
		}
		else
		{
			asgn.SetIthVariableValue(cudd_.GetNodeIndex(node),
				VariableAssignmentType::ONE);
			getNodeDescription(cudd_.GetThenChild(node), asgn, desc);
			asgn.SetIthVariableValue(cudd_.GetNodeIndex(node),
				VariableAssignmentType::ZERO);
			getNodeDescription(cudd_.GetElseChild(node), asgn, desc);
		}
	}


	size_t GetMaxSize() const
	{
		// TODO: declare a private field maxSize_ that remembers the maximum
		// possible size of the MTBDD, i.e., if a new variable is first
		// referenced, the value is increased, and if a variable is removed (or
		// renamed), the value is possibly decreased.
		return 64;
	}

public:   // Public methods


	/**
	 * @brief  Constructor
	 *
	 * The constructor of CUDDSharedMTBDD.
	 */
	CUDDSharedMTBDD() : cudd_()
	{ }


	virtual void SetValue(const RootType& root,
		const VariableAssignmentType& asgn, const LeafType& value)
	{
		CUDDFacade::Node* mtbddAsgn = createMTBDDForVariableAssignment(asgn, value);
		CUDDFacade::Node* rootNode = RA::getHandleOfRoot(root);

		OverwriteByRightApplyFunctor overwriter;
		CUDDFacade::Node* res = cudd_.Apply(rootNode, mtbddAsgn, &overwriter);
		cudd_.Ref(res);

		// remove the temporary MTBDD
		cudd_.RecursiveDeref(mtbddAsgn);

		// get rid of the old MTBDD for the function
		cudd_.RecursiveDeref(rootNode);

		// substitute the new MTBDD for the old one
		RA::changeHandleOfRoot(root, res);
	}


	virtual LeafContainer GetValue(const RootType& root,
		const VariableAssignmentType& asgn)
	{
		CUDDFacade::Node* mtbddAsgn = createMTBDDForVariableProjection(asgn);

		ProjectByRightApplyFunctor projector;
		CUDDFacade::Node* res = cudd_.Apply(RA::getHandleOfRoot(root),
			mtbddAsgn, &projector);
		cudd_.Ref(res);

		// remove the temporary MTBDD
		cudd_.RecursiveDeref(mtbddAsgn);

		/**
		 * Monadic Apply functor that collects all distinct leaves of given MTBDD
		 * into an array.
		 */
		class CollectLeavesMonadicApplyFunctor
			: public CUDDFacade::AbstractMonadicApplyFunctor
		{
		public:

			typedef std::set<typename LA::HandleType> LeafHandleSet;

		private:

			LeafHandleSet leaves_;

		public:

			CollectLeavesMonadicApplyFunctor()
				: leaves_()
			{	}

			virtual CUDDFacade::ValueType operator()(const CUDDFacade::ValueType& val)
			{
				typename LeafHandleSet::const_iterator itLeaves;
				if ((itLeaves = leaves_.find(val)) == leaves_.end())
				{	// in case the value has not been found in the set
					leaves_.insert(val);
				}

				return val;
			}

			const LeafHandleSet& GetLeaves() const
			{
				return leaves_;
			}
		};

		CollectLeavesMonadicApplyFunctor collector;
		CUDDFacade::Node* monRes = cudd_.MonadicApply(res, &collector);
		cudd_.Ref(monRes);

		// remove temporary MTBDDs
		cudd_.RecursiveDeref(res);
		cudd_.RecursiveDeref(monRes);

		// get reference to the result
		const typename CollectLeavesMonadicApplyFunctor::LeafHandleSet& leafSet
			= collector.GetLeaves();

		// TODO @todo output directly output of collector?
		typename ParentClass::LeafContainer leaves;
		for (typename
			CollectLeavesMonadicApplyFunctor::LeafHandleSet::const_iterator it
			= leafSet.begin();
			it != leafSet.end(); ++it)
		{	// for each leaf handle
			if (*it != LA::BOTTOM)
			{
				leaves.push_back(&LA::getLeafOfHandle(*it));
			}
		}

		return leaves;
	}


	/**
	 * @brief  @copybrief  SFTA::AbstractSharedMTBDD::Apply()
	 *
	 * @copydetails  SFTA::AbstractSharedMTBDD::Apply()
	 */
	virtual RootType Apply(const RootType& lhs, const RootType& rhs,
		AbstractApplyFunctorType* func)
	{
		// Assertions
		assert(func
			!= static_cast<typename ParentClass::AbstractApplyFunctorType*>(0));

		GenericApplyFunctor applier(this, func);

		// carry out the Apply operation
		CUDDFacade::Node* res = cudd_.Apply(RA::getHandleOfRoot(lhs),
			RA::getHandleOfRoot(rhs), &applier);

		cudd_.Ref(res);

		return RA::allocateRoot(res);
	}


	/**
	 * @brief  @copybrief  SFTA::AbstractSharedMTBDD::TernaryApply()
	 *
	 * @copydetails  SFTA::AbstractSharedMTBDD::TernaryApply()
	 */
	virtual RootType TernaryApply(const RootType& lhs, const RootType& mhs,
		const RootType& rhs, AbstractTernaryApplyFunctorType* func)
	{
		// Assertions
		assert(func
			!= static_cast<typename ParentClass::AbstractTernaryApplyFunctorType*>(0));

		GenericTernaryApplyFunctor applier(this, func);

		// carry out the ternary Apply operation
		CUDDFacade::Node* res = cudd_.TernaryApply(RA::getHandleOfRoot(lhs),
			RA::getHandleOfRoot(mhs), RA::getHandleOfRoot(rhs), &applier);

		cudd_.Ref(res);

		return RA::allocateRoot(res);
	}


	/**
	 * @brief  @copybrief  SFTA::AbstractSharedMTBDD::MonadicApply()
	 *
	 * @copydetails  SFTA::AbstractSharedMTBDD::MonadicApply()
	 */
	virtual RootType MonadicApply(const RootType& root,
		AbstractMonadicApplyFunctorType* func)
	{
		// Assertions
		assert(func
			!= static_cast<typename ParentClass::AbstractMonadicApplyFunctorType*>(0));

		GenericMonadicApplyFunctor applier(this, func);

		// carry out the monadic Apply operation
		CUDDFacade::Node* res = cudd_.MonadicApply(RA::getHandleOfRoot(root),
			&applier);

		cudd_.Ref(res);

		return RA::allocateRoot(res);
	}


	virtual RootType CreateRoot()
	{
		CUDDFacade::Node* node = cudd_.ReadBackground();
		cudd_.Ref(node);

		return RA::allocateRoot(node);
	}


	virtual void EraseRoot(const RootType& root)
	{
		eraseCUDDRoot(RA::getHandleOfRoot(root));
		RA::eraseRoot(root);
	}


	virtual void SetBottomValue(const LeafType& bottom)
	{
		LA::setBottom(bottom);
	}


	virtual std::string Serialize() const
	{
		// the array of roots
		RootArray roots = RA::getAllRoots();

		// dictionary to store pointers to roots
		CUDDFacade::StringNodeMapType rootDict;

		for (size_t i = 0; i < roots.size(); ++i)
		{	// insert all pointers to roots
			rootDict[Convert::ToString(roots[i])] = RA::getHandleOfRoot(roots[i]);
		}

		std::string result;
		result += "<cuddsharedmtbdd>\n";
		result += RA::serialize();
		result += "\n";
		result += LA::serialize();
		result += "\n";
		result += cudd_.SerializeToXML(rootDict);
		result += "\n</cuddsharedmtbdd>";

		return result;
	}


	virtual void DumpToDotFile(const std::string& filename) const
	{
		// the array of roots
		RootArray roots = RA::getAllRoots();
		// the array of root nodes
		std::vector<CUDDFacade::Node*> nodes(roots.size());
		// the array of root names
		std::vector<std::string> rootNames(roots.size());

		for (size_t i = 0; i < roots.size(); ++i)
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

		for (size_t i = 0; i < sinks.size(); ++i)
		{	// insert all sink nodes' names
			sinkNames[sinks[i]] = Convert::ToString(sinks[i]) + " : "
				+ Convert::ToString(LA::getLeafOfHandle(sinks[i]));
		}

		// create the Dot file
		cudd_.DumpDot(nodes, rootNames, sinkNames, filename);
	}


	virtual RootType RenameVariables(const RootType& root,
		AbstractVariableRenamingFunctorType* func)
	{
		// Assertions
		assert(func != static_cast<AbstractVariableRenamingFunctorType*>(0));

		CUDDFacade::Node* newRoot = RA::getHandleOfRoot(root);
		cudd_.Ref(newRoot);

		for (VariableType i = 0; i < GetMaxSize(); ++i)
		{	// rename all variables according to given renaming functor
			VariableType newName;
			if ((newName = (*func)(i)) != i)
			{	// in case the new name is different from the old one
				CUDDFacade::Node* tmp = newRoot;
				newRoot = cudd_.ChangeVariableIndex(newRoot, i, newName);
				cudd_.Ref(newRoot);
				cudd_.RecursiveDeref(tmp);
			}
		}

		return RA::allocateRoot(newRoot);
	}

	virtual RootType TrimVariables(const RootType& root,
		AbstractVariablePredicateFunctorType* pred,
		AbstractApplyFunctorType* merger)
	{
		// Assertions
		assert(pred != static_cast<AbstractVariablePredicateFunctorType*>(0));
		assert(merger != static_cast<AbstractApplyFunctorType*>(0));

		CUDDFacade::Node* oldRoot = RA::getHandleOfRoot(root);
		cudd_.Ref(oldRoot);

		class WrappingNodePredicateFunctor
			: public CUDDFacade::AbstractNodePredicateFunctor
		{
		private:

			AbstractVariablePredicateFunctorType* predFunc_;

		private:

			WrappingNodePredicateFunctor(const WrappingNodePredicateFunctor&);
			WrappingNodePredicateFunctor& operator=(
				const WrappingNodePredicateFunctor&);

		public:

			WrappingNodePredicateFunctor(
				AbstractVariablePredicateFunctorType* predFunc)
				: predFunc_(predFunc)
			{ }


			virtual bool operator()(unsigned index)
			{
				return (*predFunc_)(index);
			}
		};

		WrappingNodePredicateFunctor predFunc(pred);
		GenericApplyFunctor mergeFunc(this, merger);

		CUDDFacade::Node* newRoot = cudd_.RemoveVariables(oldRoot,
			&predFunc, &mergeFunc);
		cudd_.RecursiveDeref(oldRoot);

		return RA::allocateRoot(newRoot);
	}


	virtual DescriptionType GetMinimumDescription(const RootType& root) const
	{
		DescriptionType result;

		CUDDFacade::Node* rt = RA::getHandleOfRoot(root);
		// TODO @todo: do something
		VariableAssignmentType asgn = VariableAssignmentType::GetUniversalSymbol();

		getNodeDescription(rt, asgn, result);

		return result;
	}


	virtual ~CUDDSharedMTBDD()
	{
		RootArray roots = RA::getAllRoots();
		for (typename RootArray::const_iterator it = roots.begin();
			it != roots.end(); ++it)
		{	// traverse all roots
			EraseRoot(*it);
		}
	}
};

#endif
