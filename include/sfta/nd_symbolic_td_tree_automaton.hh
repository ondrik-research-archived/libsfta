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
		template <typename> class RightHandSide
	>
	class NDSymbolicTDTreeAutomaton;
}


// forward declaration
template
<
	class MTBDDTransitionTableWrapper,
	typename State,
	typename Symbol,
	template <typename> class RightHandSide
>
class NDSymbolicBUTreeAutomaton;


namespace SFTA
{
	namespace Private
	{
		template <typename T>
		class ElemOrVector
		{
		public:
			typedef T Type;
			typedef SFTA::Vector<T> VectorType;

		private:
			bool isElem;               // if true, elem is valid, if false, elemVector is valid
			Type elem;
			VectorType elemVector;

		public:
			ElemOrVector()
				: isElem(true), elem(), elemVector()
			{	}

			ElemOrVector(const Type& el)
				: isElem(true), elem(el), elemVector()
			{ }

			ElemOrVector(const VectorType& elVec)
				: isElem(false), elem(), elemVector(elVec)
			{ }

			inline bool IsElement() const
			{
				return isElem;
			}

			const Type& GetElement() const
			{
				if (!isElem)
				{
					throw std::runtime_error(__func__ +
						std::string(": an attempt to get an element from vector"));
				}

				return elem;
			}

			const VectorType& GetVector() const
			{
				if (isElem)
				{
					throw std::runtime_error(__func__ +
						std::string(": an attempt to get a vector from element"));
				}
				return elemVector;
			}

			friend bool operator<(const ElemOrVector<T>& lhs, const ElemOrVector<T>& rhs)
			{
				if (lhs.isElem && !rhs.isElem)
				{
					return true;    // elements are smaller than vectors
				}
				else if (!lhs.isElem && rhs.isElem)
				{
					return false;   // elements are smaller than vectors
				}
				else if (lhs.isElem)
				{
					return lhs.elem < rhs.elem;
				}
				else
				{
					return lhs.elemVector < rhs.elemVector;
				}
			}

			friend bool operator==(const ElemOrVector<T>& lhs, const ElemOrVector<T>& rhs)
			{
				if ((lhs.isElem && !rhs.isElem) || (!lhs.isElem && rhs.isElem))
				{
					return false;
				}
				else if (lhs.isElem)
				{
					return lhs.elem == rhs.elem;
				}
				else
				{
					return lhs.elemVector == rhs.elemVector;
				}
			}

			friend std::ostream& operator<<(std::ostream& os, const ElemOrVector& eov)
			{
				if (eov.isElem)
				{
					os << eov.elem;
				}
				else
				{
					os << eov.elemVector;
				}

				return os;
			}
		};
	}
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
 * @tparam  RightHandSide                Type of the left-hand side of
 *                                       automaton rules.
 */
template
<
	class MTBDDTransitionTableWrapper,
	typename State,
	typename Symbol,
	template <typename> class RightHandSide
>
class SFTA::NDSymbolicTDTreeAutomaton
	: public SFTA::SymbolicTDTreeAutomaton
		<
			MTBDDTransitionTableWrapper,
			State,
			Symbol,
			RightHandSide<SFTA::Private::ElemOrVector<State> >
		>
{
public:   // Public data types

	template < class, typename, typename, template <typename> class >
		friend class NDSymbolicBUTreeAutomaton;

	typedef NDSymbolicTDTreeAutomaton
		<
			MTBDDTransitionTableWrapper,
			State,
			Symbol,
			RightHandSide
		> Type;

	typedef SymbolicTDTreeAutomaton
		<
			MTBDDTransitionTableWrapper,
			State,
			Symbol,
			RightHandSide<SFTA::Private::ElemOrVector<State> >
		> ParentClass;

	typedef typename ParentClass::HierarchyRoot HierarchyRoot;

	typedef typename ParentClass::StateType StateType;
	typedef typename ParentClass::LeftHandSideType LeftHandSideType;

	typedef typename ParentClass::RightHandSideType RightHandSideType;

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
			assert(aut != static_cast<const HierarchyRoot*>(0));

			throw std::runtime_error(__func__ + std::string(": not implemented"));
		}


		virtual bool CheckLanguageInclusion(const HierarchyRoot* a1,
			const HierarchyRoot* a2) const
		{
			const Type* a1Sym = static_cast<Type*>(0);
			const Type* a2Sym = static_cast<Type*>(0);

			if ((a1Sym = dynamic_cast<const Type*>(a1)) ==
				static_cast<const Type*>(0))
			{	// in case the type is not OK
				throw std::runtime_error(__func__ + std::string(": Invalid type"));
			}

			if ((a2Sym = dynamic_cast<const Type*>(a2)) ==
				static_cast<const Type*>(0))
			{	// in case the type is not OK
				throw std::runtime_error(__func__ + std::string(": Invalid type"));
			}

			// array of states
			std::vector<StateType> a1InitStates = a1Sym->GetVectorOfInitialStates();



			assert(false);
			return true;
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
			RightHandSideType());
	}

	NDSymbolicTDTreeAutomaton(const NDSymbolicTDTreeAutomaton& aut)
		: ParentClass(aut)
	{ }

	explicit NDSymbolicTDTreeAutomaton(TTWrapperPtrType ttWrapper)
		: ParentClass(ttWrapper)
	{ }

};

#endif
