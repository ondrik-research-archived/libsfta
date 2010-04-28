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

	typedef std::pair<StateType, StateType> PairOfStatesType;
	typedef std::pair<PairOfStatesType, StateType> PairOfStatesNewStateType;
	typedef std::queue<PairOfStatesNewStateType> QueueOfPairsPlusNewStateType;
	typedef std::set<StateType> SetOfStates;
	typedef std::map<PairOfStatesType, StateType> MapOfPairsToStateType;

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
			if ((it = translationTable_.find(prod)) == translationTable_.end())
			{	// in case the product state is not in the table
				StateType newState = transFunc_->AllocateState(regToken_);
				translationTable_.insert(std::make_pair(prod, newState));
				return newState;
			}

			return it->second;
		}


	public:   // Public methods

		LeafIntersection(QueueOfPairsPlusNewStateType* workingQueue,
			TransFuncPtrType transFunc, RegistrationTokenType regToken,
			MapOfPairsToStateType translationTable)
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

public:   // Public methods

	virtual TreeAutomaton Union(TreeAutomaton& ta1, TreeAutomaton& ta2) const
	{
		// Assertions
		assert(ta1.GetTransitionFunction() == ta2.GetTransitionFunction());

		TransFuncPtrType transFunc = ta1.GetTransitionFunction();

		MTBDDType& mtbdd = transFunc->getMTBDD();

		LeftHandSideType lhs;

		RootType root = mtbdd.Apply(transFunc->getRoot(ta1.GetRegToken(), lhs), transFunc->getRoot(ta2.GetRegToken(), lhs), new LeafUnion);

		TreeAutomaton result(transFunc);

		transFunc->setRoot(result.GetRegToken(), lhs, root);

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
			result.GetRegToken(), translationTable);
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

				// find roots for unary symbols with the states
				LeftHandSideType lhs1(1);
				lhs1[0] = prod.first;
				LeftHandSideType lhs2(1);
				lhs2[0] = prod.second;
				RootType root1 = transFunc->getRootForArity1(ta1.GetRegToken(), lhs1);
				RootType root2 = transFunc->getRootForArity1(ta2.GetRegToken(), lhs2);

				if ((root1 != transFunc->sinkState_) && (root2 != transFunc->sinkState_))
				{	// in case both states have successors
					LeftHandSideType newLhs;
					newLhs.push_back(newState);
					root = mtbdd.Apply(root1, root2, &intersectFunc);
					transFunc->setRootForArity1(result.GetRegToken(), newLhs, root);
				}

				// find roots for binary symbols with the states
				lhs1 = LeftHandSideType(2);

				// TODO: this could be done more efficiently using proper data
				// structures!

				// bind to the first position
				for (size_t i = 0; i < transFunc->container2_.size(); ++i)
				{
					for (size_t j = 0; j < transFunc->container2_.size(); ++j)
					{	// for each pair of states
						if (translationTable.find(std::make_pair(i,j)))
						{
						}

						transFunc->container2_[prod.first][i];
						transFunc->container2_[prod.second][j];
					}
				}

				// bind to the second position
			}
		} while (!queueOfProducts.empty());

		return result;
	}
};

#endif
