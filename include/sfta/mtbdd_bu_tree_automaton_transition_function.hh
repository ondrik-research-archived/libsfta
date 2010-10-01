/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    File with the MTBDDTransitionFunction class.
 *
 *****************************************************************************/

#ifndef _SFTA_MTBDD_TRANSITION_FUNCTION_HH_
#define _SFTA_MTBDD_TRANSITION_FUNCTION_HH_

// SFTA headers
#include <sfta/sfta.hh>
#include <sfta/abstract_transition_function.hh>
#include <sfta/convert.hh>

// Standard library headers
#include <cassert>
#include <vector>
#include <list>
#include <tr1/unordered_map>

// Boost library headers
#include <boost/functional/hash.hpp>

// Loki library header files
#include <loki/HierarchyGenerators.h>
#include <loki/TypelistMacros.h>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename Symbol,
		typename State,
		template <typename> class LeftHandSide,
		template <typename> class InputRightHandSide,
		template <typename> class OutputRightHandSide,
		typename RegistrationToken,
		class MTBDD,
		typename MTBDDRoot,
		template <typename> class StateAllocator
	>
	class MTBDDTransitionFunction;

	template <class>
	class MTBDDOperation;
}


struct MyHasher
{
	size_t operator()(const std::vector<unsigned>& key) const
	{
		return boost::hash_range(key.begin(), key.end());
	}
};


/**
 * @brief   Class for representation of transition function using MTBDDs
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Class that uses MTBDDs for symbolic representation of transition function
 * of an automaton.
 *
 * @tparam  Symbol               The data type for a symbol.
 * @tparam  State                The data type for a state.
 * @tparam  LeftHandSide         The type for a left-hand side of a transition
 *                               rule.
 * @tparam  InputRightHandSide   The data type for a right-hand side of
 *                               a transition rule that is used for insertion
 *                               of the rule to the transition function table.
 * @tparam  OutputRightHandSide  The data type for a right-hand side of
 *                               a transition rule that is used for retrieval
 *                               of the rule from the transition function
 *                               table.
 * @tparam  RegistrationToken    The type used as a token when registering the
 *                               use from an automaton.
 * @tparam  MTBDD                The data type of the MTBDD to be used.
 * @tparam  MTBDDRoot            The data type of a MTBDD root.
 * @tparam  StateAllocator       Policy used for allocation of new states.
 *
 * TODO: @todo  Rewrite this class to be parametrized by a type list that
 *              contains types for containers used for various length of
 *              left-hand side. For example: ((RootType,
 *              std::vector<RootType>, Matrix<RootType>),
 *              HashTable<RootType>), where RootType would be used for
 *              0 arity, std::vector<RootType> for the arity of 1,
 *              Matrix<RootType> for the arrity of 2 and HashTable<RootType>
 *              for all others.
 * TODO: @todo  Add template parameter whether the number of states is fixed
 *              or not.
 */
template
<
	typename Symbol,
	typename State,
	template <typename> class LeftHandSide,
	template <typename> class InputRightHandSide,
	template <typename> class OutputRightHandSide,
	typename RegistrationToken,
	class MTBDD,
	typename MTBDDRoot,
	template <typename> class StateAllocator
>
class SFTA::MTBDDTransitionFunction
	: public AbstractTransitionFunction
		<
			Symbol,
			State,
			LeftHandSide,
			InputRightHandSide,
			OutputRightHandSide,
			RegistrationToken
		>,
	  protected StateAllocator
		<
			State
		>
{
public:   // Public data types

	typedef AbstractTransitionFunction<Symbol, State, LeftHandSide,
		InputRightHandSide, OutputRightHandSide, RegistrationToken> ParentClass;

	typedef typename ParentClass::SymbolType SymbolType;
	typedef typename ParentClass::StateType StateType;
	typedef typename ParentClass::LeftHandSideType LeftHandSideType;
	typedef typename ParentClass::InputRightHandSideType InputRightHandSideType;
	typedef typename ParentClass::OutputRightHandSideType OutputRightHandSideType;
	typedef typename ParentClass::RegistrationTokenType RegistrationTokenType;
	typedef MTBDD MTBDDType;
	typedef MTBDDRoot MTBDDRootType;

	typedef typename ParentClass::TransitionType TransitionType;
	typedef typename ParentClass::TransitionListType TransitionListType;

	typedef Loki::Tuple<LOKI_TYPELIST_2(SymbolType, InputRightHandSideType)>
		RHSSymbolType;
	typedef std::list<RHSSymbolType> RHSSymbolListType;

	typedef std::vector<MTBDDRootType> MTBDDRootTypeArray;
	typedef std::vector<MTBDDRootTypeArray> MTBDDRootTypeMatrix;
	typedef std::tr1::unordered_map<LeftHandSideType, MTBDDRootType, MyHasher>
		MTBDDRootTypeHashTable;

private:  // Private data types

	/**
	 * @brief  The type of the Convert class
	 *
	 * The type of the Convert class.
	 */
	typedef SFTA::Private::Convert Convert;

	typedef Loki::Tuple<LOKI_TYPELIST_2(LeftHandSideType, MTBDDRootType)>
		LHSRootPair;

	typedef std::list<LHSRootPair> LHSRootList;


	typedef StateAllocator
	<
		StateType
	>
	SA;

	class LeafUnionFunctor
		: public MTBDDType::AbstractApplyFunctorType
	{
	private:

		StateType sinkState_;

	public:

		explicit LeafUnionFunctor(const StateType& sinkState)
			: sinkState_(sinkState)
		{ }

		virtual typename MTBDDType::LeafType operator()(
			const typename MTBDDType::LeafType& lhs,
			const typename MTBDDType::LeafType& rhs)
		{
			if (lhs[0] == sinkState_)
			{
				return rhs;
			}
			else if (rhs[0] == sinkState_)
			{
				return lhs;
			}

			typename MTBDDType::LeafType newLeaf = lhs;
			newLeaf.insert(newLeaf.end(), rhs.begin(), rhs.end());

			return newLeaf;
		}
	};

private:  // Private data members

	/**
	 * @brief  The MTBDD
	 *
	 * The MTBDD used to implement the transition function.
	 */
	MTBDDType mtbdd_;

	MTBDDRootType sinkStateRoot_;

	StateType sinkState_;

	MTBDDRootTypeArray container0_;
	MTBDDRootTypeArray container1_;
	MTBDDRootTypeMatrix container2_;
	MTBDDRootTypeHashTable containerN_;


	/**
	 * @brief  The name of the Log4cpp category for logging
	 *
	 * The name of the Log4cpp category used for logging messages from this
	 * class.
	 */
	static const char* LOG_CATEGORY_NAME;


private:  // Private methods

	inline MTBDDType& getMTBDD()
	{
		return mtbdd_;
	}

	MTBDDRootType getRoot(const RegistrationTokenType& regToken,
		const LeftHandSideType& lhs) const
	{
		switch (lhs.size())
		{
			case 0: return getRootForArity0(regToken, lhs);
			case 1: return getRootForArity1(regToken, lhs); break;
			case 2: return getRootForArity2(regToken, lhs); break;
			default: return getRootForArityN(regToken, lhs); break;
		}
	}


	MTBDDRootType getRootForArity0(const RegistrationTokenType& regToken,
		const LeftHandSideType& lhs) const
	{
		// Assertions
		assert(lhs.size() == 0);

		return container0_[regToken];
	}

	MTBDDRootType getRootForArity1(const RegistrationTokenType& regToken,
		const LeftHandSideType& lhs) const
	{
		// Assertions
		assert(lhs.size() == 1);
		assert(regToken == regToken);  // just to make the compiler happy

		return container1_[lhs[0]];
	}

	MTBDDRootType getRootForArity2(const RegistrationTokenType& regToken,
		const LeftHandSideType& lhs) const
	{
		// Assertions
		assert(lhs.size() == 2);
		assert(regToken == regToken);  // just to make the compiler happy

		return container2_[lhs[0]][lhs[1]];
	}

	MTBDDRootType getRootForArityN(const RegistrationTokenType& regToken,
		const LeftHandSideType& lhs) const
	{
		// Assertions
		assert(lhs.size() > 2);
		assert(regToken == regToken);  // just to make the compiler happy

		typename MTBDDRootTypeHashTable::const_iterator it = containerN_.find(lhs);
		if (it == containerN_.end())
		{	// in case the key is not in the hash table
			return mtbdd_.GetRootForBottom();
		}

		return it->second;
	}


	void setRoot(const RegistrationTokenType& regToken,
		const LeftHandSideType& lhs, MTBDDRootType root)
	{
		switch (lhs.size())
		{
			case 0: return setRootForArity0(regToken, lhs, root); break;
			case 1: return setRootForArity1(regToken, lhs, root); break;
			case 2: return setRootForArity2(regToken, lhs, root); break;
			default: return setRootForArityN(regToken, lhs, root); break;
		}
	}

	void setRootForArity0(const RegistrationTokenType& regToken,
		const LeftHandSideType& lhs, MTBDDRootType root)
	{
		// Assertions
		assert(lhs.size() == 0);

		container0_[regToken] = root;
	}

	void setRootForArity1(const RegistrationTokenType& regToken,
		const LeftHandSideType& lhs, MTBDDRootType root)
	{
		// Assertions
		assert(lhs.size() == 1);
		assert(regToken == regToken);  // just to make the compiler happy

		container1_[lhs[0]] = root;
	}

	void setRootForArity2(const RegistrationTokenType& regToken,
		const LeftHandSideType& lhs, MTBDDRootType root)
	{
		// Assertions
		assert(lhs.size() == 2);
		assert(regToken == regToken);  // just to make the compiler happy

		container2_[lhs[0]][lhs[1]] = root;
	}

	void setRootForArityN(const RegistrationTokenType& regToken,
		const LeftHandSideType& lhs, MTBDDRootType root)
	{
		// Assertions
		assert(lhs.size() > 2);
		assert(regToken == regToken);  // just to make the compiler happy

		containerN_[lhs] = root;
	}

	LHSRootList getAllRootsForArity0(const RegistrationTokenType& regToken) const
	{
		LHSRootList lst;
		if (container0_[regToken] != sinkStateRoot_)
		{
			LHSRootPair pair;
			Loki::Field<0>(pair) = LeftHandSideType();
			Loki::Field<1>(pair) = container0_[regToken];
			lst.push_back(pair);
		}

		return lst;
	}

	LHSRootList getAllRootsForArity1(const RegistrationTokenType& regToken) const
	{
		// Assertions
		assert(regToken == regToken);  // just to make the compiler happy

		LHSRootList lst;
		for (size_t i = 0; i < container1_.size(); ++i)
		{	// append all non-bottom roots
			if (container1_[i] != sinkStateRoot_)
			{
				LeftHandSideType lhs(1);
				lhs[0] = i;

				LHSRootPair pair;
				Loki::Field<0>(pair) = lhs;
				Loki::Field<1>(pair) = container1_[i];
				lst.push_back(pair);
			}
		}

		return lst;
	}

	LHSRootList getAllRootsForArity2(const RegistrationTokenType& regToken) const
	{
		// Assertions
		assert(regToken == regToken);  // just to make the compiler happy

		LHSRootList lst;

		for (size_t i = 0; i < container2_.size(); ++i)
		{
			const MTBDDRootTypeArray& innerArr = container2_[i];

			for (size_t j = 0; j < innerArr.size(); ++j)
			{	// append all non-bottom roots
				if (innerArr[j] != sinkStateRoot_)
				{
					LeftHandSideType lhs(2);
					lhs[0] = i;
					lhs[1] = j;

					LHSRootPair pair;
					Loki::Field<0>(pair) = lhs;
					Loki::Field<1>(pair) = innerArr[j];
					lst.push_back(pair);
				}
			}
		}

		return lst;
	}

	LHSRootList getAllRootsForArityN(const RegistrationTokenType& regToken) const
	{
		// Assertions
		assert(regToken == regToken);  // just to make the compiler happy

		LHSRootList lst;

		for (typename MTBDDRootTypeHashTable::const_iterator it
			= containerN_.begin(); it != containerN_.end(); ++it)
		{	// append all non-bottom roots
			if (it->second != sinkStateRoot_)
			{
				LHSRootPair pair;
				Loki::Field<0>(pair) = it->first;
				Loki::Field<1>(pair) = it->second;
				lst.push_back(pair);
			}
		}

		return lst;
	}


	RHSSymbolListType getTransitionsForSymbol(MTBDDRootType root,
		SymbolType symbol, size_t position)
	{
		// Assertions
		assert(position <= SymbolType::VariablesCount);

		//SFTA_LOGGER_DEBUG("Getting transition from root "
		//	+ Convert::ToString(root) + " for symbol "
		//	+ Convert::ToString(symbol) + " and position "
		//	+ Convert::ToString(position));

		if (position == SymbolType::VariablesCount)
		{	// if we are at the bottom

			// read the leaf
			typename MTBDDType::LeafContainer leaves = mtbdd_.GetValue(root, symbol);

			// assertion
			assert(leaves.size() <= 1);

			RHSSymbolListType result;
			if (leaves.size() == 1)
			{	// in case something was found

				// assertions
				assert(leaves[0] != static_cast<typename MTBDDType::LeafType*>(0));
				typename MTBDDType::LeafType& leaf = *(leaves[0]);

				RHSSymbolType rhsSymbol;
				Loki::Field<0>(rhsSymbol) = symbol;
				Loki::Field<1>(rhsSymbol) = leaf;
				result.push_back(rhsSymbol);
			}

			return result;
		}
		else
		{	// in case we are not at the bottom
			SymbolType restrictionToZero = symbol;
			restrictionToZero.SetIthVariableValue(position, SymbolType::ZERO);
			SymbolType restrictionToOne = symbol;
			restrictionToOne.SetIthVariableValue(position, SymbolType::ONE);

			typename MTBDDType::LeafContainer zeroLeaves
				= mtbdd_.GetValue(root, restrictionToZero);
			typename MTBDDType::LeafContainer oneLeaves
				= mtbdd_.GetValue(root, restrictionToOne);

			if (zeroLeaves == oneLeaves)
			{	// in case variable at position is not important
				return getTransitionsForSymbol(root, symbol, position + 1);
			}
			else
			{	// in case leaves differ depending on the variable
				RHSSymbolListType resultZero = getTransitionsForSymbol(root,
					restrictionToZero, position + 1);
				RHSSymbolListType resultOne = getTransitionsForSymbol(root,
					restrictionToOne, position + 1);

				resultZero.insert(resultZero.end(), resultOne.begin(), resultOne.end());
				return resultZero;
			}
		}
	}

public:   // Public methods

	MTBDDTransitionFunction()
		: mtbdd_(),
			sinkStateRoot_(),
			sinkState_(),
			container0_(),
			container1_(),
			container2_(),
			containerN_()
	{
		sinkState_ = SA::AllocateState();
		// Assertion
		assert(sinkState_ == 0);

		InputRightHandSideType rhs;
		rhs.push_back(sinkState_);
		mtbdd_.SetBottomValue(rhs);

		sinkStateRoot_ = mtbdd_.GetRootForBottom();
		container0_ = MTBDDRootTypeArray(0);
		container1_ = MTBDDRootTypeArray(1, sinkStateRoot_);
		container2_ = MTBDDRootTypeMatrix(1, MTBDDRootTypeArray(1, sinkStateRoot_));
	}

	virtual void AddTransition(const RegistrationTokenType& regToken,
		const SymbolType& symbol, const LeftHandSideType& lhs,
		const InputRightHandSideType& rhs)
	{
		//SFTA_LOGGER_DEBUG("Adding transition for automaton "
		//	+ Convert::ToString(regToken) + ": \"" + Convert::ToString(symbol)
		//	+ Convert::ToString(lhs) + " -> " + Convert::ToString(rhs) + "\"");

		// retrieve the pointer to the root of the MTBDD
		MTBDDRootType root = getRoot(regToken, lhs);
		if (root == sinkStateRoot_)
		{	// in case there has been nothing assigned to this combination of states
			SFTA_LOGGER_DEBUG("Creating new MTBDD for automaton "
				+ Convert::ToString(regToken) + " and states "
				+ Convert::ToString(lhs));
			root = mtbdd_.CreateRoot();
			setRoot(regToken, lhs, root);
			mtbdd_.SetValue(root, symbol, rhs);
		}
		else
		{	// in case there already is something

			// create a new root
			MTBDDRootType tmpRoot = mtbdd_.CreateRoot();
			mtbdd_.SetValue(tmpRoot, symbol, rhs);

			MTBDDRootType newRoot = mtbdd_.Apply(root, tmpRoot,
				new LeafUnionFunctor(sinkState_));

			mtbdd_.EraseRoot(root);
			mtbdd_.EraseRoot(tmpRoot);
			setRoot(regToken, lhs, newRoot);
		}
	}


	virtual OutputRightHandSideType GetTransition(
		const RegistrationTokenType& regToken, const SymbolType& symbol,
		const LeftHandSideType& lhs)
	{
		SFTA_LOGGER_DEBUG("Reading transition for automaton "
			+ Convert::ToString(regToken) + ": \"" + Convert::ToString(symbol)
			+ Convert::ToString(lhs) + " -> ???\"");

		MTBDDRootType root = getRoot(regToken, lhs);

		OutputRightHandSideType res;

		typename MTBDDType::LeafContainer leaves
			= mtbdd_.GetValue(root, symbol);

		SFTA_LOGGER_DEBUG("Read transition of automaton "
			+ Convert::ToString(regToken) + ": \"" + Convert::ToString(symbol)
			+ Convert::ToString(lhs) + " -> " + Convert::ToString(leaves) + "\"");

		for (typename MTBDDType::LeafContainer::const_iterator it = leaves.begin();
			it != leaves.end(); ++it)
		{
			res.insert(res.end(), (*it)->begin(), (*it)->end());
		}

		return res;
	}

	virtual TransitionListType GetListOfTransitions(
		const RegistrationTokenType& regToken)
	{
		LHSRootList lst;
		LHSRootList arity0Lst = getAllRootsForArity0(regToken);
		lst.insert(lst.end(), arity0Lst.begin(), arity0Lst.end());
		LHSRootList arity1Lst = getAllRootsForArity1(regToken);
		lst.insert(lst.end(), arity1Lst.begin(), arity1Lst.end());
		LHSRootList arity2Lst = getAllRootsForArity2(regToken);
		lst.insert(lst.end(), arity2Lst.begin(), arity2Lst.end());
		LHSRootList arityNLst = getAllRootsForArityN(regToken);
		lst.insert(lst.end(), arityNLst.begin(), arityNLst.end());

		TransitionListType result;

		for (typename LHSRootList::const_iterator it = lst.begin();
			it != lst.end(); ++it)
		{	// for every valid root
			const LHSRootPair& pair = *it;

			RHSSymbolListType rhsSymbolLst;
			typename SymbolType::AssignmentList asgns =
				SymbolType::GetAllAssignments();

			for (typename SymbolType::AssignmentList::const_iterator jt
				= asgns.begin(); jt != asgns.end(); ++jt)
			{	// for every assignment of variables to the symbol
				const Symbol& symbol = *jt;
				RHSSymbolListType lstSinks
					= getTransitionsForSymbol(Loki::Field<1>(pair), symbol, 0);

				for (typename RHSSymbolListType::const_iterator kt = lstSinks.begin();
					kt != lstSinks.end(); ++kt)
				{	// for all sinks in the list
					TransitionType trans;
					Loki::Field<0>(trans) = Loki::Field<0>(*kt);
					Loki::Field<1>(trans) = Loki::Field<0>(pair);
					Loki::Field<2>(trans) = Loki::Field<1>(*kt);

					result.push_back(trans);
				}
			}
		}


		return result;
	}

	virtual RegistrationTokenType RegisterAutomaton()
	{
		container0_.push_back(sinkStateRoot_);

		return container0_.size() - 1;
	}

	virtual void UnregisterAutomaton(const RegistrationToken& regToken)
	{
		// Assertions
		assert(regToken == regToken);  // just to make the compiler happy
	}

	virtual StateType AllocateState(const RegistrationToken& regToken)
	{
		// Assertions
		assert(regToken == regToken);  // just to make the compiler happy

		StateType state = SA::AllocateState();

		container1_.push_back(sinkStateRoot_);

		MTBDDRootTypeArray arr;
		for (typename MTBDDRootTypeMatrix::iterator it = container2_.begin();
			it != container2_.end(); ++it)
		{
			it->push_back(sinkStateRoot_);
			arr.push_back(sinkStateRoot_);
		}

		arr.push_back(sinkStateRoot_);

		container2_.push_back(arr);

		return state;
	}

	virtual ~MTBDDTransitionFunction()
	{ }

	template <class> friend class MTBDDOperation;
};


// Setting the logging category name for Log4cpp
template
<
	typename Symbol,
	typename State,
	template <typename> class LHS,
	template <typename> class IRHS,
	template <typename> class ORHS,
	typename RT,
	class MTBDD,
	typename MTBDDRoot,
	template <typename> class SA
>
const char* SFTA::MTBDDTransitionFunction<Symbol, State, LHS, IRHS, ORHS, RT,
	MTBDD, MTBDDRoot, SA>::LOG_CATEGORY_NAME = "mtbdd_transition_function";

#endif
