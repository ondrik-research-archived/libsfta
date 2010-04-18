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
		class MTBDD,
		typename MTBDDRoot
	>
	class MTBDDTransitionFunction;
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
 * @tparam  InputRightHandSide   The data type for a right-hand side of
 *                               a transition rule that is used for insertion
 *                               of the rule to the transition function table.
 * @tparam  OutputRightHandSide  The data type for a right-hand side of
 *                               a transition rule that is used for retrieval
 *                               of the rule from the transition function
 *                               table.
 * @tparam  MTBDD                The data type of the MTBDD to be used.
 * @tparam  MTBDDRoot            The data type of a MTBDD root.
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
	class MTBDD,
	typename MTBDDRoot
>
class SFTA::MTBDDTransitionFunction
	: public AbstractTransitionFunction
		<
			Symbol,
			State,
			LeftHandSide,
			InputRightHandSide,
			OutputRightHandSide
		>
{
public:   // Public data types

	typedef AbstractTransitionFunction<Symbol, State, LeftHandSide, InputRightHandSide, OutputRightHandSide> ParentClass;

	typedef typename ParentClass::SymbolType SymbolType;
	typedef typename ParentClass::StateType StateType;
	typedef typename ParentClass::LeftHandSideType LeftHandSideType;
	typedef typename ParentClass::InputRightHandSideType InputRightHandSideType;
	typedef typename ParentClass::OutputRightHandSideType OutputRightHandSideType;
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

private:  // Private data members

	/**
	 * @brief  The MTBDD
	 *
	 * The MTBDD used to implement the transition function.
	 */
	MTBDDType mtbdd_;

	StateType sinkState_;


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

	MTBDDRootType getRoot(const LeftHandSideType& lhs) const
	{
		// Assertions
		assert(lhs.size() > 0);

		switch (lhs.size())
		{
			case 1: return getRootForArity1(lhs); break;
			case 2: return getRootForArity2(lhs); break;
			default: return getRootForArityN(lhs); break;
		}
	}

	MTBDDRootType getRootForArity1(const LeftHandSideType& lhs) const
	{
		// Assertions
		assert(lhs.size() == 1);

		return container1_[lhs[0]];
	}

	MTBDDRootType getRootForArity2(const LeftHandSideType& lhs) const
	{
		// Assertions
		assert(lhs.size() == 2);

		return container2_[lhs[0]][lhs[1]];
	}

	MTBDDRootType getRootForArityN(const LeftHandSideType& lhs) const
	{
		// Assertions
		assert(lhs.size() > 2);

		typename MTBDDRootTypeHashTable::const_iterator it = containerN_.find(lhs);
		if (it == containerN_.end())
		{	// in case the key is not in the hash table
			return mtbdd_.GetRootForBottom();
		}

		return it->second;
	}


	void setRoot(const LeftHandSideType& lhs, MTBDDRootType root)
	{
		// Assertions
		assert(lhs.size() > 0);

		switch (lhs.size())
		{
			case 1: return setRootForArity1(lhs, root); break;
			case 2: return setRootForArity2(lhs, root); break;
			default: return setRootForArityN(lhs, root); break;
		}
	}

	void setRootForArity1(const LeftHandSideType& lhs, MTBDDRootType root)
	{
		// Assertions
		assert(lhs.size() == 1);

		container1_[lhs[0]] = root;
	}

	void setRootForArity2(const LeftHandSideType& lhs, MTBDDRootType root)
	{
		// Assertions
		assert(lhs.size() == 2);

		container2_[lhs[0]][lhs[1]] = root;
	}

	void setRootForArityN(const LeftHandSideType& lhs, MTBDDRootType root)
	{
		// Assertions
		assert(lhs.size() > 2);

		containerN_[lhs] = root;
	}

	LHSRootList getAllRootsForArity1() const
	{
		LHSRootList lst;

		for (size_t i = 0; i < container1_.size(); ++i)
		{	// append all non-bottom roots
			if (container1_[i] != sinkState_)
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

	LHSRootList getAllRootsForArity2() const
	{
		LHSRootList lst;

		for (size_t i = 0; i < container2_.size(); ++i)
		{
			const MTBDDRootTypeArray& innerArr = container2_[i];

			for (size_t j = 0; j < innerArr.size(); ++j)
			{	// append all non-bottom roots
				if (innerArr[j] != sinkState_)
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

	LHSRootList getAllRootsForArityN() const
	{
		LHSRootList lst;

		for (typename MTBDDRootTypeHashTable::const_iterator it
			= containerN_.begin(); it != containerN_.end(); ++it)
		{	// append all non-bottom roots
			if (it->second != sinkState_)
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

		SFTA_LOGGER_DEBUG("Getting transition from root "
			+ Convert::ToString(root) + " for symbol "
			+ Convert::ToString(symbol) + " and position "
			+ Convert::ToString(position));

		if (position == SymbolType::VariablesCount)
		{	// if we are at the bottom
			RHSSymbolType rhsSymbol;
			Loki::Field<0>(rhsSymbol) = symbol;

			Loki::Field<1>(rhsSymbol) = *(mtbdd_.GetValue(root, symbol)[0]);

			typename MTBDDType::LeafContainer leaves = mtbdd_.GetValue(root, symbol);
			for (typename MTBDDType::LeafContainer::const_iterator it
				= leaves.begin(); it != leaves.end(); ++it)
			{
				if ((**it)[0] != sinkState_)
				{
					Loki::Field<1>(rhsSymbol) = **it;
				}
			}

			RHSSymbolListType result;

			if (Loki::Field<1>(rhsSymbol)[0] != sinkState_)
			{
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

	MTBDDTransitionFunction(size_t statesCount, const StateType& sinkState)
		: mtbdd_(),
			sinkState_(),
			container1_(),
			container2_(),
			containerN_()
	{
		InputRightHandSideType rhs;
		rhs.push_back(sinkState);
		mtbdd_.SetBottomValue(rhs);

		sinkState_ = mtbdd_.GetRootForBottom();
		container1_ = MTBDDRootTypeArray(statesCount, mtbdd_.GetRootForBottom());
		container2_ = MTBDDRootTypeMatrix(statesCount, MTBDDRootTypeArray(statesCount, mtbdd_.GetRootForBottom()));
	}


	virtual void AddTransition(const SymbolType& symbol,
		const LeftHandSideType& lhs, const InputRightHandSideType& rhs)
	{
		SFTA_LOGGER_DEBUG("Adding transition: \"" + Convert::ToString(symbol)
			+ Convert::ToString(lhs) + " -> " + Convert::ToString(rhs) + "\"");

		// retrieve the pointer to the root of the MTBDD
		MTBDDRootType root = getRoot(lhs);
		if (root == mtbdd_.GetRootForBottom())
		{	// in case there has been nothing assigned to this combination of states
			SFTA_LOGGER_DEBUG("Creating new MTBDD for " + Convert::ToString(lhs));
			root = mtbdd_.CreateRoot();
			setRoot(lhs, root);
		}

		mtbdd_.SetValue(root, symbol, rhs);
	}


	virtual OutputRightHandSideType GetTransition(const SymbolType& symbol,
		const LeftHandSideType& lhs)
	{
		SFTA_LOGGER_DEBUG("Reading transition: \"" + Convert::ToString(symbol)
			+ Convert::ToString(lhs) + " -> ???\"");

		MTBDDRootType root = getRoot(lhs);

		OutputRightHandSideType res;

		typename MTBDDType::LeafContainer leaves = mtbdd_.GetValue(root, symbol);

		SFTA_LOGGER_DEBUG("Read transition: \"" + Convert::ToString(symbol)
			+ Convert::ToString(lhs) + " -> " + Convert::ToString(leaves) + "\"");

		for (typename MTBDDType::LeafContainer::const_iterator it = leaves.begin();
			it != leaves.end(); ++it)
		{
			res.insert(res.end(), (*it)->begin(), (*it)->end());
		}

		return res;
	}

	inline MTBDDType& GetMTBDD()
	{
		return mtbdd_;
	}

	virtual TransitionListType GetListOfTransitions()
	{
		LHSRootList lst;
		LHSRootList arity1Lst = getAllRootsForArity1();
		lst.insert(lst.end(), arity1Lst.begin(), arity1Lst.end());
		LHSRootList arity2Lst = getAllRootsForArity2();
		lst.insert(lst.end(), arity2Lst.begin(), arity2Lst.end());
		LHSRootList arityNLst = getAllRootsForArityN();
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


	virtual ~MTBDDTransitionFunction()
	{ }

};


// Setting the logging category name for Log4cpp
template
<
	typename Symbol,
	typename State,
	template <typename> class LHS,
	template <typename> class IRHS,
	template <typename> class ORHS,
	class MTBDD,
	typename MTBDDRoot
>
const char* SFTA::MTBDDTransitionFunction<Symbol, State, LHS, IRHS, ORHS,
	MTBDD, MTBDDRoot>::LOG_CATEGORY_NAME = "mtbdd_transition_function";

#endif
