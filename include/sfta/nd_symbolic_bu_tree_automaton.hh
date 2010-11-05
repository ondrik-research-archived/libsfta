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

// Standard library headers
#include <queue>
#include <tr1/unordered_map>

// Boost library headers
#include <boost/functional/hash.hpp>


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
	private:  // Private data types

		typedef typename SharedMTBDDType::RootType RootType;
		typedef typename SharedMTBDDType::LeafType LeafType;

		typedef Type* (Operation::*BinaryOperation)(const Type&, const Type&) const;

		struct HasherPair
		{
			template <typename T>
			size_t operator()(const std::pair<T, T>& key) const
			{
				size_t seed  = 0;
				boost::hash_combine(seed, key.first);
				boost::hash_combine(seed, key.second);
				return seed;
			}
		};

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
			class UnionApplyFunctor
				: public SharedMTBDDType::AbstractApplyFunctorType
			{
			public:
				virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs)
				{
					return lhs.Union(rhs);
				}
			};


			Type* result = new Type(a1);
			result->CopyStates(a2);

			RootType lhsMtbdd = a1.getRoot(LeftHandSideType());
			RootType rhsMtbdd = a2.getRoot(LeftHandSideType());

			UnionApplyFunctor unionFunc;
			RootType resultRoot = result->GetTTWrapper()->GetMTBDD()->Apply(
				lhsMtbdd, rhsMtbdd, &unionFunc);

			result->setRoot(LeftHandSideType(), resultRoot);

			return result;
		}


		Type* langIntersection(const Type& a1, const Type& a2) const
		{
			typedef std::pair<StateType, StateType> StatePair;
			typedef std::pair<StatePair, StateType> StatePairToState;
			typedef std::queue<StatePairToState> NewStatesQueueType;

			typedef std::tr1::unordered_map<StatePair, StateType, HasherPair>
				StatePairToStateTable;

			class IntersectionApplyFunctor
				: public SharedMTBDDType::AbstractApplyFunctorType
			{
			private:  // Private data members

				Type* resultAutomaton_;
				NewStatesQueueType* newStates_;
				StatePairToStateTable* productStatesTable_;

			private:  // Private methods

				IntersectionApplyFunctor(const IntersectionApplyFunctor&);
				IntersectionApplyFunctor& operator=(const IntersectionApplyFunctor&);

			public:   // Public methods

				IntersectionApplyFunctor(Type* resultAutomaton,
					NewStatesQueueType* newStates,
					StatePairToStateTable* productStatesTable)
					: resultAutomaton_(resultAutomaton),
						newStates_(newStates),
						productStatesTable_(productStatesTable)
				{
					// Assertions
					assert(resultAutomaton_ != static_cast<Type*>(0));
					assert(newStates_ != static_cast<NewStatesQueueType*>(0));
					assert(productStatesTable_ != static_cast<StatePairToStateTable*>(0));
				}

				virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs)
				{
					// Assertions
					assert(resultAutomaton_ != static_cast<Type*>(0));
					assert(newStates_ != static_cast<NewStatesQueueType*>(0));
					assert(productStatesTable_ != static_cast<StatePairToStateTable*>(0));

					LeafType result;

					for (typename LeafType::const_iterator lhsIt = lhs.begin();
						lhsIt != lhs.end(); ++lhsIt)
					{
						for (typename LeafType::const_iterator rhsIt = rhs.begin();
							rhsIt != rhs.end(); ++rhsIt)
						{
							StatePair productState = std::make_pair(*lhsIt, *rhsIt);
							StateType resultState;

							typename StatePairToStateTable::const_iterator itPairs;
							if ((itPairs = productStatesTable_->find(productState))
								== productStatesTable_->end())
							{	// in case the product state is new
								resultState = resultAutomaton_->AddState();

								if (!(productStatesTable_->insert(std::make_pair(
									productState, resultState))).second)
								{
									throw std::logic_error(__func__ +
										std::string(": inserted value found!"));
								}

								newStates_->push(std::make_pair(productState, resultState));
							}
							else
							{
								resultState = itPairs->second;
							}

							result.insert(resultState);
						}
					}

					return result;
				}
			};


			// create structure for output automaton
			Type* result = new Type(a1.GetTTWrapper());

			// create used data structures
			NewStatesQueueType newStates;
			StatePairToStateTable productStatesTable;
			IntersectionApplyFunctor intersectionFunc(result, &newStates,
				&productStatesTable);

			// get rules for leaves
			RootType lhsMtbdd = a1.getRoot(LeftHandSideType());
			RootType rhsMtbdd = a2.getRoot(LeftHandSideType());

			// carry out the initial apply operation on leaves
			RootType resultRoot = result->GetTTWrapper()->GetMTBDD()->Apply(
				lhsMtbdd, rhsMtbdd, &intersectionFunc);
			result->setRoot(LeftHandSideType(), resultRoot);

			while (!newStates.empty())
			{	// until we process all states
				StatePair productState = newStates.front().first;
				StateType resultState = newStates.front().second;
				newStates.pop();

				if (a1.IsStateFinal(productState.first) &&
					a2.IsStateFinal(productState.second))
				{
					result->SetStateFinal(resultState);
				}

				typename LHSRootContainerType::IndexValueArray a1Lhss =
					a1.getRootMap().GetItemsWith(productState.first, a1.getStates());

				typename LHSRootContainerType::IndexValueArray a2Lhss =
					a2.getRootMap().GetItemsWith(productState.second, a2.getStates());


				for (size_t arity = 0;
					(arity < a1Lhss.size()) && (arity < a2Lhss.size()); ++arity)
				{	// for each arity of left-hand side in A1 and A2
					for (size_t a1index = 0; a1index < a1Lhss[arity].size(); ++a1index)
					{	// for each left-hand side of given arity in A1
						for (size_t a2index = 0; a2index < a2Lhss[arity].size(); ++a2index)
						{	// for each left-hand side of given arity in A2
							const LeftHandSideType& a1candidate = a1Lhss[arity][a1index].first;
							const LeftHandSideType& a2candidate = a2Lhss[arity][a2index].first;
							// Assertions
							assert(a1candidate.size() == arity);
							assert(a2candidate.size() == arity);

							LeftHandSideType newLhs;
							for (size_t arityIndex = 0; arityIndex < arity; ++arityIndex)
							{	// check if respective states have product state
								typename StatePairToStateTable::const_iterator itTable;
								if ((itTable = productStatesTable.find(std::make_pair(
									a1candidate[arityIndex], a2candidate[arityIndex]))) !=
									productStatesTable.end())
								{
									newLhs.push_back(itTable->second);
								}
								else
								{
									break;
								}
							}

							if (newLhs.size() == arity)
							{	// in case all positions match
								// get rules for leaves
								lhsMtbdd = a1.getRoot(a1Lhss[arity][a1index].first);
								rhsMtbdd = a2.getRoot(a2Lhss[arity][a2index].first);

								// carry out the apply operation on leaves
								resultRoot = result->GetTTWrapper()->GetMTBDD()->Apply(
									lhsMtbdd, rhsMtbdd, &intersectionFunc);
								result->setRoot(newLhs, resultRoot);
							}
						}
					}
				}
			}


			return result;
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

	explicit NDSymbolicBUTreeAutomaton(TTWrapperPtrType ttWrapper)
		: ParentClass(ttWrapper)
	{ }
};

#endif
