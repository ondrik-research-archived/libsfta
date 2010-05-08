/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    The file with the MTBDDOperation class.
 *
 *****************************************************************************/

#ifndef _SFTA_MTBDD_OPERATION_HH_
#define _SFTA_MTBDD_OPERATION_HH_

// SFTA header files
#include <sfta/abstract_operation.hh>

// Standard library header files
#include <algorithm>
#include <queue>

// Loki library header files
#include <loki/HierarchyGenerators.h>
#include <loki/TypelistMacros.h>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		class TreeAutomaton
	>
	class MTBDDOperation;
}


template
<
	class TreeAutomaton
>
class SFTA::MTBDDOperation
	: public SFTA::AbstractOperation<TreeAutomaton>
{
private:  // Private data types

	typedef TreeAutomaton TreeAutomatonType;

	typedef typename TreeAutomatonType::TransFuncPtrType TransFuncPtrType;
	typedef typename TreeAutomatonType::TransitionFunctionType TransitionFunctionType;

	typedef typename TransitionFunctionType::StateType StateType;
	typedef typename TransitionFunctionType::LeftHandSideType LeftHandSideType;
	typedef typename TransitionFunctionType::MTBDDType MTBDDType;
	typedef typename TransitionFunctionType::RegistrationTokenType RegistrationTokenType;

	typedef typename MTBDDType::RootType RootType;

	typedef typename MTBDDType::LeafType LeafType;

	typedef typename TransitionFunctionType::MTBDDRootTypeHashTable MTBDDRootTypeHashTable;

	typedef std::pair<StateType, StateType> PairOfStatesType;
	typedef std::pair<PairOfStatesType, StateType> PairOfStatesNewStateType;
	typedef std::queue<PairOfStatesNewStateType> QueueOfPairsPlusNewStateType;
	typedef std::set<StateType> SetOfStates;
	typedef std::map<PairOfStatesType, StateType> MapOfPairsToStateType;

	typedef typename SFTA::Private::Convert Convert;

	typedef std::map<StateType, SFTA::OrderedVector<StateType> > SimulationTable;

	typedef std::map<StateType, SFTA::Vector<RootType> >
		VectorSimulationTableUnary;

	typedef std::map<std::pair<StateType, StateType>, SFTA::Vector<RootType> >
		VectorSimulationTableBinary;

	typedef std::queue<std::pair<RootType, RootType> > RootPairQueue;

	typedef std::set<StateType> StateClass;
	typedef Vector<StateClass> QuotientSet;

	typedef std::map<StateType, std::pair<StateType, StateClass> > ProjectionMap;

	typedef std::queue<StateType> StateQueue;

	class LeafIntersection
		: public MTBDDType::AbstractApplyFunctorType
	{
	private:  // Private data members

		QueueOfPairsPlusNewStateType* workingQueue_;

		TransFuncPtrType transFunc_;

		RegistrationTokenType regToken_;

		StateType sinkState_;

		// TODO: substitute for a matrix
		MapOfPairsToStateType* translationTable_;

	private:  // Private methods

		LeafIntersection(const LeafIntersection& li);
		LeafIntersection& operator=(const LeafIntersection& li);

	protected:// Protected methods

		StateType getNewStateForProduct(const PairOfStatesType& prod)
		{
			typename MapOfPairsToStateType::const_iterator it;
			if ((it = translationTable_->find(prod)) == translationTable_->end())
			{	// in case the product state is not in the table
				StateType newState = transFunc_->AllocateState(regToken_);
				translationTable_->insert(std::make_pair(prod, newState));
				return newState;
			}

			return it->second;
		}


	public:   // Public methods

		LeafIntersection(QueueOfPairsPlusNewStateType* workingQueue,
			TransFuncPtrType transFunc, RegistrationTokenType regToken,
			MapOfPairsToStateType* translationTable)
			: workingQueue_(workingQueue),
				transFunc_(transFunc),
				regToken_(regToken),
				sinkState_(transFunc->sinkState_),
			  translationTable_(translationTable)
		{ }



		virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs)
		{
			LeafType newLeaf;

			for (typename LeafType::const_iterator it = lhs.begin();
				it != lhs.end(); ++it)
			{
				for (typename LeafType::const_iterator jt = rhs.begin();
						jt != rhs.end(); ++jt)
				{	// for each pair from the cartesian product lhs x rhs
					if ((*it == sinkState_) || (*jt == sinkState_))
					{	// in case any state is a sink state
						newLeaf.clear();
						newLeaf.push_back(sinkState_);
						return newLeaf;
					}

					PairOfStatesType prod(*it, *jt);
					StateType newState = getNewStateForProduct(prod);
					workingQueue_->push(std::make_pair(prod, newState));
					newLeaf.push_back(newState);
				}
			}

			return newLeaf;
		}
	};


	class LeafUnion
		: public MTBDDType::AbstractApplyFunctorType
	{
	public:

		virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs)
		{
			return lhs.Union(rhs);
		}
	};


	class LeafUnionWithTranslation
		: public MTBDDType::AbstractApplyFunctorType
	{
	private:  // Private data members

		ProjectionMap* canonicalProjection_;
		StateType sinkState_;

	private:  // Private methods

		LeafUnionWithTranslation(const LeafUnionWithTranslation& leafUnion);
		LeafUnionWithTranslation& operator=(const LeafUnionWithTranslation&
			leafUnion);

	public:   // Public methods

		LeafUnionWithTranslation(ProjectionMap* canonicalProjection,
			StateType sinkState)
			: canonicalProjection_(canonicalProjection), sinkState_(sinkState)
		{ }

		virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs)
		{
			LeafType newLeaf;

			for (typename LeafType::const_iterator it = lhs.begin();
				it != lhs.end(); ++it)
			{	// for each state from the left-hand side list
				RootType state = (*canonicalProjection_)[*it].first;
				if (state != sinkState_)
				{	// in case the state is not a sink state
					newLeaf.push_back(state);
				}
			}

			for (typename LeafType::const_iterator it = rhs.begin();
				it != rhs.end(); ++it)
			{	// for each state from the right-hand side list
				RootType state = (*canonicalProjection_)[*it].first;
				if (state != sinkState_)
				{	// in case the state is not a sink state
					newLeaf.push_back(state);
				}
			}

			if (newLeaf.empty())
			{	// in case the list is empty
				newLeaf.push_back(sinkState_);
			}

			return newLeaf;
		}
	};


	class SimulationRefinement
		: public MTBDDType::AbstractApplyFunctorType
	{
	private:  // Private data members

		SimulationTable* table_;
		bool* changed_;

	public:   // Public methods

		SimulationRefinement(SimulationTable* table, bool* changed)
			: table_(table), changed_(changed)
		{ }

		virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs)
		{
			SFTA_LOGGER_DEBUG("LHS: " + Convert::ToString(lhs));
			SFTA_LOGGER_DEBUG("RHS: " + Convert::ToString(rhs));

			for (typename LeafType::const_iterator it = lhs.begin();
				it != lhs.end(); ++it)
			{	// for each state q from the simulated MTBDD
				SFTA::OrderedVector<StateType>& simulatingStates = (*table_)[*it];

				SFTA_LOGGER_DEBUG("States simulating " + Convert::ToString(*it)
					+ ": " + Convert::ToString(simulatingStates));

				size_t j = 0;
				for (size_t i = 0; i < simulatingStates.size(); ++i)
				{	// each state simulating q; (q, r) \in D
					bool found = false;
					while (j < rhs.size())
					{	// continually traverse RHS leaf
						if (rhs[j] == simulatingStates[i])
						{	// in case the simulation holds
							SFTA_LOGGER_DEBUG("Found match for "
								+ Convert::ToString(rhs[j]));
							++j;
							found = true;
							break;
						}
						else if (rhs[j] < simulatingStates[i])
						{	// in case we might be in front of the correct state
							SFTA_LOGGER_DEBUG("Searching "
								+ Convert::ToString(simulatingStates[i]));

							++j;
						}
						else if (rhs[j] > simulatingStates[i])
						{	// in case we did not find the state
							SFTA_LOGGER_DEBUG("Removing (" + Convert::ToString(*it) + ", "
								+ Convert::ToString(simulatingStates[i])
								+ ") from the simulation table.");
							simulatingStates.Erase(i);
							--i;
							*changed_ = true;
							found = true;
							break;
						}
					}

					if (!found)
					{	// if we left the array
						SFTA_LOGGER_DEBUG("Removing_(" + Convert::ToString(*it) + ", "
							+ Convert::ToString(simulatingStates[i])
							+ ") from the simulation table.");
						simulatingStates.Erase(i);
						--i;
						*changed_ = true;
					}
				}
			}

			return lhs;
		}
	};


private:  // Private data members

	/**
	 * @brief  The name of the Log4cpp category for logging
	 *
	 * The name of the Log4cpp category used for logging messages from this
	 * class.
	 */
	static const char* LOG_CATEGORY_NAME;


public:   // Public methods

	virtual TreeAutomaton Union(TreeAutomaton& ta1, TreeAutomaton& ta2) const
	{
		// Assertions
		assert(ta1.GetTransitionFunction() == ta2.GetTransitionFunction());

		TransFuncPtrType transFunc = ta1.GetTransitionFunction();

		MTBDDType& mtbdd = transFunc->getMTBDD();

		LeftHandSideType lhs;

		LeafUnion leafUnion;
		RootType root = mtbdd.Apply(transFunc->getRoot(ta1.GetRegToken(), lhs),
			transFunc->getRoot(ta2.GetRegToken(), lhs), &leafUnion);

		TreeAutomaton result(transFunc);

		transFunc->setRoot(result.GetRegToken(), lhs, root);

		std::vector<StateType> states = ta1.GetTFStates();
		for (size_t i = 0; i < states.size(); ++i)
		{
			// insert the state into the state set
			result.MapStateToTF("q" + Convert::ToString(states[i]), states[i]);
			if (ta1.IsTFStateFinal(states[i]))
			{
				result.SetTFStateFinal(states[i]);
			}
		}

		states = ta2.GetTFStates();
		for (size_t i = 0; i < states.size(); ++i)
		{
			// insert the state into the state set
			result.MapStateToTF("q" + Convert::ToString(states[i]), states[i]);
			if (ta2.IsTFStateFinal(states[i]))
			{
				result.SetTFStateFinal(states[i]);
			}
		}

		return result;
	}

	virtual TreeAutomaton Intersection(TreeAutomaton& ta1,
		TreeAutomaton& ta2) const
	{
		// Assertions
		assert(ta1.GetTransitionFunction() == ta2.GetTransitionFunction());

		TransFuncPtrType transFunc = ta1.GetTransitionFunction();

		MTBDDType& mtbdd = transFunc->getMTBDD();

		LeftHandSideType lhs;

		TreeAutomaton result(transFunc);
		QueueOfPairsPlusNewStateType queueOfProducts;
		MapOfPairsToStateType translationTable;
		SetOfStates processedStates;
		LeafIntersection intersectFunc(&queueOfProducts, transFunc,
			result.GetRegToken(), &translationTable);
		RootType root = mtbdd.Apply(transFunc->getRoot(ta1.GetRegToken(), lhs),
			transFunc->getRoot(ta2.GetRegToken(), lhs), &intersectFunc);
		transFunc->setRoot(result.GetRegToken(), lhs, root);


		do
		{	// until the queue is empty
			PairOfStatesNewStateType prodNewState = queueOfProducts.front();
			queueOfProducts.pop();

			const StateType& newState = prodNewState.second;

			if (processedStates.insert(prodNewState.second).second)
			{	// in case the pair has not been processed yet
				const PairOfStatesType& prod = prodNewState.first;

				// insert the state into the state set
				result.MapStateToTF("q" + Convert::ToString(newState), newState);

				if (ta1.IsTFStateFinal(prod.first) && ta2.IsTFStateFinal(prod.second))
				{	// in case both states are final in their respective automata
					result.SetTFStateFinal(newState);
				}

				// find roots for unary symbols with the states
				LeftHandSideType lhs1(1);
				lhs1[0] = prod.first;
				LeftHandSideType lhs2(1);
				lhs2[0] = prod.second;
				RootType root1 = transFunc->getRootForArity1(ta1.GetRegToken(), lhs1);
				RootType root2 = transFunc->getRootForArity1(ta2.GetRegToken(), lhs2);

				if ((root1 != transFunc->sinkState_) && (root2 != transFunc->sinkState_))
				{	// in case both states have successors
					LeftHandSideType newLhs(1);
					newLhs[0] = newState;
					root = mtbdd.Apply(root1, root2, &intersectFunc);
					transFunc->setRootForArity1(result.GetRegToken(), newLhs, root);
				}

				// find roots for binary symbols with the states
				lhs1 = LeftHandSideType(2);
				lhs2 = LeftHandSideType(2);

				// TODO: this could be done more efficiently using proper data
				// structures!

				// bind to the first position
				lhs1[0] = prod.first;
				lhs2[0] = prod.second;
				for (size_t i = 0; i < transFunc->container2_.size(); ++i)
				{
					lhs1[1] = i;
					root1 = transFunc->getRootForArity2(ta1.GetRegToken(), lhs1);

					if (root1 != transFunc->sinkState_)
					{	// in case the first root is interesting
						for (size_t j = 0; j < transFunc->container2_.size(); ++j)
						{	// find another matching pair
							lhs2[1] = j;
							root2 = transFunc->getRootForArity2(ta2.GetRegToken(), lhs2);

							if (root2 != transFunc->sinkState_)
							{	// in case the second root is interesting
								typename MapOfPairsToStateType::const_iterator it;
								if ((it = translationTable.find(std::make_pair(i,j)))
									!= translationTable.end())
								{	// in case such a pair exists
									transFunc->container2_[prod.first][i];
									transFunc->container2_[prod.second][j];

									LeftHandSideType newLhs(2);
									newLhs[0] = newState;
									newLhs[1] = it->second;
									root = mtbdd.Apply(root1, root2, &intersectFunc);
									transFunc->setRootForArity2(result.GetRegToken(), newLhs, root);
								}
							}
						}
					}
				}

				// bind to the second position
				// TODO: check whether the position was looked at in the previous go
				// (e.g. for (q1, q1))
				lhs1[1] = prod.first;
				lhs2[1] = prod.second;
				for (size_t i = 0; i < transFunc->container2_.size(); ++i)
				{
					lhs1[0] = i;
					root1 = transFunc->getRootForArity2(ta1.GetRegToken(), lhs1);

					if (root1 != transFunc->sinkState_)
					{	// in case the first root is interesting
						for (size_t j = 0; j < transFunc->container2_.size(); ++j)
						{	// find another matching pair
							lhs2[0] = j;
							root2 = transFunc->getRootForArity2(ta2.GetRegToken(), lhs2);

							if (root2 != transFunc->sinkState_)
							{	// in case the second root is interesting
								typename MapOfPairsToStateType::const_iterator it;
								if ((it = translationTable.find(std::make_pair(i,j)))
									!= translationTable.end())
								{	// in case such a pair exists
									transFunc->container2_[i][prod.first];
									transFunc->container2_[j][prod.second];

									LeftHandSideType newLhs(2);
									newLhs[1] = newState;
									newLhs[0] = it->second;
									root = mtbdd.Apply(root1, root2, &intersectFunc);
									transFunc->setRootForArity2(result.GetRegToken(), newLhs, root);
								}
							}
						}
					}
				}

				// bind roots with n-ary symbols with the states
				for (typename MTBDDRootTypeHashTable::const_iterator it
					= transFunc->containerN_.begin();
					it != transFunc->containerN_.end(); ++it)
				{	// for all entries in the hash table
					const LeftHandSideType& firstVector = it->first;
					root1 = it->second;

					size_t position;
					for (position = 0; position < firstVector.size(); ++position)
					{ // check that the entry has x from (x, y)
						if (firstVector[position] == prod.first)
						{	// if we found x in the left-hand side
							break;
						}
					}

					if (position == firstVector.size())
					{	// in case no state in the left-hand side was x
						continue;
					}

					// find an entry with y from (x, y) on the same position as x
					for (typename MTBDDRootTypeHashTable::const_iterator jt
						= transFunc->containerN_.begin();
						jt != transFunc->containerN_.end(); ++jt)
					{	// for all entries in the hash table
						const LeftHandSideType& secondVector = jt->first;
						root2 = jt->second;

						if ((firstVector.size() != secondVector.size())
							|| (secondVector[position] != prod.second))
						{	// in case the required state is not at searched position (or if
							// the size is different)
							continue;
						}

						size_t i;
						LeftHandSideType newLhs(firstVector.size());
						newLhs[position] = newState;
						for (i = 0 ; i < firstVector.size(); ++i)
						{	// try to bind remaining states
							if (i != position)
							{	// in case we try to bind position that is not already bound
								typename MapOfPairsToStateType::const_iterator kt;
								if ((kt = translationTable.find(std::make_pair(firstVector[i],
									secondVector[i]))) == translationTable.end())
								{	// in case there does not exist a product state
									break;
								}

								newLhs[i] = kt->second;
							}
						}

						if (i == firstVector.size())
						{	// in case there exists product state for each respective
							// component of the left-hand side of a rule
							root = mtbdd.Apply(root1, root2, &intersectFunc);
							transFunc->setRootForArityN(result.GetRegToken(), newLhs, root);
						}
					}
				}
			}
		} while (!queueOfProducts.empty());

		return result;
	}

	virtual TreeAutomaton SimulationReduction(TreeAutomaton& ta) const
	{
		TransFuncPtrType transFunc = ta.GetTransitionFunction();

		MTBDDType& mtbdd = transFunc->getMTBDD();

		TreeAutomaton result(transFunc);

		SimulationTable simulations;

		std::vector<StateType> states = ta.GetTFStates();
		SFTA::OrderedVector<StateType> orderedStates;
		for (typename std::vector<StateType>::const_iterator it = states.begin();
			it != states.end(); ++it)
		{	// for each state of the automaton
			orderedStates.push_back(*it);
		}

		SFTA_LOGGER_DEBUG("Ordered states: " + Convert::ToString(orderedStates));

		for (typename std::vector<StateType>::const_iterator it = states.begin();
			it != states.end(); ++it)
		{	// for each state of the automaton
			simulations.insert(std::make_pair(*it, orderedStates));
		}

		LeafUnion leafUnion;
		RootPairQueue rootQueue;
		bool changed = true;

		while (changed)
		{	// until we reach the fixpoint
			changed = false;

			// process nullary left-hand side
			{
				RootType root = transFunc->container0_[ta.GetRegToken()];
				rootQueue.push(std::make_pair(root, root));
			}

			// process unary left-hand sides 

			// for each left-hand side, compute left-hand sides that simulate that
			// side
			VectorSimulationTableUnary unarySimulations;
			for (typename SFTA::OrderedVector<StateType>::const_iterator it
				= orderedStates.begin(); it != orderedStates.end(); ++it)
			{	// for each state of the automaton
				if (transFunc->container1_[*it] != transFunc->sinkState_)
				{	// in case the left-hand side is interesting
					Vector<StateType> roots;

					for (typename OrderedVector<StateType>::const_iterator jt
						= simulations[*it].begin(); jt != simulations[*it].end(); ++jt)
					{	// for all states from the left-hand sides that simulate this
						// left-hand side
						roots.push_back(transFunc->container1_[*jt]);
					}

					unarySimulations.insert(std::make_pair(*it, roots));
				}
				else
				{	// in case it is not interesting
					unarySimulations.insert(std::make_pair(*it, Vector<StateType>()));
				}
			}

			// for each unary left-hand side, merge right-hand sides of all other
			// left-hand sides that simulate that left-hand side
			for (typename VectorSimulationTableUnary::const_iterator it
				= unarySimulations.begin(); it != unarySimulations.end(); ++it)
			{	// for every unary left-hand side

				// merge MTBDDs of all vectors that simulate processed vector
				RootType root = mtbdd.CreateRoot();
				for (typename Vector<RootType>::const_iterator jt
					= (it->second).begin(); jt != (it->second).end(); ++jt)
				{	// for every left-hand side that simulates chosen left-hand side
					RootType newRoot = mtbdd.Apply(root, *jt, &leafUnion);
					mtbdd.EraseRoot(root);
					root = newRoot;
				}

				rootQueue.push(std::make_pair(transFunc->container1_[it->first], root));
			}

			// process binary left-hand sides 

			// for each left-hand side, compute left-hand sides that simulate that
			// side
			VectorSimulationTableBinary binarySimulations;
			for (typename SFTA::OrderedVector<StateType>::const_iterator it
				= orderedStates.begin(); it != orderedStates.end(); ++it)
			{
				for (typename SFTA::OrderedVector<StateType>::const_iterator jt
					= orderedStates.begin(); jt != orderedStates.end(); ++jt)
				{	// for each pair of states of the automaton
					if (transFunc->container2_[*it][*jt] != transFunc->sinkState_)
					{	// in case the left-hand side is interesting
						Vector<StateType> roots;

						for (typename OrderedVector<StateType>::const_iterator kt
							= simulations[*it].begin(); kt != simulations[*it].end(); ++kt)
						{	// for all states from the left-hand sides that simulate the first
							// state of the left-hand side
							for (typename OrderedVector<StateType>::const_iterator lt
								= simulations[*jt].begin(); lt != simulations[*jt].end(); ++lt)
							{	// for all states from the left-hand sides that simulate the
								// second state of the left-hand side
								roots.push_back(transFunc->container2_[*kt][*lt]);
							}
						}

						binarySimulations.insert(std::make_pair(std::make_pair(*it, *jt),
							roots));
					}
					else
					{	// in case it is not interesting
						binarySimulations.insert(std::make_pair(std::make_pair(*it, *jt),
							Vector<StateType>()));
					}
				}
			}


			// for each binary left-hand side, merge right-hand sides of all other
			// left-hand sides that simulate that left-hand side
			for (typename VectorSimulationTableBinary::const_iterator it
				= binarySimulations.begin(); it != binarySimulations.end(); ++it)
			{	// for every binary left-hand side

				// merge MTBDDs of all vectors that simulate processed vector
				RootType root = mtbdd.CreateRoot();
				for (typename Vector<RootType>::const_iterator jt
					= (it->second).begin(); jt != (it->second).end(); ++jt)
				{	// for every left-hand side that simulates chosen left-hand side
					RootType newRoot = mtbdd.Apply(root, *jt, &leafUnion);
					mtbdd.EraseRoot(root);
					root = newRoot;
				}

				rootQueue.push(std::make_pair(transFunc->container2_[(it->first).first]
					[(it->first).second], root));
			}




			// process N-ary left-hand sides 
			// TODO



			SimulationRefinement refinement(&simulations, &changed);

			// process each pair (rootNode, sim(rootNode))
			while (!rootQueue.empty())
			{	// until the queue is empty
				std::pair<RootType, RootType> procPair = rootQueue.front();
				rootQueue.pop();

				// attempt to refine the simulation relation
				RootType tmpNode = mtbdd.Apply(procPair.first, procPair.second,
					&refinement);
				mtbdd.EraseRoot(tmpNode);
			}
		}


		// print out the simulation relation
		for (typename SimulationTable::const_iterator it = simulations.begin();
			it != simulations.end(); ++it)
		{
			SFTA_LOGGER_DEBUG("States simulating " + Convert::ToString(it->first)
				+ ": " + Convert::ToString(it->second));
		}

		// compute classes of equivalence relation ~ induced by symmetric closure
		// of simulation relation
		std::set<StateType> processedStates;
		QuotientSet quotientSet;
		for (typename SimulationTable::const_iterator it = simulations.begin();
			it != simulations.end(); ++it)
		{	// for each state
			StateClass stateClass;
			StateQueue stateQueue;
			stateQueue.push(it->first);

			while (!stateQueue.empty())
			{	// while the queue is not empty
				StateType state = stateQueue.front();
				stateQueue.pop();
				if ((processedStates.insert(state)).second)
				{	// in case the state has not been processed yet
					for (typename Vector<StateType>::const_iterator jt
						= (it->second).begin(); jt != (it->second).end(); ++jt)
					{	// for each state simulating current state
						stateClass.insert(*jt);
						stateQueue.push(*jt);
					}
				}
			}

			if (!stateClass.empty())
			{	// unless the class of the quotient set is empty
				quotientSet.push_back(stateClass);
			}
		}

		// print quotient set
		for (typename QuotientSet::const_iterator it = quotientSet.begin();
			it != quotientSet.end(); ++it)
		{	// for all classes of the quotient set
			SFTA_LOGGER_DEBUG("Class of quotient set: " + Convert::ToString(*it));
		}

		// check that the result really is a quotient set
		// TODO

		// create new states for classes of the quotient set
		for (size_t i = 0; i < quotientSet.size(); ++i)
		{	// for each quotient set class, create new state
			result.AddState(Convert::ToString(i));
		}

		std::vector<StateType> newStates(result.GetTFStates());

		// create canonical projection map Q -> Q/~
		ProjectionMap canonicalProjection;
		for (size_t i = 0; i < quotientSet.size(); ++i)
		{	// for each class of the quotient set
			for (typename StateClass::const_iterator it = quotientSet[i].begin();
				it != quotientSet[i].end(); ++it)
			{	// for each state x, create projection x |-> [x]
				canonicalProjection.insert(std::make_pair(*it,
					std::make_pair(newStates[i], quotientSet[i])));;
			}
		}

		// create new transition set from the simulation relation
		LeafUnionWithTranslation leafUnionTranslation(&canonicalProjection,
			transFunc->sinkState_);

		// for nullary transition
		{
			RootType nullaryNode = transFunc->container0_[ta.GetRegToken()];
			RootType root = mtbdd.CreateRoot();
			RootType newRoot = mtbdd.Apply(root, nullaryNode, &leafUnionTranslation);
			mtbdd.EraseRoot(root);
			root = newRoot;
			transFunc->container0_[result.GetRegToken()] = root;
		}

		// for unary transitions
		processedStates.clear();
		for (size_t i = 0; i < transFunc->container1_.size(); ++i)
		{	// for all unary transitions
			if (ta.ContainsTFState(i))
			{	// if the only state of the unary left-hand side is from the input
				// automaton

				if ((processedStates.insert(i)).second)
				{	// in case the left-hand side with the state has not been processed
					RootType root = mtbdd.CreateRoot();

					const StateClass& stClass = canonicalProjection[i].second;
					for (typename StateClass::const_iterator it = stClass.begin();
						it != stClass.end(); ++it)
					{	// for each state in the class
						RootType newRoot = mtbdd.Apply(root, transFunc->container1_[*it],
							&leafUnionTranslation);
						mtbdd.EraseRoot(root);
						root = newRoot;

						// mark the state as processed
						processedStates.insert(*it);
					}

					// insert the MTBDD
					transFunc->container1_[canonicalProjection[i].first] = root;
				}
			}
		}

		// for binary transitions
		std::set<std::pair<StateType, StateType> > processedPairs;
		for (size_t i = 0; i < transFunc->container2_.size(); ++i)
		{	// for all rows
			if (ta.ContainsTFState(i))
			{	// case state i is in the correct automaton
				for (size_t j = 0; j < transFunc->container2_[i].size(); ++j)
				{	// for all columns
					if (ta.ContainsTFState(j))
					{	// in case both states are in the automaton
						if ((processedPairs.insert(std::make_pair(i, j)).second))
						{	// in case the left-hand side with the states has not been
							// processed

							RootType root = mtbdd.CreateRoot();

							const StateClass& stClass1 = canonicalProjection[i].second;
							const StateClass& stClass2 = canonicalProjection[j].second;
							for (typename StateClass::const_iterator it = stClass1.begin();
								it != stClass1.end(); ++it)
							{	// for each state in the first class
								for (typename StateClass::const_iterator jt = stClass2.begin();
									jt != stClass2.end(); ++jt)
								{	// for each state in the second class
									RootType newRoot = mtbdd.Apply(root,
										transFunc->container2_[*it][*jt], &leafUnionTranslation);
									mtbdd.EraseRoot(root);
									root = newRoot;

									// mark the pair of states as processed
									processedPairs.insert(std::make_pair(*it, *jt));
								}
							}

							// insert the MTBDD
							transFunc->container2_[canonicalProjection[i].first]
								[canonicalProjection[j].first] = root;
						}
					}
				}
			}
		}


		// for N-ary transitions
		std::set<LeftHandSideType> processedLHSs;
		for (typename
			TransitionFunctionType::MTBDDRootTypeHashTable::const_iterator it
			= transFunc->containerN_.begin();
			it != transFunc->containerN_.end(); ++it)
		{	// for every N-ary left-hand side
			const LeftHandSideType& lhs = it->first;

			SFTA_LOGGER_DEBUG("Processing N-ary LHS " + Convert::ToString(lhs));

			bool contains = true;
			for (typename LeftHandSideType::const_iterator jt = lhs.begin();
				jt != lhs.end(); ++jt)
			{	// check that every state is from the input automaton
				if (!ta.ContainsTFState(*jt))
				{	// in case this state is not
					contains = false;
					break;
				}
			}

			if (contains)
			{	// in case the left-hand side is interesting
				SFTA_LOGGER_DEBUG("All states are from this automaton");
				if ((processedLHSs.insert(lhs).second))
				{	// in case the left-hand side with the states has not been
					// processed yet

					RootType root = mtbdd.CreateRoot();
					LeftHandSideType newLhs(lhs.size());

					std::vector<const StateClass*> stClasses(lhs.size());
					for (size_t i = 0; i < lhs.size(); ++i)
					{	// for each state of the left-hand side
						stClasses[i] = &(canonicalProjection[lhs[i]].second);
						newLhs[i] = canonicalProjection[lhs[i]].first;
					}

					// now traverse all N-ary LHSs and in case they belong to respective
					// classes, make union
					for (typename
						TransitionFunctionType::MTBDDRootTypeHashTable::const_iterator jt
						= transFunc->containerN_.begin();
						jt != transFunc->containerN_.end(); ++jt)
					{	// traverse all N-ary left-hand sides
						bool isFromClasses = true;
						const LeftHandSideType& innerLhs = jt->first;
						for (size_t i = 0; i < innerLhs.size(); ++i)
						{	// for all states of the left-hand side
							if (stClasses[i]->find(innerLhs[i]) == stClasses[i]->end())
							{	// in case the state is not in the state class
								isFromClasses = false;
								break;
							}
						}

						if (isFromClasses)
						{	// in case the states are from proper classes
							SFTA_LOGGER_DEBUG("All states are from this automaton");
							RootType newRoot = mtbdd.Apply(root, jt->second,
								&leafUnionTranslation);
							mtbdd.EraseRoot(root);
							root = newRoot;

							// mark the left-hand side as processed
							processedLHSs.insert(innerLhs);
						}
					}

					// insert the MTBDD
					transFunc->setRootForArityN(result.GetRegToken(), newLhs, root);
				}
			}
		}

		return result;
	}
};


// Setting the logging category name for Log4cpp
template
<
	typename TA
>
const char* SFTA::MTBDDOperation<TA>::LOG_CATEGORY_NAME = "mtbdd_operation";

#endif
