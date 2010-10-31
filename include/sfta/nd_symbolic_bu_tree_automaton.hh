/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for NDSymbolicBUTreeAutomaton class.
 *
 *****************************************************************************/

#ifndef _ND_SYMBOLIC_BU_TREE_AUTOMATON_HH_
#define _ND_SYMBOLIC_BU_TREE_AUTOMATON_HH_

// SFTA headers
#include <sfta/symbolic_bu_tree_automaton.hh>

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
	class NDSymbolicBUTreeAutomaton;
}


template
<
	class MTBDDTransitionTableWrapper,
	typename State,
	typename Symbol,
	template <typename> class InputRightHandSide,
	template <typename> class OutputRightHandSide = InputRightHandSide
>
class SFTA::NDSymbolicBUTreeAutomaton
	: public SFTA::SymbolicBUTreeAutomaton
		<
			MTBDDTransitionTableWrapper,
			State,
			Symbol,
			InputRightHandSide<State>,
			OutputRightHandSide<State>
		>
{
private:  // Private data types

	typedef typename SFTA::Private::Convert Convert;

public:   // Public data types

	typedef NDSymbolicBUTreeAutomaton
		<
			MTBDDTransitionTableWrapper,
			State,
			Symbol,
			InputRightHandSide,
			OutputRightHandSide
		> Type;

	typedef SymbolicBUTreeAutomaton
		<
			MTBDDTransitionTableWrapper,
			State,
			Symbol,
			InputRightHandSide<State>,
			OutputRightHandSide<State>
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


	class Operation
		: public ParentClass::Operation
	{
	public:   // Public data types

		typedef typename SharedMTBDDType::RootType RootType;
		typedef typename SharedMTBDDType::LeafType LeafType;


	protected:// Protected methods

		virtual Type* Union( const Type& a1, const Type& a2) const
		{
			if (a1.GetTTWrapper() != a2.GetTTWrapper())
			{
				throw std::runtime_error(__func__ +
					std::string(": trying to perform operation on automata "
						"with different transition table wrapper"));
			}

			Type* result = new Type(a1);

			result->CopyStates(a2);

			RootType lhsMtbdd = a1.getRoot(LeftHandSideType());
			RootType rhsMtbdd = a2.getRoot(LeftHandSideType());

			class UnionApplyFunctor
				: public SharedMTBDDType::AbstractApplyFunctorType
			{
				virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs)
				{
					return lhs.Union(rhs);
				}
			};

			UnionApplyFunctor unionFunc;
			RootType resultRoot = result->GetTTWrapper()->GetMTBDD()->Apply(
				lhsMtbdd, rhsMtbdd, &unionFunc);

			result->setRoot(LeftHandSideType(), resultRoot);

			return result;
		}

	public:   // Public methods

		virtual Type* Union(const HierarchyRoot* a1, const HierarchyRoot* a2) const
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
					return Union(*a1Sym, *a2Sym);
				}
			}

			throw std::runtime_error(__func__ + std::string(": Invalid types"));
		}
	};

protected:// Protected methods

	virtual Operation* createOperation() const
	{
		return new Operation();
	}


public:   // Public methods

	NDSymbolicBUTreeAutomaton()
	{
		ParentClass::GetTTWrapper()->GetMTBDD()->SetValue(
			ParentClass::getSinkSuperState(), Symbol::GetUniversalSymbol(),
			InputRightHandSideType());
	}

	NDSymbolicBUTreeAutomaton(const NDSymbolicBUTreeAutomaton& aut)
		: ParentClass(aut)
	{ }

	NDSymbolicBUTreeAutomaton(TTWrapperPtrType ttWrapper)
		: ParentClass(ttWrapper)
	{ }
};

#endif
