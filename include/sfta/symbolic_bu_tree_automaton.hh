/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file with SymbolicBUTreeAutomaton class.
 *
 *****************************************************************************/

#ifndef _SYMBOLIC_BU_TREE_AUTOMATON_HH_
#define _SYMBOLIC_BU_TREE_AUTOMATON_HH_

// SFTA headers
#include <sfta/abstract_bu_tree_automaton.hh>
#include <sfta/ordered_vector.hh>
#include <sfta/vector_map.hh>

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
	class SymbolicBUTreeAutomaton;
}


/**
 * @brief   Symbolically represented bottom-up tree automaton
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Bottom-up tree automaton with symbolically represented transition function.
 */
template
<
	class MTBDDTransitionTableWrapper,
	typename State,
	typename Symbol,
	class InputRightHandSide,
	class OutputRightHandSide = InputRightHandSide
>
class SFTA::SymbolicBUTreeAutomaton
	: public SFTA::AbstractBUTreeAutomaton
		<
			State,
			Symbol,
			InputRightHandSide,
			OutputRightHandSide
		>
{
public:   // Public data types

	typedef SymbolicBUTreeAutomaton
		<
			MTBDDTransitionTableWrapper,
			State,
			Symbol,
			InputRightHandSide,
			OutputRightHandSide
		> Type;

	typedef State StateType;
	typedef Symbol SymbolType;

	typedef SFTA::AbstractBUTreeAutomaton
		<
			StateType,
			SymbolType,
			InputRightHandSide,
			OutputRightHandSide
		> ParentClass;

	typedef typename ParentClass::HierarchyRoot HierarchyRoot;

	typedef MTBDDTransitionTableWrapper MTBDDTTWrapperType;

	typedef typename MTBDDTTWrapperType::SharedMTBDDType::RootType
		RootType;

	typedef typename ParentClass::LeftHandSideType LeftHandSideType;

	typedef typename ParentClass::InputRightHandSideType InputRightHandSideType;
	typedef typename ParentClass::OutputRightHandSideType OutputRightHandSideType;

	typedef typename SFTA::OrderedVector
		<
			StateType
		> StateSetType;

	typedef typename SFTA::VectorMap
		<
			StateType,
			RootType
		> LHSRootContainer;

	class Operation
		: public ParentClass::Operation
	{
	};

private:  // Private data types

	typedef SFTA::Private::Convert Convert;
	typedef typename MTBDDTTWrapperType::SharedMTBDDType::DescriptionType
		TransitionMapType;


private:  // Private data members

	StateSetType states_;

	MTBDDTTWrapperType* ttWrapper_;

	RootType sinkSuperState_;

	LHSRootContainer rootMap_;

private:  // Private methods

	SymbolicBUTreeAutomaton& operator=(const SymbolicBUTreeAutomaton& aut);

protected:// Protected methods


	virtual Operation* createOperation() const = 0;

	inline RootType getRoot(const LeftHandSideType& lhs) const
	{
		return rootMap_.GetValue(lhs);
	}

	inline void setRoot(const LeftHandSideType& lhs, RootType root)
	{
		rootMap_.SetValue(lhs, root);
	}

	void copyStates(const Type& aut)
	{
		states_.insert(aut.states_);

		// also copy superstates
		rootMap_.insert(aut.rootMap_);
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

	inline RootType getSinkSuperState() const
	{
		return sinkSuperState_;
	}

public:   // Public methods

	/**
	 * @brief  Default constructor
	 *
	 * Default constructor.
	 */
	SymbolicBUTreeAutomaton()
		: states_(),
			ttWrapper_(new MTBDDTTWrapperType()),
			sinkSuperState_(getTTWrapper()->GetMTBDD()->CreateRoot()),
			rootMap_(sinkSuperState_)
	{
		// Assertions
		assert(ttWrapper_ != static_cast<MTBDDTTWrapperType*>(0));
	}

	/**
	 * @brief  Copy constructor
	 *
	 * Copy constructor of the class.
	 *
	 * @param[in]  aut  The automaton to be copied
	 */
	SymbolicBUTreeAutomaton(const SymbolicBUTreeAutomaton& aut)
		: ParentClass(aut),
			states_(aut.states_),
			ttWrapper_(aut.ttWrapper_),
			sinkSuperState_(aut.sinkSuperState_),
			rootMap_(aut.rootMap_)
	{
		// Assertions
		assert(ttWrapper_ != static_cast<MTBDDTTWrapperType*>(0));
	}

	virtual void CopyStates(const HierarchyRoot& aut)
	{
		copyStates(aut);
	}



	virtual MTBDDTTWrapperType* GetTTWrapper() const
	{
		// Assertions
		assert(ttWrapper_ != static_cast<MTBDDTTWrapperType*>(0));

		return ttWrapper_;
	}


	virtual std::string ToString() const
	{
		std::string result;
		result += "Automaton\n";
		result += "States: " + Convert::ToString(states_) + "\n";
		result += "Transitions: \n";

		for (typename LHSRootContainer::const_iterator itRoot = rootMap_.begin();
			itRoot != rootMap_.end(); ++itRoot)
		{
			TransitionMapType transMap =
				ttWrapper_->GetMTBDD()->GetMinimumDescription(itRoot->second);

			for (typename TransitionMapType::const_iterator itTrans = transMap.begin();
				itTrans != transMap.end(); ++itTrans)
			{
				result += Convert::ToString(itTrans->first);
				result += Convert::ToString(itRoot->first);
				result += " -> ";
				result += Convert::ToString(itTrans->second);
				result += "\n";
			}
		}

		return result;
	}
};

#endif
