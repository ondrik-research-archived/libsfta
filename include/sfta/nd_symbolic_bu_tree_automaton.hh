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
			InputRightHandSide,
			OutputRightHandSide
		>
{
public:   // Public data types

	typedef SFTA::SymbolicBUTreeAutomaton
		<
			MTBDDTransitionTableWrapper,
			State,
			Symbol,
			InputRightHandSide,
			OutputRightHandSide
		> ParentClass;

	typedef typename ParentClass::HierarchyRoot HierarchyRoot;


	typedef NDSymbolicBUTreeAutomaton
		<
			MTBDDTransitionTableWrapper,
			State,
			Symbol,
			InputRightHandSide,
			OutputRightHandSide
		> Type;

	typedef typename ParentClass::MTBDDTransitionTableWrapperType
		MTBDDTransitionTableWrapperType;

	typedef typename MTBDDTransitionTableWrapperType::AbstractSharedMTBDDType
		AbstractSharedMTBDDType;

	typedef typename ParentClass::LeftHandSideType LeftHandSideType;

	class Operation
		: public ParentClass::Operation
	{
	public:   // Public data types

		typedef typename AbstractSharedMTBDDType::RootType RootType;
		typedef typename AbstractSharedMTBDDType::LeafType LeafType;


	protected:// Protected methods

		virtual Type* Union( const Type& a1, const Type& a2) const
		{
			Type* result = new Type(a1);

			result->InsertStates(a2);

			RootType lhsMtbdd = a1.getRoot(LeftHandSideType());
			RootType rhsMtbdd = a2.getRoot(LeftHandSideType());

			class UnionApplyFunctor
				: public AbstractSharedMTBDDType::AbstractApplyFunctorType
			{
				virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs)
				{
					assert(&rhs != 0);
					return lhs;

				}
			};

			UnionApplyFunctor unionFunc;
			result->getTTWrapper()->GetMTBDD()->Apply(lhsMtbdd, rhsMtbdd, &unionFunc);


			assert(result != static_cast<Type*>(0));

			assert(false);
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

	virtual Operation* CreateOperation() const
	{
		return new Operation();
	}

public:   // Public methods

	NDSymbolicBUTreeAutomaton()
		: ParentClass()
	{
		// TODO @todo
		assert(false);
	}

	NDSymbolicBUTreeAutomaton(const NDSymbolicBUTreeAutomaton& aut)
		: ParentClass(aut)
	{
		// TODO @todo
		assert(false);
	}

};

#endif
