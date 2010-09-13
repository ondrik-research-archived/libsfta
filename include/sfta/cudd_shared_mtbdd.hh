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
	 * @brief  Type of variable renaming functor
	 *
	 * The data type for class of variable renaming functor.
	 */
	typedef typename ParentClass::AbstractVariableRenamingFunctorType
		AbstractVariableRenamingFunctorType;


	/**
	 * @brief  Type of variable name
	 *
	 * The data type for variable name.
	 */
	typedef typename ParentClass::VariableType VariableType;


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

		CUDDSharedMTBDD* mtbdd_;
		typename ParentClass::AbstractApplyFunctorType* func_;

	private:

		GenericApplyFunctor(const GenericApplyFunctor&);
		GenericApplyFunctor& operator=(const GenericApplyFunctor&);

	public:

		GenericApplyFunctor(CUDDSharedMTBDD* mtbdd,
			typename ParentClass::AbstractApplyFunctorType* func)
			: mtbdd_(mtbdd), func_(func)
		{
			// Assertions
			assert(mtbdd != static_cast<CUDDSharedMTBDD*>(0));
			assert(func
				!= static_cast<typename ParentClass::AbstractApplyFunctorType*>(0));
		}

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

		CUDDSharedMTBDD* mtbdd_;
		typename ParentClass::AbstractMonadicApplyFunctorType* func_;

	private:

		GenericMonadicApplyFunctor(const GenericMonadicApplyFunctor&);
		GenericMonadicApplyFunctor& operator=(const GenericMonadicApplyFunctor&);

	public:

		GenericMonadicApplyFunctor(CUDDSharedMTBDD* mtbdd,
			typename ParentClass::AbstractMonadicApplyFunctorType* func)
			: mtbdd_(mtbdd), func_(func)
		{
			// Assertions
			assert(mtbdd != static_cast<CUDDSharedMTBDD*>(0));
			assert(func
				!= static_cast<typename ParentClass::AbstractMonadicApplyFunctorType*>(0));
		}

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
	 *
	 * @todo TODO Use function that defines number of variables
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
	 *
	 * @todo TODO Use function that defines number of variables
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
	 *
	 * @todo TODO Use function that defines number of variables
	 */
	CUDDFacade::Node* createMTBDDForVariableAssignment(
		const VariableAssignmentType& vars, const LeafType& value)
	{
		CUDDFacade::ValueType leaf = LA::createLeaf(value);
		CUDDFacade::Node* node = cudd_.AddConst(leaf);
		cudd_.Ref(node);

		for (size_t i = 0; i < vars.Size(); ++i)
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
	 *
	 * @todo  TODO Use function that defines number of variables
	 */
	CUDDFacade::Node* createMTBDDForVariableProjection(
		const VariableAssignmentType& vars)
	{
		CUDDFacade::Node* node = cudd_.AddConst(1);
		cudd_.Ref(node);

		for (size_t i = 0; i < vars.Size(); ++i)
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


	virtual typename ParentClass::LeafContainer GetValue(const RootType& root,
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
		typename ParentClass::AbstractApplyFunctorType* func)
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
	 * @brief  @copybrief  SFTA::AbstractSharedMTBDD::MonadicApply()
	 *
	 * @copydetails  SFTA::AbstractSharedMTBDD::MonadicApply()
	 */
	virtual RootType MonadicApply(const RootType& root,
		typename ParentClass::AbstractMonadicApplyFunctorType* func)
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


	virtual void EraseRoot(RootType root)
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
