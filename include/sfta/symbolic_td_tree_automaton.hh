/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file with SymbolicTDTreeAutomaton class.
 *
 *****************************************************************************/

#ifndef _SYMBOLIC_TD_TREE_AUTOMATON_HH_
#define _SYMBOLIC_TD_TREE_AUTOMATON_HH_

// SFTA headers
#include <sfta/abstract_td_tree_automaton.hh>
#include <sfta/ordered_vector.hh>

// Loki headers
#include <loki/SmartPtr.h>

// Standard library headers
#include <tr1/unordered_map>

// insert the class into proper namespace
namespace SFTA
{
	template
	<
		class MTBDDTransitionTableWrapper,
		typename State,
		typename Symbol,
		class InputRightHandSide,
		class OutputRightHandSide
	>
	class SymbolicTDTreeAutomaton;
}


/**
 * @brief   Symbolically represented top-down tree automaton
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Top-down tree automaton with symbolically represented transition function.
 */
template
<
	class MTBDDTransitionTableWrapper,
	typename State,
	typename Symbol,
	class InputRightHandSide,
	class OutputRightHandSide = InputRightHandSide
>
class SFTA::SymbolicTDTreeAutomaton
	: public SFTA::AbstractTDTreeAutomaton
		<
			State,
			Symbol,
			InputRightHandSide,
			OutputRightHandSide
		>
{
public:   // Public data types

	typedef SymbolicTDTreeAutomaton
		<
			MTBDDTransitionTableWrapper,
			State,
			Symbol,
			InputRightHandSide,
			OutputRightHandSide
		> Type;

	typedef State StateType;
	typedef Symbol SymbolType;

	typedef SFTA::AbstractTDTreeAutomaton
		<
			StateType,
			SymbolType,
			InputRightHandSide,
			OutputRightHandSide
		> ParentClass;

	typedef typename ParentClass::HierarchyRoot HierarchyRoot;

	typedef MTBDDTransitionTableWrapper MTBDDTTWrapperType;
	typedef Loki::SmartPtr<MTBDDTTWrapperType> TTWrapperPtrType;

	typedef typename MTBDDTTWrapperType::SharedMTBDDType::RootType
		RootType;

	typedef typename ParentClass::LeftHandSideType LeftHandSideType;

	typedef typename ParentClass::InputRightHandSideType InputRightHandSideType;
	typedef typename ParentClass::OutputRightHandSideType OutputRightHandSideType;


	typedef typename SFTA::OrderedVector
		<
			StateType
		> StateSetType;

	typedef typename std::tr1::unordered_map
		<
			StateType,
			RootType
		> LHSRootContainerType;


	/**
	 * @brief  Data type for transition rule of an automaton
	 *
	 * This type is used to store elements of transition rule of an automaton.
	 */
	struct Transition
	{
		LeftHandSideType lhs;
		SymbolType symbol;
		OutputRightHandSideType rhs;

		Transition(const LeftHandSideType& inLhs, const SymbolType& inSymbol,
			const OutputRightHandSideType& inRhs)
			: lhs(inLhs),
				symbol(inSymbol),
				rhs(inRhs)
		{ }
	};

	typedef Transition TransitionType;

	/**
	 * @brief  @copybrief SFTA::AbstractTDTreeAutomaton::Operation
	 *
	 * @copydetails SFTA::AbstractTDTreeAutomaton::Operation
	 */
	class Operation
		: public ParentClass::Operation
	{
	};

private:  // Private data types

	typedef SFTA::Private::Convert Convert;
	typedef typename MTBDDTTWrapperType::SharedMTBDDType SharedMTBDDType;
	typedef typename SharedMTBDDType::DescriptionType TransitionMapType;

private:  // Private data members

	StateSetType states_;

	StateSetType initialStates_;

	TTWrapperPtrType ttWrapper_;

	RootType sinkState_;

	LHSRootContainerType rootMap_;

protected:// Protected methods

	void copyStates(const Type& aut)
	{
		states_.insert(aut.states_);
		initialStates_.insert(aut.initialStates_);

		// also copy MTBDD root nodes
		rootMap_.insert(aut.rootMap_.begin(), aut.rootMap_.end());
	}

	void copyStates(const HierarchyRoot& aut)
	{
		const Type* autSym = static_cast<Type*>(0);

		if ((autSym = dynamic_cast<const Type*>(&aut)) !=
			static_cast<const Type*>(0))
		{
			return copyStates(*autSym);
		}

		throw std::runtime_error(__func__ + std::string(": Invalid types"));
	}

	inline RootType getSinkState() const
	{
		return sinkState_;
	}


protected:// Protected methods

	virtual Operation* createOperation() const = 0;

	inline bool isStateLocal(const StateType& state) const
	{
		return (states_.find(state) != states_.end());
	}


public:   // Public methods

	/**
	 * @brief  Default constructor
	 *
	 * Default constructor.
	 */
	SymbolicTDTreeAutomaton()
		: states_(),
			initialStates_(),
			ttWrapper_(new MTBDDTTWrapperType()),
			sinkState_(GetTTWrapper()->GetMTBDD()->CreateRoot()),
			rootMap_(sinkState_)
	{
		// Assertions
		assert(ttWrapper_ != static_cast<TTWrapperPtrType>(0));

		GetTTWrapper()->GetMTBDD()->SetBottomValue(InputRightHandSideType());
	}

	/**
	 * @brief  Copy constructor
	 *
	 * Copy constructor of the class.
	 *
	 * @param[in]  aut  The automaton to be copied
	 */
	SymbolicTDTreeAutomaton(const SymbolicTDTreeAutomaton& aut)
		: ParentClass(aut),
			states_(aut.states_),
			initialStates_(aut.initialStates_),
			ttWrapper_(aut.ttWrapper_),
			sinkState_(aut.sinkState_),
			rootMap_(aut.rootMap_)
	{
		// Assertions
		assert(ttWrapper_ != static_cast<TTWrapperPtrType>(0));
	}

	explicit SymbolicTDTreeAutomaton(TTWrapperPtrType ttWrapper)
		: states_(),
			initialStates_(),
			ttWrapper_(ttWrapper),
			sinkState_(GetTTWrapper()->GetMTBDD()->CreateRoot()),
			rootMap_(sinkState_)
	{
		// Assertions
		assert(ttWrapper_ != static_cast<TTWrapperPtrType>(0));
	}


	virtual void CopyStates(const HierarchyRoot& aut)
	{
		copyStates(aut);
	}

	virtual StateType AddState()
	{
		StateType newState = GetTTWrapper()->CreateState();
		states_.insert(newState);

		return newState;
	}

	virtual void SetStateInitial(const StateType& state)
	{
		// Assertions
		assert(isStateLocal(state));

		initialStates_.insert(state);
	}

	virtual bool IsStateInitial(const StateType& state) const
	{
		// Assertions
		assert(isStateLocal(state));

		return initialStates_.find(state) != initialStates_.end();
	}

	virtual void AddTransition(const LeftHandSideType& lhs,
		const SymbolType& symbol, const InputRightHandSideType& rhs)
	{
		// Assertions
		assert(isStateLocal(lhs));

		RootType root = sinkState_;

		typename LHSRootContainerType::const_iterator it;
		if ((it = rootMap_.find(lhs)) == rootMap_.end())
		{	// in case the value is not in the hash table
			root = GetTTWrapper()->GetMTBDD()->CreateRoot();
			rootMap_.insert(std::make_pair(lhs, root));
		}
		else
		{
			root = it->second;
		}

		GetTTWrapper()->GetMTBDD()->SetValue(root, symbol, rhs);
	}

	virtual OutputRightHandSideType GetTransition(const LeftHandSideType& lhs,
		const SymbolType& symbol)
	{
		// Assertions
		assert(isStateLocal(lhs));

		OutputRightHandSide rhs;

		typename LHSRootContainerType::const_iterator it;
		if ((it = rootMap_.find(lhs)) == rootMap_.end())
		{	// in case the value is not in the hash table
			return rhs;
		}
		else
		{
			RootType root = it->second;
			typename SharedMTBDDType::LeafContainer output =
				GetTTWrapper()->GetMTBDD()->GetValue(root, symbol);

			for (typename SharedMTBDDType::LeafContainer::const_iterator itCont =
				output.begin(); itCont != output.end(); ++itCont)
			{
				rhs.insert(**itCont);
			}

			return rhs;
		}
	}

	/**
	 * @brief  Returns a transition table wrapper
	 *
	 * This method returns given transition table wrapper of the automaton.
	 *
	 * @returns  Transition table wrapper
	 */
	virtual TTWrapperPtrType GetTTWrapper() const
	{
		// Assertions
		assert(ttWrapper_ != static_cast<TTWrapperPtrType>(0));

		return ttWrapper_;
	}

	virtual std::vector<TransitionType> GetVectorOfTransitions() const
	{
		typedef std::vector<TransitionType> TransitionVector;

		TransitionVector result;

		for (typename LHSRootContainerType::const_iterator itRoot = rootMap_.begin();
			itRoot != rootMap_.end(); ++itRoot)
		{
			TransitionMapType transMap =
				ttWrapper_->GetMTBDD()->GetMinimumDescription(itRoot->second);

			for (typename TransitionMapType::const_iterator itTrans = transMap.begin();
				itTrans != transMap.end(); ++itTrans)
			{
				if (!(itTrans->second.empty()))
				{
					result.push_back(
						TransitionType(itRoot->first, itTrans->first, itTrans->second));
				}
			}
		}

		return result;
	}

	virtual std::string ToString() const
	{
		std::string result;
		result += "Automaton\n";
		result += "States: " + Convert::ToString(states_) + "\n";
		result += "Initial states: " + Convert::ToString(initialStates_) + "\n";
		result += "Transitions: \n";

		typedef std::vector<TransitionType> TransitionVector;
		TransitionVector trans = GetVectorOfTransitions();
		for (typename TransitionVector::const_iterator itTrans = trans.begin();
			itTrans != trans.end(); ++itTrans)
		{
			result += Convert::ToString(itTrans->symbol);
			result += Convert::ToString(itTrans->lhs);
			result += " -> ";
			result += Convert::ToString(itTrans->rhs);
			result += "\n";
		}

		return result;
	}

	virtual std::vector<StateType> GetVectorOfStates() const
	{
		std::vector<StateType> result;

		for (typename StateSetType::const_iterator itStates = states_.begin();
			itStates != states_.end(); ++itStates)
		{
			result.push_back(*itStates);
		}

		return result;
	}

	virtual std::vector<StateType> GetVectorOfInitialStates() const
	{
		std::vector<StateType> result;

		for (typename StateSetType::const_iterator itStates = initialStates_.begin();
			itStates != initialStates_.end(); ++itStates)
		{
			result.push_back(*itStates);
		}

		return result;
	}
};

#endif
