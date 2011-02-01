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
							StatePair productState = std::make_pair(StateType(*lhsIt), StateType(*rhsIt));
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
					if (lhs.empty() && !rhs.empty())
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
									const StateType& p = *itPreQ;

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

					if (!autSym->IsStateFinal(q) || autSym->IsStateFinal(r))
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

			CountersType cnt(autSym->getSinkSuperState());

			for (typename LHSRootContainerType::const_iterator itSuperStates =
				buLHSs.begin(); itSuperStates != buLHSs.end(); ++itSuperStates)
			{	// fill the counters
				cnt.SetValue(itSuperStates->first, initCnt);
			}

			// TODO: prepare for erasing
//			mtbdd->EraseRoot(initCnt);


			// ********************************************************************
			//                           COMPUTATION
			// ********************************************************************

			while (!remove.empty())
			{	// while there is a need for backwards propagation of cut simulations
				StateVectorPair cutRel = *(remove.begin());
				remove.erase(remove.begin());

				const StateVector& qVec = cutRel.first;
				const StateVector& rVec = cutRel.second;

				//SFTA_LOGGER_INFO("removed pair (" + Convert::ToString(qVec) + ", " + Convert::ToString(rVec) + ")");

				RootType tmpRoot = mtbdd->TernaryApply(autSym->getRoot(qVec),
					autSym->getRoot(rVec), cnt.GetValue(qVec),
					&simulationRefineFunc);

				cnt.SetValue(qVec, tmpRoot);

				//TODO: erase the old cnt counter
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
