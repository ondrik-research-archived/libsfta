/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for NDSymbolicTDTreeAutomaton class.
 *
 *****************************************************************************/

#ifndef _ND_SYMBOLIC_TD_TREE_AUTOMATON_HH_
#define _ND_SYMBOLIC_TD_TREE_AUTOMATON_HH_

// SFTA headers
#include <sfta/symbolic_td_tree_automaton.hh>
#include <sfta/vector.hh>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		class MTBDDTransitionTableWrapper,
		typename State,
		typename Symbol,
		template <typename> class InputRightHandSide,
		template <typename> class OutputRightHandSide
	>
	class NDSymbolicTDTreeAutomaton;
}


/**
 * @brief   Nondeterministic symbolic top-down finite tree automaton class
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * This class represents a symbolically encoded nondeterministic top-down
 * finite tree automaton.
 *
 * @tparam  MTBDDTransitionTableWrapper  Type of MTBDD wrapper of a transition
 *                                       table.
 * @tparam  State                        Type of states of the automaton.
 * @tparam  Symbol                       Type of symbols of the alphabet of
 *                                       the automaton.
 * @tparam  InputRightHandSide           Type of the left-hand side of
 *                                       automaton rules that is used for input.
 * @tparam  OutputRightHandSide          Type of the left-hand side of
 *                                       automaton rules that is used for output.
 */
template
<
	class MTBDDTransitionTableWrapper,
	typename State,
	typename Symbol,
	template <typename> class InputRightHandSide,
	template <typename> class OutputRightHandSide = InputRightHandSide
>
class SFTA::NDSymbolicTDTreeAutomaton
	: public SFTA::SymbolicTDTreeAutomaton
		<
			MTBDDTransitionTableWrapper,
			State,
			Symbol,
			InputRightHandSide<SFTA::Vector<State> >,
			OutputRightHandSide<SFTA::Vector<State> >
		>
{
public:   // Public data types

	typedef NDSymbolicTDTreeAutomaton
		<
			MTBDDTransitionTableWrapper,
			State,
			Symbol,
			InputRightHandSide,
			OutputRightHandSide
		> Type;

	typedef SymbolicTDTreeAutomaton
		<
			MTBDDTransitionTableWrapper,
			State,
			Symbol,
			InputRightHandSide<SFTA::Vector<State> >,
			OutputRightHandSide<SFTA::Vector<State> >
		> ParentClass;

	typedef typename ParentClass::HierarchyRoot HierarchyRoot;

	typedef typename ParentClass::StateType StateType;
	typedef typename ParentClass::LeftHandSideType LeftHandSideType;

	typedef typename ParentClass::InputRightHandSideType InputRightHandSideType;
	typedef typename ParentClass::OutputRightHandSideType OutputRightHandSideType;

	typedef typename ParentClass::LHSRootContainerType LHSRootContainerType;

	typedef typename ParentClass::MTBDDTTWrapperType MTBDDTTWrapperType;
	typedef typename MTBDDTTWrapperType::SharedMTBDDType SharedMTBDDType;
	typedef typename ParentClass::TTWrapperPtrType TTWrapperPtrType;


	/**
	 * @brief  @copybrief SFTA::SymbolicTDTreeAutomaton::Operation
	 *
	 * @copydetails SFTA::SymbolicTDTreeAutomaton::Operation
	 */
	class Operation
		: public ParentClass::Operation
	{
	private:  // Private data types

		typedef typename SharedMTBDDType::RootType RootType;
		typedef typename SharedMTBDDType::LeafType LeafType;

		typedef Type* (Operation::*BinaryOperation)(const Type&, const Type&) const;

	private:  // Private methods

		Type* safelyPerformOperation(BinaryOperation oper,
			const HierarchyRoot* a1, const HierarchyRoot* a2) const
		{
			// Assertions
			assert(a1 != static_cast<Type*>(0));
			assert(a2 != static_cast<Type*>(0));

			const Type* a1Sym = static_cast<Type*>(0);
			const Type* a2Sym = static_cast<Type*>(0);

			if ((a1Sym = dynamic_cast<const Type*>(a1)) !=
				static_cast<const Type*>(0))
			{
				if ((a2Sym = dynamic_cast<const Type*>(a2)) !=
					static_cast<const Type*>(0))
				{	// in case the types are OK
					if (a1Sym->GetTTWrapper() != a2Sym->GetTTWrapper())
					{
						throw std::runtime_error(__func__ +
							std::string(": trying to perform operation on automata "
								"with different transition table wrapper"));
					}

					return (this->*oper)(*a1Sym, *a2Sym);
				}
			}

			throw std::runtime_error(__func__ + std::string(": Invalid types"));
		}

		Type* langUnion(const Type& a1, const Type& a2) const
		{
			Type* result = new Type(a1);
			result->CopyStates(a2);

			return result;
		}

		Type* langIntersection(const Type& a1, const Type& a2) const
		{
			assert(&a1 != 0);
			assert(&a2 != 0);
			assert(false);
		}


	public:   // Public methods

		virtual Type* Union(const HierarchyRoot* a1, const HierarchyRoot* a2) const
		{
			return safelyPerformOperation(&Operation::langUnion, a1, a2);
		}

		virtual Type* Intersection(const HierarchyRoot* a1, const HierarchyRoot* a2) const
		{
			return safelyPerformOperation(&Operation::langIntersection, a1, a2);
		}

		virtual typename HierarchyRoot::Operation::SimulationRelationType*
			ComputeSimulationPreorder(const HierarchyRoot* aut) const
		{
			assert(false);
		}
	};

protected:// Protected methods

	virtual Operation* createOperation() const
	{
		return new Operation();
	}


public:   // Public methods

	NDSymbolicTDTreeAutomaton()
	{
		ParentClass::GetTTWrapper()->GetMTBDD()->SetValue(
			ParentClass::getSinkState(), Symbol::GetUniversalSymbol(),
			InputRightHandSideType());
	}

	NDSymbolicTDTreeAutomaton(const NDSymbolicTDTreeAutomaton& aut)
		: ParentClass(aut)
	{ }

	explicit NDSymbolicTDTreeAutomaton(TTWrapperPtrType ttWrapper)
		: ParentClass(ttWrapper)
	{ }

};

#endif
