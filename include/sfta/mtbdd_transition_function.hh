/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    File with the MTBDDTransitionFunction class.
 *
 *****************************************************************************/

#ifndef _MTBDD_TRANSITION_FUNCTION_HH_
#define _MTBDD_TRANSITION_FUNCTION_HH_

// SFTA headers
#include <sfta/sfta.hh>
#include <sfta/abstract_transition_function.hh>
#include <sfta/convert.hh>

// Standard library headers
#include <cassert>
#include <vector>
#include <tr1/unordered_map>

// Boost library headers
#include <boost/functional/hash.hpp>


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

	typedef Symbol SymbolType;
	typedef State StateType;
	typedef LeftHandSide<StateType> LeftHandSideType;
	typedef InputRightHandSide<StateType> InputRightHandSideType;
	typedef OutputRightHandSide<StateType> OutputRightHandSideType;
	typedef MTBDD MTBDDType;
	typedef MTBDDRoot MTBDDRootType;

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


private:  // Private data members

	/**
	 * @brief  The MTBDD
	 *
	 * The MTBDD used to implement the transition function.
	 */
	MTBDDType mtbdd_;

	StateType sinkState_;


	MTBDDRootType container0_;
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
		switch (lhs.size())
		{
			case 0: return getRootForArity0(lhs); break;
			case 1: return getRootForArity1(lhs); break;
			case 2: return getRootForArity2(lhs); break;
			default: return getRootForArityN(lhs); break;
		}
	}

	MTBDDRootType getRootForArity0(const LeftHandSideType& lhs) const
	{
		// Assertions
		assert(lhs.size() == 0);

		return container0_;
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
		switch (lhs.size())
		{
			case 0: return setRootForArity0(lhs, root); break;
			case 1: return setRootForArity1(lhs, root); break;
			case 2: return setRootForArity2(lhs, root); break;
			default: return setRootForArityN(lhs, root); break;
		}
	}

	void setRootForArity0(const LeftHandSideType& lhs, MTBDDRootType root)
	{
		// Assertions
		assert(lhs.size() == 0);

		container0_ = root;
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


public:   // Public methods

	MTBDDTransitionFunction(size_t statesCount, const StateType& sinkState)
		: mtbdd_(),
			sinkState_(sinkState),
		  container0_(sinkState),
			container1_(statesCount, sinkState),
			container2_(statesCount, MTBDDRootTypeArray(statesCount, sinkState)),
			containerN_()
	{
		InputRightHandSideType rhs;
		rhs.push_back(sinkState);
		mtbdd_.SetBottomValue(rhs);
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
