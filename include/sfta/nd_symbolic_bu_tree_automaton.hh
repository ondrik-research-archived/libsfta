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
#include <sfta/inflatable_vector.hh>
#include <sfta/symbolic_bu_tree_automaton.hh>
#include <sfta/nd_symbolic_td_tree_automaton.hh>

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
		template <typename> class RightHandSide
	>
	class NDSymbolicBUTreeAutomaton;
}


/**
 * @brief   Nondeterministic symbolic bottom-up finite tree automaton class
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * This class represents a symbolically encoded nondeterministic bottom-up
 * finite tree automaton.
 *
 * @tparam  MTBDDTransitionTableWrapper  Type of MTBDD wrapper of a transition
 *                                       table.
 * @tparam  State                        Type of states of the automaton.
 * @tparam  Symbol                       Type of symbols of the alphabet of
 *                                       the automaton.
 * @tparam  RightHandSide                Type of the right-hand side of
 *                                       automaton rules.
 */
template
<
	class MTBDDTransitionTableWrapper,
	typename State,
	typename Symbol,
	template <typename> class RightHandSide
>
class SFTA::NDSymbolicBUTreeAutomaton
	: public SFTA::SymbolicBUTreeAutomaton
		<
			MTBDDTransitionTableWrapper,
			State,
			Symbol,
			RightHandSide<SFTA::Private::ElemOrVector<State> >
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
			RightHandSide
		> Type;

	typedef SymbolicBUTreeAutomaton
		<
			MTBDDTransitionTableWrapper,
			State,
			Symbol,
			RightHandSide<SFTA::Private::ElemOrVector<State> >
		> ParentClass;

	typedef typename ParentClass::HierarchyRoot HierarchyRoot;

	typedef typename ParentClass::StateType StateType;
	typedef typename ParentClass::SymbolType SymbolType;
	typedef typename ParentClass::LeftHandSideType LeftHandSideType;

	typedef typename ParentClass::RightHandSideType RightHandSideType;

	typedef typename ParentClass::LHSRootContainerType LHSRootContainerType;

	typedef typename ParentClass::MTBDDTTWrapperType MTBDDTTWrapperType;
	typedef typename MTBDDTTWrapperType::SharedMTBDDType SharedMTBDDType;
	typedef typename ParentClass::TTWrapperPtrType TTWrapperPtrType;

	typedef SFTA::NDSymbolicTDTreeAutomaton
	<
		MTBDDTransitionTableWrapper,
		StateType,
		SymbolType,
		SFTA::OrderedVector
	> NDSymbolicTDTreeAutomatonType;


	/**
	 * @brief  @copybrief SFTA::SymbolicBUTreeAutomaton::Operation
	 *
	 * @copydetails SFTA::SymbolicBUTreeAutomaton::Operation
	 */
	class Operation
		: public ParentClass::Operation
	{
	private:  // Private data types

		typedef typename SharedMTBDDType::RootType RootType;
		typedef typename SharedMTBDDType::LeafType LeafType;

		typedef Type* (Operation::*BinaryOperation)(const Type&, const Type&) const;

		/**
		 * @brief  Structure for hashing function of a pair
		 *
		 * This structure encapsulates hashing function for a pair of elements.
		 */
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


		class InclusionCheckingFunctor
		{
		private:  // Private data members

			typedef OrderedVector<StateType> StateSetType;
			typedef std::pair<size_t, StateSetType> NumberSetType;
			typedef std::list<NumberSetType> StateSetListType;
			typedef std::tr1::unordered_map<StateType, StateSetListType>
				StateToStateSetListHashTableType;
			typedef std::pair<StateType, NumberSetType> AntichainPairType;
			typedef std::queue<AntichainPairType> PairQueueType;
			typedef std::set<size_t> RevokedSetType;

		private:  // Private data members

			const Type* smallerAut_;
			const Type* biggerAut_;

		private:  // Private methods

			InclusionCheckingFunctor(const InclusionCheckingFunctor&);
			InclusionCheckingFunctor& operator=(const InclusionCheckingFunctor&);

		public:   // Public methods

			InclusionCheckingFunctor(const Type* smallerAut, const Type* biggerAut)
				: smallerAut_(smallerAut),
					biggerAut_(biggerAut)
			{
				assert(smallerAut_ != static_cast<Type*>(0));
				assert(biggerAut_ != static_cast<Type*>(0));
			}

			bool operator()()
			{
				class CollectorApplyFunctor
					: public SharedMTBDDType::AbstractApplyFunctorType
				{
				private:  // Private data members

					const Type* smallerAut_;
					const Type* biggerAut_;
					StateToStateSetListHashTableType* antichain_;
					PairQueueType* pairQueue_;
					bool failed_;
					size_t counter_;
					RevokedSetType* revokedNumbers_;

				private:  // Private methods

					CollectorApplyFunctor(const CollectorApplyFunctor&);
					CollectorApplyFunctor& operator=(const CollectorApplyFunctor&);

				public:   // Public data members

					CollectorApplyFunctor(const Type* smallerAut, const Type* biggerAut,
						StateToStateSetListHashTableType* antichain, PairQueueType* pairQueue,
						RevokedSetType* revokedNumbers)
						: smallerAut_(smallerAut),
							biggerAut_(biggerAut),
							antichain_(antichain),
							pairQueue_(pairQueue),
							failed_(false),
							counter_(),
							revokedNumbers_(revokedNumbers)
					{
						assert(smallerAut_ != static_cast<Type*>(0));
						assert(biggerAut_ != static_cast<Type*>(0));
						assert(antichain_ != static_cast<StateToStateSetListHashTableType*>(0));
						assert(pairQueue_ != static_cast<PairQueueType*>(0));
						assert(revokedNumbers_ != static_cast<RevokedSetType*>(0));
					}

					virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs)
					{
						if (!failed_)
						{	// in case there is some sense in doing the following
							for (typename LeafType::const_iterator itLhs = lhs.begin();
								itLhs != lhs.end() && !(failed_); ++itLhs)
							{
								const StateType& smallerState = itLhs->GetElement();

								bool addSet = false;         // flag that indicates that the following
								                             // variable should be checked for finality
								StateSetType biggerStates;   // contains newly added set to antichain
								                             // (if such exists)

								typename StateToStateSetListHashTableType::iterator itHT;
								if ((itHT = antichain_->find(smallerState)) != antichain_->end())
								{	// if there is already some list for the smallerState
									StateSetListType& biggerSetList = itHT->second;

									// try to find some smaller set in the antichain
									bool isSubset = true;
									typename StateSetListType::const_iterator itList;
									for (itList = biggerSetList.begin();
										itList != biggerSetList.end(); ++itList)
									{
										const StateSetType& listItem = itList->second;

										// check if 'listItem' is a subset of 'rhs'
										typename LeafType::const_iterator itRhs = rhs.begin();
										for (typename StateSetType::const_iterator itAntichainSet
											= listItem.begin(); itAntichainSet != listItem.end();
											++itAntichainSet)
										{
											while ((itRhs != rhs.end()) &&
												(itRhs->GetElement() < *itAntichainSet))
											{
												++itRhs;
											}

											if ((itRhs == rhs.end()) || (itRhs->GetElement() != *itAntichainSet))
											{	// in case the 'listItem' is not a subset of 'rhs'
												isSubset = false;
												break;
											}
										}

										assert(!isSubset || (itRhs != rhs.end()) || rhs.empty());

										if (isSubset)
										{	// in case we found some subset
											break;
										}
									}

									if (!isSubset)
									{	// in case there is no smaller set in the antichain
										addSet = true;

										// remove all bigger sets from the antichain
										typename StateSetListType::iterator itList;
										bool incrementIterator = true;
										for (itList = biggerSetList.begin();
											itList != biggerSetList.end();
											itList = incrementIterator? ++itList : itList)
										{
											incrementIterator = true;
											const StateSetType& listItem = itList->second;

											// check if 'rhs' is a subset of 'listItem'
											bool rhsIsSubset = true;
											typename StateSetType::const_iterator itAntichainSet
												= listItem.begin();
											for (typename LeafType::const_iterator itRhs = rhs.begin();
												itRhs != rhs.end(); ++itRhs)
											{
												while ((itAntichainSet != listItem.end()) &&
													(*itAntichainSet < itRhs->GetElement()))
												{
													++itAntichainSet;
												}

												if ((itAntichainSet == listItem.end()) ||
													(*itAntichainSet != itRhs->GetElement()))
												{	// in case the 'rhs' is not a subset of 'listItem'
													rhsIsSubset = false;
													break;
												}
											}

											assert(!rhsIsSubset || (itAntichainSet != listItem.end()));

											if (rhsIsSubset)
											{	// in case 'rhs' is smaller, remove 'listItem' from antichain
												itList = biggerSetList.erase(itList);
												revokedNumbers_->insert(itList->first);
												incrementIterator = false;
											}
										}
									}
								}
								else
								{	// if there isn't any list for smallerState
									addSet = true;
									itHT = antichain_->insert(std::make_pair(smallerState, StateSetListType())).first;
								}

								if (addSet)
								{
									//SFTA_LOGGER_INFO("Adding pair " + Convert::ToString(std::make_pair(smallerState, Convert::ToString(rhs))));
									for (typename LeafType::const_iterator itRhs = rhs.begin();
										itRhs != rhs.end(); ++itRhs)
									{
										biggerStates.insert(itRhs->GetElement());
									}

									AntichainPairType newPair = std::make_pair(smallerState,
										std::make_pair(getNewNumber(), biggerStates));
									itHT->second.push_back(newPair.second);
									pairQueue_->push(newPair);

									if (smallerAut_->IsStateFinal(smallerState))
									{	// in case the state from the smaller automaton is final
										failed_ = true;
										for (typename LeafType::const_iterator itRhs = rhs.begin();
											itRhs != rhs.end(); ++itRhs)
										{
											if (biggerAut_->IsStateFinal(itRhs->GetElement()))
											{
												failed_ = false;
												break;
											}
										}

										if (failed_)
										{
											//SFTA_LOGGER_INFO("Failing pair: " + Convert::ToString(std::make_pair(smallerState, rhs)));
										}
									}
								}
							}
						}

						return LeafType();
					}

					inline bool Failed() const
					{
						return failed_;
					}

					inline size_t getNewNumber()
					{
						return counter_++;
					}
				};


				class UnionApplyFunctor
					: public SharedMTBDDType::AbstractApplyFunctorType
				{
				public:
					virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs)
					{
						return lhs.Union(rhs);
					}
				};


				UnionApplyFunctor unionFunc;

				// the antichain
				StateToStateSetListHashTableType antichain;
				// queue of pairs (state, state_set) added to antichain
				// TODO: try stack here (compare with the queue)
				PairQueueType pairQueue;
				// set of numbers of revoked pairs
				RevokedSetType revokedNumbers;

				CollectorApplyFunctor collector(smallerAut_, biggerAut_, &antichain,
					&pairQueue, &revokedNumbers);

				SharedMTBDDType* mtbdd = smallerAut_->GetTTWrapper()->GetMTBDD();

				RootType smallerRoot = smallerAut_->getRoot(LeftHandSideType());
				RootType biggerRoot = biggerAut_->getRoot(LeftHandSideType());

				RootType tmp = mtbdd->Apply(smallerRoot, biggerRoot, &collector);

				// Erase the following line for better performance ;-)
				//mtbdd->EraseRoot(tmp);

				while (!collector.Failed() && !pairQueue.empty())
				{
					AntichainPairType nextPair = pairQueue.front();
					pairQueue.pop();

					//SFTA_LOGGER_INFO("Antichain = " + Convert::ToString(antichain));

					if (revokedNumbers.find(nextPair.second.first) == revokedNumbers.end())
					{	// in case this pair has not been revoked
						//SFTA_LOGGER_INFO("Processing pair: " + Convert::ToString(nextPair));

						StateType& smallerState = nextPair.first;

						// find all superstates such that 'smallerState' is an element of
						// these superstates
						typename LHSRootContainerType::IndexValueArray smallerLhss =
							smallerAut_->getRootMap().GetItemsWith(smallerState,
							smallerAut_->getStates());

						for (size_t arity = 0; arity < smallerLhss.size(); ++arity)
						{	// for each arity of left-hand side in smaller automaton
							for (size_t smallerIndex = 0;
								smallerIndex < smallerLhss[arity].size(); ++smallerIndex)
							{	// for each left-hand side of given arity in smaller automaton
								assert(arity > 0);    // there should be nothing for ()

								const typename LHSRootContainerType::IndexValueType& lhsIV
									= smallerLhss[arity][smallerIndex];
								//SFTA_LOGGER_INFO("Checking LHS: " + Convert::ToString(lhsIV.first));

								// collect vector of lists of possible values
								bool allComponentsInAntichain = true;
								std::vector<StateSetListType> listVector;
								for (size_t arityIndex = 0; arityIndex < arity; ++arityIndex)
								{
									typename StateToStateSetListHashTableType::const_iterator itHT;
									if ((itHT = antichain.find(lhsIV.first[arityIndex])) != antichain.end())
									{
										listVector.push_back(itHT->second);
									}
									else
									{
										allComponentsInAntichain = false;
										break;
									}
								}

								if (allComponentsInAntichain)
								{
									//SFTA_LOGGER_INFO("All components are in the antichain for LHS " + Convert::ToString(lhsIV.first));

									//SFTA_LOGGER_INFO("Respective sets: " + Convert::ToString(listVector));

									assert(listVector.size() == arity);

									// initialize vector of iterators
									std::vector<typename StateSetListType::const_iterator> vecIterator;
									for (typename std::vector<StateSetListType>::const_iterator itList
										= listVector.begin(); itList != listVector.end(); ++itList)
									{
										vecIterator.push_back(itList->begin());
									}

									assert(vecIterator.size() == arity);

									// generate all possible arity-tuples of sets from 'listVector'
									int index = vecIterator.size() - 1;
									while (index >= 0)
									{	// until the most significant component overflows

										std::string tmpString = "(";
										for (typename std::vector<typename StateSetListType::const_iterator>
											::const_iterator itItVec = vecIterator.begin();
											itItVec != vecIterator.end(); ++itItVec)
										{
											tmpString += Convert::ToString(**itItVec) + ", ";
										}

										//SFTA_LOGGER_INFO("TODO: Generating.... " + tmpString + ")");

										// generate the cartesian product of the sets

										// initialize vector of set iterators
										std::vector<typename StateSetType::const_iterator> setVecIterator;
										for (typename std::vector<typename StateSetListType::const_iterator>
											::const_iterator itItVec = vecIterator.begin();
											itItVec != vecIterator.end(); ++itItVec)
										{
											setVecIterator.push_back((*itItVec)->second.begin());
										}

										assert(setVecIterator.size() == arity);

										// generate all possible arity-tuples of sets from 'listVector'
										RootType unitedRoots = mtbdd->CreateRoot();
										int setIndex = setVecIterator.size() - 1;
										while (setIndex >= 0)
										{
											// get the left-hand side vector
											LeftHandSideType biggerLhs;
											bool setEmpty = false;
											for (size_t iVec = 0; iVec < setVecIterator.size(); ++iVec)
											{
												typename StateSetType::const_iterator& itTmp
													= setVecIterator[iVec];

												if (itTmp == vecIterator[iVec]->second.end())
												{	// in case the set is empty
													setEmpty = true;
													break;
												}

												biggerLhs.push_back(*itTmp);
											}

											if (setEmpty)
											{
												break;
											}

											//SFTA_LOGGER_INFO("Generating.... " + Convert::ToString(biggerLhs));

											tmp = unitedRoots;
											unitedRoots = mtbdd->Apply(unitedRoots,
												biggerAut_->getRoot(biggerLhs), &unionFunc);

											// Erase the following line for better performance ;-)
											// mtbdd->EraseRoot(tmp);


											setIndex = setVecIterator.size() - 1;

											do
											{
												setVecIterator[setIndex]++;

												if (setVecIterator[setIndex] ==
													vecIterator[setIndex]->second.end())
												{
													setVecIterator[setIndex] = vecIterator[setIndex]->second.begin();
													--setIndex;
												}
												else
												{
													break;
												}
											} while (setIndex >= 0);
										}

										tmp = mtbdd->Apply(smallerAut_->getRoot(lhsIV.first),
											unitedRoots, &collector);

										// Erase the following line for better performance ;-)
										// mtbdd->EraseRoot(tmp);

										index = vecIterator.size() - 1;

										do
										{
											vecIterator[index]++;

											if (vecIterator[index] == listVector[index].end())
											{
												vecIterator[index] = listVector[index].begin();
												--index;
											}
											else
											{
												break;
											}
										} while (index >= 0);
									}
								}
							}
						}
					}
					else
					{
						//SFTA_LOGGER_INFO("Revoked pair: " + Convert::ToString(nextPair));
					}
				}

				return !collector.Failed();
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
							StatePair productState = std::make_pair(lhsIt->GetElement(), rhsIt->GetElement());
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

		virtual typename HierarchyRoot::Operation::SimulationRelationType*
			GetIdentityRelation(const HierarchyRoot* aut) const
		{
			// Assertions
			assert(aut != static_cast<Type*>(0));

			typedef OrderedVector<StateType> StateSetType;
			typedef typename HierarchyRoot::Operation::SimulationRelationType SimType;

			const Type* autSym = static_cast<Type*>(0);

			if ((autSym = dynamic_cast<const Type*>(aut)) ==
				static_cast<const Type*>(0))
			{	// in case the type is not OK
				throw std::runtime_error(__func__ + std::string(": Invalid type"));
			}

			// the simulation relation
			SimType* sim = new SimType();

			StateSetType states = autSym->getStates();
			for (typename StateSetType::const_iterator itStates = states.begin();
				itStates != states.end(); ++itStates)
			{
				sim->insert(std::make_pair(*itStates, *itStates));
			}

			return sim;
		}

		virtual typename HierarchyRoot::Operation::SimulationRelationType*
			ComputeSimulationPreorder(const HierarchyRoot* aut) const
		{
			// Assertions
			assert(aut != static_cast<Type*>(0));

			typedef typename HierarchyRoot::Operation::SimulationRelationType SimType;
			typedef LeftHandSideType StateVector;
			typedef std::pair<StateVector, StateVector> StateVectorPair;
			typedef VectorMap<StateType, RootType> CountersType;
			typedef std::set<StateVectorPair> RemoveSetType;
			typedef SFTA::InflatableVector<SFTA::Vector<StateVector> >
				InflatableListOfVectorsType;
			typedef SFTA::InflatableVector<InflatableListOfVectorsType>
				InflatableListOfInflatableListsOfVectorsType;
			typedef std::tr1::unordered_map<StateType,
				InflatableListOfInflatableListsOfVectorsType> StateToLHSsType;

			class SimulationCounterInitializationApplyFunctor
				: public SharedMTBDDType::AbstractApplyFunctorType
			{
			private:

				StateType state_;

			public:

				SimulationCounterInitializationApplyFunctor()
					: state_()
				{ }

				void SetState(const StateType& state)
				{
					state_ = state;
				}

				virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs)
				{
					//SFTA_LOGGER_INFO("Initializing simulation counter for state " + Convert::ToString(state_));

					if (lhs.size() != 0)
					{
						LeafType newRhs = rhs;

						SFTA::Vector<StateType> newVec;
						newVec.push_back(state_);
						newVec.push_back(lhs.size());
						newRhs.insert(newVec);

						return newRhs;
					}
					else
					{
						return rhs;
					}
				}
			};

			class SimulationDetectorApplyFunctor
				: public SharedMTBDDType::AbstractApplyFunctorType
			{
			private:

				bool doesSimulationHold_;

			public:

				SimulationDetectorApplyFunctor()
					: doesSimulationHold_()
				{ }

				inline void Reset()
				{
					doesSimulationHold_ = true;
				}

				inline bool DoesSimulationHold() const
				{
					return doesSimulationHold_;
				}

				virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs)
				{
					//SFTA_LOGGER_INFO("Detecting simulation...");
					if (!lhs.empty() && rhs.empty())
					{
						doesSimulationHold_ = false;
					}

					return LeafType();
				}
			};

			class SimulationRefinementApplyFunctor
				: public SharedMTBDDType::AbstractTernaryApplyFunctorType
			{
			private:

				SimType* sim_;

				RemoveSetType* remove_;

				StateToLHSsType* stateToLhss_;

			private:

				SimulationRefinementApplyFunctor(const SimulationRefinementApplyFunctor& rhs);
				SimulationRefinementApplyFunctor& operator=(
					const SimulationRefinementApplyFunctor& rhs);


				void addToRemoveCutPairsOfVector(const StateType& p, const StateType& s)
				{
					typename StateToLHSsType::iterator itState2LHSsP;
					if ((itState2LHSsP = stateToLhss_->find(p)) != stateToLhss_->end())
					{	// in case there is something for p
						InflatableListOfInflatableListsOfVectorsType& innerContainerP =
							itState2LHSsP->second;

						for (size_t iSize = 0; iSize < innerContainerP.size(); ++iSize)
						{	// for all sizes of vector in which p is present
							const InflatableListOfVectorsType& listOfPositions = innerContainerP[iSize];

							for (size_t iPosition = 0; iPosition < listOfPositions.size(); ++iPosition)
							{	// for all positions in vectors of given size
								const SFTA::Vector<StateVector>& listOfStateVectorsP =
									listOfPositions[iPosition];

								for (typename SFTA::Vector<StateVector>::const_iterator itListP =
									listOfStateVectorsP.begin(); itListP != listOfStateVectorsP.end(); ++itListP)
								{	// for all vectors of given size that have p at the iPosition-th position
									const StateVector& pVec = *itListP;

									typename StateToLHSsType::iterator itState2LHSsS;
									if ((itState2LHSsS = stateToLhss_->find(s)) != stateToLhss_->end())
									{	// if there is something for s
										InflatableListOfInflatableListsOfVectorsType& innerContainerS =
											itState2LHSsS->second;

										const SFTA::Vector<StateVector>& listOfStateVectorsS =
											innerContainerS[iSize][iPosition];

										for (typename SFTA::Vector<StateVector>::const_iterator itListS =
											listOfStateVectorsS.begin(); itListS != listOfStateVectorsS.end(); ++itListS)
										{	// for all vectors of given size that have s at the iPosition-th position
											const StateVector& sVec = *itListS;

											bool vectorsSimulate = true;

											for (size_t iVecPosition = 0; iVecPosition < iSize; ++iVecPosition)
											{
												if (!sim_->is_in(std::make_pair(pVec[iVecPosition], sVec[iVecPosition])))
												{
													vectorsSimulate = false;
												}
											}

											if (vectorsSimulate)
											{
												remove_->insert(std::make_pair(pVec, sVec));
											}
										}
									}
								}
							}
						}
					}
				}

			public:

				SimulationRefinementApplyFunctor(SimType* sim, RemoveSetType* remove,
					StateToLHSsType* stateToLhss)
					: sim_(sim),
						remove_(remove),
						stateToLhss_(stateToLhss)
				{
					assert(sim_ != static_cast<SimType*>(0));
					assert(remove_ != static_cast<RemoveSetType*>(0));
					assert(stateToLhss_ != static_cast<StateToLHSsType*>(0));
				}

				virtual LeafType operator()(const LeafType& preR, const LeafType& preQ,
					const LeafType& cntQ)
				{
					//SFTA_LOGGER_INFO("Performing simulation refinement...");

					LeafType newCntQ;

					for (typename LeafType::const_iterator itCntQ = cntQ.begin();
						itCntQ != cntQ.end(); ++itCntQ)
					{
						const SFTA::Vector<StateType>& vec = itCntQ->GetVector();

						// we assert that the counters are in correct format
						assert(vec.size() == 2);

						const StateType& s = vec[0];

						if (preR.find(s) != preR.end())
						{	// in case the counter is to be decremented
							SFTA::Vector<StateType> newVec(vec);

							// we assert that we do not make mistakes in the algorithm :-)
							assert(newVec[1] > 0);

							--newVec[1];
							newCntQ.insert(newVec);

							if (newVec[1] == 0)
							{	// in case we break the simulation relation
								for (typename LeafType::const_iterator itPreQ = preQ.begin();
									itPreQ != preQ.end(); ++itPreQ)
								{	// for each element p of preQ
									const StateType& p = itPreQ->GetElement();

									if (sim_->is_in(std::make_pair(p, s)))
									{
										addToRemoveCutPairsOfVector(p, s);
										sim_->erase(std::make_pair(p, s));
									}
								}
							}
						}
						else
						{	// the counter is to be copied only
							newCntQ.insert(vec);
						}
					}

					return newCntQ;
				}
			};

			const Type* autSym = static_cast<Type*>(0);

			if ((autSym = dynamic_cast<const Type*>(aut)) ==
				static_cast<const Type*>(0))
			{	// in case the type is not OK
				throw std::runtime_error(__func__ + std::string(": Invalid type"));
			}

			// ********************************************************************
			//                         INITIALIZATION
			// ********************************************************************

			// the simulation relation
			SimType* sim = new SimType();

			//SFTA_LOGGER_INFO("Started computing top-down automaton");

			// corresponding TD automaton
			std::auto_ptr<NDSymbolicTDTreeAutomatonType> topDown(
				autSym->GetTopDownAutomaton());

			// used MTBDD
			SharedMTBDDType* mtbdd = autSym->GetTTWrapper()->GetMTBDD();

			// set "remove"
			RemoveSetType remove;

			// initial value of counters
			RootType initCnt = mtbdd->CreateRoot();

			// array of states
			std::vector<StateType> states = autSym->GetVectorOfStates();

			// The map of all LHSs of the BU automaton
			LHSRootContainerType buLHSs =	autSym->getRootMap();

			// first we create a data structure that for each state holds all
			// superstates where the state is
			StateToLHSsType stateToLhss;
			for (typename LHSRootContainerType::const_iterator itLhss = buLHSs.begin();
				itLhss != buLHSs.end(); ++itLhss)
			{	// for each LHS
				const StateVector& vec = itLhss->first;

				for (size_t iVec = 0; iVec < vec.size(); ++iVec)
				{	// for each state in the LHS
					const StateType& state = vec[iVec];

					typename StateToLHSsType::iterator itState2LHSs;
					if ((itState2LHSs = stateToLhss.find(state)) == stateToLhss.end())
					{	// in case there is nothing for the state
						itState2LHSs = (stateToLhss.insert(std::make_pair(state,
							InflatableListOfInflatableListsOfVectorsType()))).first;
					}

					InflatableListOfInflatableListsOfVectorsType& innerContainer =
						itState2LHSs->second;

					innerContainer[vec.size()][iVec].push_back(vec);
				}
			}


			// create necessary apply functors
			SimulationCounterInitializationApplyFunctor simulationCounterInitializer;
			SimulationDetectorApplyFunctor simulationDetector;
			SimulationRefinementApplyFunctor simulationRefineFunc(sim, &remove, &stateToLhss);

			//SFTA_LOGGER_INFO("Started computing initial refinement");

			// now we perform initial refinement
			for (typename std::vector<StateType>::const_iterator itStates = states.begin();
				itStates != states.end(); ++itStates)
			{
				const StateType& q = *itStates;

				simulationCounterInitializer.SetState(q);
				RootType qRoot = topDown->getRoot(q);

				// accumulate the initial counters
				RootType newCnt = mtbdd->Apply(qRoot, initCnt,
						&simulationCounterInitializer);
				mtbdd->EraseRoot(initCnt);
				initCnt = newCnt;

				for (typename std::vector<StateType>::const_iterator itHigherStates = states.begin();
					itHigherStates != states.end(); ++itHigherStates)
				{
					const StateType& r = *itHigherStates;

					bool simulationHolds = false;

					// NB: for downward simulation, the initial preorder is Q x Q
					if (/*!autSym->IsStateFinal(q) || autSym->IsStateFinal(r)*/ true)
					{	// in case the pair (itStates, itHigherStates) is in the initial preorder
						RootType rRoot = topDown->getRoot(r);

						simulationDetector.Reset();

						RootType tmp = mtbdd->Apply(qRoot, rRoot, &simulationDetector);
						mtbdd->EraseRoot(tmp);

						if (simulationDetector.DoesSimulationHold())
						{	// in case there holds the simulation relation
							simulationHolds = true;
							sim->insert(std::make_pair(q, r));
						}
					}

					if (!simulationHolds)
					{	// in case q is not simulated by r
						typename StateToLHSsType::iterator itState2LHSsQ;
						if ((itState2LHSsQ = stateToLhss.find(q)) != stateToLhss.end())
						{	// in case there is something for q
							InflatableListOfInflatableListsOfVectorsType& innerContainerQ =
								itState2LHSsQ->second;

							for (size_t iSize = 0; iSize < innerContainerQ.size(); ++iSize)
							{	// for all sizes of vector in which q is present
								const InflatableListOfVectorsType& listOfPositions = innerContainerQ[iSize];

								for (size_t iPosition = 0; iPosition < listOfPositions.size(); ++iPosition)
								{	// for all positions in vectors of given size
									const SFTA::Vector<StateVector>& listOfStateVectorsQ =
										listOfPositions[iPosition];

									for (typename SFTA::Vector<StateVector>::const_iterator itListQ =
										listOfStateVectorsQ.begin(); itListQ != listOfStateVectorsQ.end(); ++itListQ)
									{	// for all vectors of given size that have q at the iPosition-th position
										const StateVector& qVec = *itListQ;

										typename StateToLHSsType::iterator itState2LHSsR;
										if ((itState2LHSsR = stateToLhss.find(r)) != stateToLhss.end())
										{	// if there is something for r
											InflatableListOfInflatableListsOfVectorsType& innerContainerR =
												itState2LHSsR->second;

											const SFTA::Vector<StateVector>& listOfStateVectorsR =
												innerContainerR[iSize][iPosition];

											for (typename SFTA::Vector<StateVector>::const_iterator itListR =
												listOfStateVectorsR.begin(); itListR != listOfStateVectorsR.end(); ++itListR)
											{	// for all vectors of given size that have r at the iPosition-th position
												const StateVector& rVec = *itListR;

												remove.insert(std::make_pair(qVec, rVec));
											}
										}
									}
								}
							}
						}
					}
				}
			}

			//SFTA_LOGGER_INFO("Finished computing initial refinement");

			CountersType cnt(autSym->getSinkSuperState());

			class CopierMonadicApplyFunctor
				: public SharedMTBDDType::AbstractMonadicApplyFunctorType
			{
			public:

				virtual LeafType operator()(const LeafType& val)
				{
					return val;
				}
			};

			CopierMonadicApplyFunctor copierFunc;

			for (typename LHSRootContainerType::const_iterator itSuperStates =
				buLHSs.begin(); itSuperStates != buLHSs.end(); ++itSuperStates)
			{	// fill the counters
				RootType copiedCnt = mtbdd->MonadicApply(initCnt, &copierFunc);
				cnt.SetValue(itSuperStates->first, copiedCnt);
			}

			// TODO: prepare for erasing
			mtbdd->EraseRoot(initCnt);


			// ********************************************************************
			//                           COMPUTATION
			// ********************************************************************

			//SFTA_LOGGER_INFO("Size of remove set: " + Convert::ToString(remove.size()));
			size_t loopCounter = 0;

			//SFTA_LOGGER_INFO("Started computation");
			while (!remove.empty())
			{	// while there is a need for backwards propagation of cut simulations
				StateVectorPair cutRel = *(remove.begin());
				remove.erase(remove.begin());

				if (++loopCounter == 1000)
				{
					loopCounter = 0;
					//SFTA_LOGGER_INFO("Size of remove set: " + Convert::ToString(remove.size()));
				}

				const StateVector& qVec = cutRel.first;
				const StateVector& rVec = cutRel.second;

				RootType tmpRoot = mtbdd->TernaryApply(autSym->getRoot(rVec),
					autSym->getRoot(qVec), cnt.GetValue(qVec),
					&simulationRefineFunc);

				// Erase the following line for better performance ;-)
				//mtbdd->EraseRoot(cnt.GetValue(qVec));

				cnt.SetValue(qVec, tmpRoot);
			}

			// ********************************************************************
			//                           TIDYING UP
			// ********************************************************************

			// TODO: there should be something like erasing all counters, etc., but
			// I guess that there currently no way how to do it nicely (it would
			// mean doing something like CopyRoot that would increase the reference
			// counter of some nodes of the MTBDD

//			for (typename LHSRootContainerType::const_iterator itCounters =
//				cnt.begin(); itCounters != cnt.end(); ++itCounters)
//			{	// fill the counters
//				mtbdd->EraseRoot(itCounters->second);
//			}

			return sim;
		}

		virtual bool CheckLanguageInclusion(const HierarchyRoot* a1,
			const HierarchyRoot* a2,
			const typename HierarchyRoot::Operation::SimulationRelationType* simA1,
			const typename HierarchyRoot::Operation::SimulationRelationType* simA2) const
		{
			assert(a1 != static_cast<HierarchyRoot*>(0));
			assert(a2 != static_cast<HierarchyRoot*>(0));

			// We do not make use of simulation
			assert(simA1 == static_cast<typename HierarchyRoot::Operation::SimulationRelationType*>(0));
			assert(simA2 == static_cast<typename HierarchyRoot::Operation::SimulationRelationType*>(0));

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

			InclusionCheckingFunctor inclFunc(a1Sym, a2Sym);
			return inclFunc();
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
			RightHandSideType());
	}

	NDSymbolicBUTreeAutomaton(const NDSymbolicBUTreeAutomaton& aut)
		: ParentClass(aut)
	{ }

	explicit NDSymbolicBUTreeAutomaton(TTWrapperPtrType ttWrapper)
		: ParentClass(ttWrapper)
	{ }


	NDSymbolicTDTreeAutomatonType* GetTopDownAutomaton() const
	{
		typedef typename SharedMTBDDType::RootType RootType;

		class CollectorApplyFunctor
			: public SharedMTBDDType::AbstractApplyFunctorType
		{
		private:  // Private data types

			StateType wantedState_;
			LeftHandSideType addedSuperState_;

		public:   // Public data types

			typedef typename SharedMTBDDType::LeafType LeafType;

		public:   // Public methods

			CollectorApplyFunctor()
				: wantedState_(),
					addedSuperState_()
			{ }

			void SetWantedState(const StateType& wantedState)
			{
				wantedState_ = wantedState;
			}

			void SetAddedSuperState(const LeftHandSideType& addedSuperState)
			{
				addedSuperState_ = addedSuperState;
			}

			virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs)
			{
				if (lhs.find(wantedState_) != lhs.end())
				{	// in case the state we are looking for is in the leaf
					LeafType result = rhs;
					result.insert(addedSuperState_);
					return result;
				}
				else
				{
					return rhs;
				}
			}
		};

		CollectorApplyFunctor collectorFunc;

		NDSymbolicTDTreeAutomatonType* tdAut =
			new NDSymbolicTDTreeAutomatonType(this->GetTTWrapper());

		std::vector<StateType> states = this->GetVectorOfStates();

		const LHSRootContainerType& rootMap = this->getRootMap();


		for (typename std::vector<StateType>::const_iterator itStates = states.begin();
			itStates != states.end(); ++itStates)
		{
			const StateType& newState = *itStates;

			tdAut->AddState(newState);
			if (this->IsStateFinal(newState))
			{	// set state as initial in case it is final in the original automaton
				tdAut->SetStateInitial(newState);
			}

			RootType tdRoot = tdAut->getRoot(newState);

			collectorFunc.SetWantedState(newState);

			for (typename LHSRootContainerType::const_iterator itSuperStates = rootMap.begin();
				itSuperStates != rootMap.end(); ++itSuperStates)
			{
				collectorFunc.SetAddedSuperState(itSuperStates->first);
				RootType newRoot = tdAut->GetTTWrapper()->GetMTBDD()->Apply(
					itSuperStates->second, tdRoot, &collectorFunc);
				tdAut->GetTTWrapper()->GetMTBDD()->EraseRoot(tdRoot);
				tdRoot = newRoot;
			}

			tdAut->setRoot(newState, tdRoot);
		}

		return tdAut;
	}

};

#endif
