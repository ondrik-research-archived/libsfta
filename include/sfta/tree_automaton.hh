/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    The file with TreeAutomaton class.
 *
 *****************************************************************************/

#ifndef _SFTA_TREE_AUTOMATON_HH_
#define _SFTA_TREE_AUTOMATON_HH_

// SFTA header files
#include <sfta/sfta.hh>
#include <sfta/abstract_transition_function.hh>
#include <sfta/vector.hh>

// Standard library header files
#include <set>

// Loki header files
#include <loki/SmartPtr.h>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename Symbol,
		typename State,
		class TransitionFunction,
		template <typename, typename> class StateTranslator
	>
	class TreeAutomaton;
}


template
<
	typename Symbol,
	typename State,
	class TransitionFunction,
	template <typename, typename> class StateTranslator
>
class SFTA::TreeAutomaton
	: private StateTranslator
		<
			State,
			typename TransitionFunction::StateType
		>
{
public:   // Public data types

	typedef Symbol SymbolType;
	typedef State AStateType;

	typedef typename TransitionFunction::StateType TFStateType;

	typedef TransitionFunction TransitionFunctionType;
	typedef typename TransitionFunctionType::TransitionListType TransitionListType;

	// uses ref counting
	typedef Loki::SmartPtr<TransitionFunctionType> TransFuncPtrType;

	typedef SFTA::Vector<AStateType> RuleLeftHandSideType;

	typedef std::set<AStateType> SetOfStatesType;

private:  // Private data types

	typedef SFTA::Private::Convert Convert;

	typedef StateTranslator
	<
		AStateType,
		TFStateType
	>
	ST;

	typedef std::set<TFStateType> SetOfTFStates;

private:  // Private data members

	TransFuncPtrType transFunc_;

	typename TransitionFunctionType::RegistrationTokenType regToken_;

	SetOfTFStates finalStates_;

	/**
	 * @brief  The name of the Log4cpp category for logging
	 *
	 * The name of the Log4cpp category used for logging messages from this
	 * class.
	 */
	static const char* LOG_CATEGORY_NAME;

public:   // Public methods

	TreeAutomaton()
		: transFunc_(new TransitionFunctionType()),
		  regToken_(),
			finalStates_()
	{
		regToken_ = transFunc_->RegisterAutomaton();
	}



	explicit TreeAutomaton(TransFuncPtrType transFunction)
		: transFunc_(transFunction),
		  regToken_(),
			finalStates_()
	{
		regToken_ = transFunc_->RegisterAutomaton();
	}


	void AddState(const AStateType& state)
	{
		TFStateType tfState = transFunc_->AllocateState(regToken_);
		ST::AddTranslation(state, tfState);
	}

	void MapStateToTF(const AStateType& aState, const TFStateType& tfState)
	{
		ST::AddTranslation(aState, tfState);
	}


	void AddTransition(const SymbolType& symbol,
		const RuleLeftHandSideType& lhs, const SetOfStatesType& rhs)
	{
		typename TransitionFunctionType::LeftHandSideType tfLhs(lhs.size());
		for (size_t i = 0; i < lhs.size(); ++i)
		{	// transform all states in LHS to their respective equivalent in the TF
			tfLhs[i] = ST::TranslateA2TF(lhs[i]);
		}

		typename TransitionFunctionType::InputRightHandSideType tfRhs;
		for (typename SetOfStatesType::const_iterator it = rhs.begin();
			it != rhs.end(); ++it)
		{	// transform all states in RHS to their respective equivalent in the TF
			tfRhs.push_back(ST::TranslateA2TF(*it));
		}

		transFunc_->AddTransition(regToken_, symbol, tfLhs, tfRhs);
	}

	void SetStateFinal(const AStateType& state)
	{
		TFStateType tfState = ST::TranslateA2TF(state);
		finalStates_.insert(tfState);
	}

	void SetTFStateFinal(const TFStateType& tfState)
	{
		finalStates_.insert(tfState);
	}

	bool IsTFStateFinal(const TFStateType& state) const
	{
		return (finalStates_.find(state) != finalStates_.end());
	}


	inline TransFuncPtrType GetTransitionFunction()
	{
		return transFunc_;
	}

	inline typename TransitionFunctionType::RegistrationTokenType
		GetRegToken() const
	{
		return regToken_;
	}

	inline std::vector<TFStateType> GetTFStates() const
	{
		return ST::GetAllTFStates();
	}

	inline bool ContainsTFState(const TFStateType& tfState) const
	{
		return ST::ContainsTFState(tfState);
	}

	std::string ToString()
	{
		typename TransitionFunctionType::TransitionListType lst
			= transFunc_->GetListOfTransitions(regToken_);

		std::string result;

		// print states
		result += "States";
		std::vector<TFStateType> states = ST::GetAllTFStates();
		SFTA_LOGGER_DEBUG("States: " + Convert::ToString(states.size()));
		for (typename std::vector<TFStateType>::const_iterator it = states.begin();
			it != states.end(); ++it)
		{
			result += " " + Convert::ToString(*it);
		}

		result += "\n";

		// print final states
		result += "Final States";
		for (typename SetOfTFStates::const_iterator it = finalStates_.begin();
			it != finalStates_.end(); ++it)
		{
			result += " " + Convert::ToString(*it);
		}

		result += "\n";

		// print transitions
		result += "Transitions\n";

		for (typename TransitionListType::const_iterator it = lst.begin();
			it != lst.end(); ++it)
		{	// for each transition of the transition function
			typename TransitionFunctionType::LeftHandSideType lhs
				= Loki::Field<1>(*it);

//			bool foundInvalid = false;
//			for (typename TransitionFunctionType::LeftHandSideType::const_iterator jt
//				= lhs.begin(); jt != lhs.end(); ++jt)
//			{	// check that all states are from this automaton
//				if (!ContainsTFState(*jt))
//				{
//					foundInvalid = true;
//					break;
//				}
//			}
//
//			if (foundInvalid)
//			{	// in case an invalid state was found, skip this transition
//				continue;
//			}

			result += Convert::ToString(Loki::Field<0>(*it)) + "(";

			if (lhs.size() > 0)
			{	// if the symbol is more than nullary
				//result += Convert::ToString(ST::TranslateTF2A(lhs[0]));
				result += Convert::ToString(lhs[0]);

				for (size_t i = 1; i < lhs.size(); ++i)
				{
					//result += ", " + Convert::ToString(ST::TranslateTF2A(lhs[i]));
					result += ", " + Convert::ToString(lhs[i]);
				}
			}

			const typename TransitionFunctionType::InputRightHandSideType& rhs
				= Loki::Field<2>(*it);

			// assertion
			assert(rhs.size() > 0);

			result += ") -> {";
			//result += Convert::ToString(ST::TranslateTF2A(rhs[0]));
			result += Convert::ToString(rhs[0]);
			for (size_t i = 1; i < rhs.size(); ++i)
			{
				//result += ", " + Convert::ToString(ST::TranslateTF2A(rhs[i]));
				result += ", " + Convert::ToString(rhs[i]);
			}

			result += "}\n";
		}

		return result;
	}


	~TreeAutomaton()
	{
		transFunc_->UnregisterAutomaton(regToken_);
	}

};


// Setting the logging category name for Log4cpp
template
<
	typename Sy,
	typename St,
	class TF,
	template <typename, typename> class ST
>
const char* SFTA::TreeAutomaton<Sy, St, TF, ST>::LOG_CATEGORY_NAME = "tree_automaton";

#endif
