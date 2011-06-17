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

// Standard library headers
#include <queue>
#include <tr1/unordered_map>

// Boost libraries
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

private:  // Private data types

	typedef SFTA::Private::Convert Convert;
	
	typedef typename HierarchyRoot::Operation::SimulationRelationType
		SimulationRelationType;

public:   // Public data types

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

		class InclusionCheckingFunctor
		{
		private:  // Private data types

			typedef std::vector<StateType> StateVector;

			//typedef std::vector<StateType> StateSetType;
			typedef OrderedVector<StateType> StateSetType;
			typedef std::list<StateSetType> StateSetListType;
			typedef std::pair<StateType, StateSetType> DisjunctType;
			typedef std::queue<DisjunctType> DisjunctQueueType;
			typedef std::list<DisjunctType> DisjunctListType;
			typedef std::vector<DisjunctType> SetOfDisjunctsType;
			typedef std::queue<SetOfDisjunctsType> SetOfDisjunctsQueueType;
			typedef std::tr1::unordered_map<StateType, StateSetListType>
				StateToStateSetListHashTableType;

		private:  // Private data types

				struct AndNode;

				struct OrNode
				{
					std::vector<AndNode*> parents_;
					std::vector<AndNode*> disjuncts_;

					OrNode()
						: parents_(),
							disjuncts_()
					{ }

					OrNode(AndNode* parent)
						: parents_(1, parent),
							disjuncts_()
					{ }

					friend std::ostream& operator<<(std::ostream& os, const OrNode& node)
					{
						os << "(Or";

						for (typename std::vector<AndNode*>::const_iterator itDisjuncts
							= node.disjuncts_.begin(); itDisjuncts != node.disjuncts_.end();
							++itDisjuncts)
						{
							os << " " << Convert::ToString(*itDisjuncts);
						}

						os << ")";

						return os;
					}
				};

				typedef unsigned char ArityType;

				typedef std::vector<ArityType> ChoiceFunctionType; // serves as choiceFunction: rhs -> {1...arity}
																													 // a |-> 0 means that a is not assigned position

				typedef std::pair<ChoiceFunctionType, OrNode*> ChoiceFunctionNodeType;

				struct AndNode
				{
				private:

					AndNode(const AndNode&);
					AndNode& operator=(const AndNode&);

				public:
					OrNode* parent_;
					std::vector<ChoiceFunctionNodeType> choiceFunctions_;

					AndNode(OrNode* parent, ArityType arity, size_t size)
						: parent_(parent),
							choiceFunctions_(arity, ChoiceFunctionNodeType(ChoiceFunctionType(size, 0), static_cast<OrNode*>(0)))
					{ }

					AndNode(OrNode* parent, ArityType arity, const ChoiceFunctionType& cf)
						: parent_(parent),
							choiceFunctions_(arity, ChoiceFunctionNodeType(cf, static_cast<OrNode*>(0)))
					{ }

					friend std::ostream& operator<<(std::ostream& os, const AndNode& node)
					{
						os << "(And";

						for (typename std::vector<ChoiceFunctionNodeType>::const_iterator itCfs
							= node.choiceFunctions_.begin(); itCfs != node.choiceFunctions_.end();
							++itCfs)
						{
							os << " " << ((itCfs->second == static_cast<OrNode*>(0))? "_|_" : Convert::ToString(itCfs->second));
						}

						os << ")";

						os << ")";

						return os;
					}
				};

				struct HasherNnary
				{
					size_t operator()(const ChoiceFunctionType& key) const
					{
						return boost::hash_range(key.begin(), key.end());
					}
				};

		private:  // Private data members

			const Type* smallerAut_;
			const Type* biggerAut_;

			StateToStateSetListHashTableType workset_;
			StateToStateSetListHashTableType includedNodes_;
			StateToStateSetListHashTableType nonincludedNodes_;

			const SimulationRelationType* simSmaller_;
			const SimulationRelationType* simBigger_;

		private:  // Private methods

			InclusionCheckingFunctor(const InclusionCheckingFunctor&);
			InclusionCheckingFunctor& operator=(const InclusionCheckingFunctor&);

			template <class T>
			bool forallExists(const T& smaller, const T& bigger,
				const SimulationRelationType& sim) const
			{
				// ASSUME that elements of smaller and bigger are sorted

				// pro kazdy prvek ze smaller si nactu sim(smaller) a divam se, jestli
				// prunik s bigger je neprazdny

				for (typename T::const_iterator itSmaller = smaller.begin();
					itSmaller != smaller.end(); ++itSmaller)
				{
					const std::set<StateType>& smallerSims = sim.GetSimulators(*itSmaller);

					typename std::set<StateType>::const_iterator itSmallerSim = smallerSims.begin();
					typename T::const_iterator itBigger = bigger.begin();

					while (true)
					{
						if ((itBigger == bigger.end()) || itSmallerSim == smallerSims.end())
						{
							return false;
						}
						else if (*itSmallerSim == *itBigger)
						{
							break;
						}
						else if (*itSmallerSim < *itBigger)
						{
							++itSmallerSim;
						}
						else
						{
							++itBigger;
						}
					}
				}

				return true;
			}


			bool isInclusionCached(const DisjunctType& disjunct) const
			{
				// check whether there exists some state in disjunct.second that
				// simulates disjunct.first
				for (typename StateSetType::const_iterator itDis = disjunct.second.begin();
					itDis != disjunct.second.end(); ++itDis)
				{
					if (simSmaller_->is_in(std::make_pair(disjunct.first, *itDis)))
					{
						if (biggerAut_->getStates().find(*itDis) != biggerAut_->getStates().end())
						{
							return true;
						}
					}
				}

				// TODO: search for smaller sets
				typename StateToStateSetListHashTableType::const_iterator itHashTable;
				if ((itHashTable = includedNodes_.find(disjunct.first)) != includedNodes_.end())
				{
					const StateSetListType& listOfStateSets = itHashTable->second;

					for (typename StateSetListType::const_iterator itInclNodes = listOfStateSets.begin();
						itInclNodes != listOfStateSets.end(); ++itInclNodes)
					{
						if (forallExists(*itInclNodes, disjunct.second, *simBigger_))
						{
							return true;
						}
					}
				}

				return false;
			}

			bool isNoninclusionCached(const DisjunctType& disjunct) const
			{
				// TODO: search for bigger sets
				typename StateToStateSetListHashTableType::const_iterator itHashTable;
				if ((itHashTable = nonincludedNodes_.find(disjunct.first)) != nonincludedNodes_.end())
				{
					const StateSetListType& listOfStateSets = itHashTable->second;

					for (typename StateSetListType::const_iterator itNoninclNodes = listOfStateSets.begin();
						itNoninclNodes != listOfStateSets.end(); ++itNoninclNodes)
					{
						if (forallExists(disjunct.second, *itNoninclNodes, *simBigger_))
						{
							return true;
						}
					}
				}

				return false;
			}

			bool isImpliedByWorkset(const DisjunctType& disjunct) const
			{
				// TODO: search for smaller sets
				typename StateToStateSetListHashTableType::const_iterator itHashTable;
				if ((itHashTable = workset_.find(disjunct.first)) != workset_.end())
				{
					const StateSetListType& listOfStateSets = itHashTable->second;

					for (typename StateSetListType::const_iterator itWorkset = listOfStateSets.begin();
						itWorkset != listOfStateSets.end(); ++itWorkset)
					{
						if (forallExists(*itWorkset, disjunct.second, *simBigger_))
						{
							return true;
						}
					}
				}

				return false;
			}

			bool isImpliedByChildren(const DisjunctListType& children,
				const DisjunctType& disjunct) const
			{
				// TODO: search for bigger children
				for (typename DisjunctListType::const_iterator itChildren = children.begin();
					itChildren != children.end(); ++itChildren)
				{
					if (itChildren->first == disjunct.first)
					{	// in case the ``smaller'' state matches
						if (forallExists(disjunct.second, itChildren->second, *simBigger_))
						{
							return true;
						}
					}
				}

				return false;
			}

			void addToWorkset(const DisjunctType& disjunct)
			{
				typename StateToStateSetListHashTableType::iterator itHashTable;
				if ((itHashTable = workset_.find(disjunct.first)) == workset_.end())
				{
					StateSetListType newList;
					itHashTable = workset_.insert(workset_.end(),
						std::make_pair(disjunct.first, newList));
				}

				itHashTable->second.push_back(disjunct.second);
			}

			void removeFromWorkset(const DisjunctType& disjunct)
			{
				typename StateToStateSetListHashTableType::iterator itHashTable;
				if ((itHashTable = workset_.find(disjunct.first)) == workset_.end())
				{
					throw std::runtime_error(__func__ +
						std::string(": an attempt to remove non-existing state"));
				}

				StateSetListType& stateSetList = itHashTable->second;
				typename StateSetListType::iterator itStateSetList;
				if ((itStateSetList = std::find(stateSetList.begin(), stateSetList.end(),
					disjunct.second)) == stateSetList.end())
				{
					throw std::runtime_error(__func__ +
						std::string(": an attempt to remove non-existing state set"));
				}

				stateSetList.erase(itStateSetList);
			}

			void addToChildren(DisjunctListType& children,
				const DisjunctType& disjunct)
			{
				// TODO: remove smaller children
				children.push_back(disjunct);
			}

			void cacheInclusion(const DisjunctType& disjunct)
			{
				// TODO: remove bigger sets??
				typename StateToStateSetListHashTableType::iterator itHashTable;
				if ((itHashTable = includedNodes_.find(disjunct.first)) == includedNodes_.end())
				{
					StateSetListType newList;
					itHashTable = includedNodes_.insert(includedNodes_.end(),
						std::make_pair(disjunct.first, newList));
				}

				itHashTable->second.push_back(disjunct.second);
			}

			void cacheNoninclusion(const DisjunctType& disjunct)
			{
				// TODO: remove smaller sets??
				typename StateToStateSetListHashTableType::iterator itHashTable;
				if ((itHashTable = nonincludedNodes_.find(disjunct.first)) == nonincludedNodes_.end())
				{
					StateSetListType newList;
					itHashTable = nonincludedNodes_.insert(nonincludedNodes_.end(),
						std::make_pair(disjunct.first, newList));
				}

				itHashTable->second.push_back(disjunct.second);
			}

			bool expandDisjunct(const DisjunctType& disjunct)
			{
/*				if (isInclusionCached(disjunct))
				{
					return true;
				}
				else */if (isNoninclusionCached(disjunct))
				{
					return false;
				}
				else if (isImpliedByWorkset(disjunct))
				{
					return true;
				}
				else if (expandSubset(disjunct))
				{
//					cacheInclusion(disjunct);
					return true;
				}
				else
				{
					cacheNoninclusion(disjunct);
					return false;
				}
			}

			bool expandSubset(const DisjunctType& disjunct)
			{
				class UnionApplyFunctor
					: public SharedMTBDDType::AbstractApplyFunctorType
				{
				public:

					virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs)
					{
						LeafType result = lhs;
						result.insert(rhs);

						return result;
					}
				};

				class ChildrenCollectorFunctor
					: public SharedMTBDDType::AbstractApplyFunctorType
				{
				private:

					bool doesInclusionHold_;

					InclusionCheckingFunctor* inclFunc_;

				private:

					ChildrenCollectorFunctor(const ChildrenCollectorFunctor&);
					ChildrenCollectorFunctor& operator=(const ChildrenCollectorFunctor&);

				public:

					ChildrenCollectorFunctor(InclusionCheckingFunctor* inclFunc)
						: doesInclusionHold_(true),
							inclFunc_(inclFunc)
					{ }

					inline bool DoesInclusionHold()
					{
						return doesInclusionHold_;
					}


					/**
					 * removes 'orNode', 'callerParent' is the parent AndNode that
					 * deallocates 'orNode'
					 */
					void recursivelyDeallocate(OrNode* orNode, AndNode* callerParent)
					{
						if (orNode != static_cast<OrNode*>(0))
						{
							if (callerParent != static_cast<AndNode*>(0))
							{	// in case 'orNode' is not the root, remove 'orNode' reference from
								// all its parents other than 'callerParent'
								assert(!callerParent->choiceFunctions_.empty());

								bool foundParent = false;
								for (typename std::vector<AndNode*>::iterator itPar
									= orNode->parents_.begin();
									itPar != orNode->parents_.end(); ++itPar)
								{
									AndNode* parentAnd = *itPar;
									assert(parentAnd != static_cast<AndNode*>(0));

									if (parentAnd == callerParent)
									{	// do not remove from this one
										foundParent = true;
									}
									else
									{
										bool foundOrNode = false;
										for (typename std::vector<ChoiceFunctionNodeType>::iterator itCf
											= parentAnd->choiceFunctions_.begin();
											itCf != parentAnd->choiceFunctions_.end(); ++itCf)
										{
											OrNode* tmpOrNode = itCf->second;
											assert(tmpOrNode != static_cast<OrNode*>(0));

											if (orNode == tmpOrNode)
											{
												foundOrNode = true;
												parentAnd->choiceFunctions_.erase(itCf);
												break;
											}
										}

										assert(foundOrNode);
									}
								}

								assert(foundParent);
							}

							for (typename std::vector<AndNode*>::iterator itDis
								= orNode->disjuncts_.begin();
								itDis != orNode->disjuncts_.end(); ++itDis)
							{
								AndNode* andNode = *itDis;
								assert(andNode != static_cast<AndNode*>(0));

								for (typename std::vector<ChoiceFunctionNodeType>::iterator itCfs
									= andNode->choiceFunctions_.begin();
									itCfs != andNode->choiceFunctions_.end(); ++itCfs)
								{	// deallocate children of the AndNode
									recursivelyDeallocate(itCfs->second, andNode);
								}

								delete andNode;
							}

							delete orNode;
						}
					}

					bool isOrNodeLeaf(const OrNode* orNode)
					{
						assert(orNode != static_cast<OrNode*>(0));

						if (!orNode->disjuncts_.empty())
						{
							assert(orNode->disjuncts_[0] != static_cast<AndNode*>(0));
							assert(!orNode->disjuncts_[0]->choiceFunctions_.empty());

							return (orNode->disjuncts_[0]->choiceFunctions_[0].second
								== static_cast<OrNode*>(0));
						}
						else
						{
							return true;
						}
					}

					void markForNotProcessing(OrNode* orNode)
					{
						assert(orNode != static_cast<OrNode*>(0));
						if (orNode->parents_.empty())
						{
							if (!isOrNodeLeaf(orNode))
							{	// leaf nodes cannot be deleted (as they are in the queue)
								for (typename std::vector<AndNode*>::iterator itDis
									= orNode->disjuncts_.begin();
									itDis != orNode->disjuncts_.end(); ++itDis)
								{
									AndNode* andNode = *itDis;
									assert(andNode != static_cast<AndNode*>(0));

									for (typename std::vector<ChoiceFunctionNodeType>::iterator itCf
										= andNode->choiceFunctions_.begin();
										itCf != andNode->choiceFunctions_.end(); ++itCf)
									{
										OrNode* lowerOrNode = itCf->second;
										assert(lowerOrNode != static_cast<OrNode*>(0));

										// remove 'andNode' from parents of 'lowerOrNode'
										bool found = false;
										for (typename std::vector<AndNode*>::iterator itPar
											= lowerOrNode->parents_.begin();
											itPar != lowerOrNode->parents_.end(); ++itPar)
										{
											assert(*itPar != static_cast<AndNode*>(0));
											if (*itPar == andNode)
											{
												found = true;
												lowerOrNode->parents_.erase(itPar);
												break;
											}
										}

										assert(found);

										markForNotProcessing(lowerOrNode);
									}

									delete andNode;
								}

								delete orNode;
							}
						}
					}

					
					/**
					 *
					 * @return  True if the root was satisfied, false otherwise
					 */
					bool satisfyNode(OrNode* orNode)
					{
						// Assertions
						assert(orNode != static_cast<OrNode*>(0));

						bool result = false;

						//
						// downwards processing 
						//
						for (typename std::vector<AndNode*>::iterator itDis
							= orNode->disjuncts_.begin(); itDis != orNode->disjuncts_.end();
							++itDis)
						{	// for every child AndNode 'andNode'
							AndNode* andNode = *itDis;
							assert(andNode != static_cast<AndNode*>(0));

							for (typename std::vector<ChoiceFunctionNodeType>::iterator itCf
								= andNode->choiceFunctions_.begin();
								itCf != andNode->choiceFunctions_.end(); ++itCf)
							{
								// for every child OrNode 'tmpOrNode' of 'andNode'
								OrNode* tmpOrNode = itCf->second;

								if (tmpOrNode != static_cast<OrNode*>(0))
								{
									// remove 'andNode' from parents of 'tmpOrNode'
									bool found = false;
									for (typename std::vector<AndNode*>::iterator itPar
										= tmpOrNode->parents_.begin();
										itPar != tmpOrNode->parents_.end(); ++itPar)
									{
										if (*itPar == andNode)
										{
											found = true;
											tmpOrNode->parents_.erase(itPar);
											break;
										}
									}

									assert(found);
									markForNotProcessing(tmpOrNode);
								}
							}

							delete andNode;
						}

						//
						// upwards processing 
						//
						if (orNode->parents_.empty())
						{	// this should be the root node
							delete orNode;
							return true;
						}

						for (typename std::vector<AndNode*>::iterator itParAnds
							= orNode->parents_.begin(); itParAnds != orNode->parents_.end();
							++itParAnds)
						{	// traverse the set of parents 'parAndNode' of 'orNode'
							AndNode* parAndNode = *itParAnds;
							assert(parAndNode != static_cast<AndNode*>(0));
							assert(parAndNode->parent_ != static_cast<OrNode*>(0));

							// remove 'orNode' from children of 'parAndNode'
							bool found = false;
							for (typename std::vector<ChoiceFunctionNodeType>::iterator itParChild
								= parAndNode->choiceFunctions_.begin();
								itParChild != parAndNode->choiceFunctions_.end(); ++itParChild)
							{
								if (itParChild->second == orNode)
								{
									found = true;
									parAndNode->choiceFunctions_.erase(itParChild);
									break;
								}
							}

							assert(found);

							if (parAndNode->choiceFunctions_.empty())
							{	// in case we also satisfied the abovelying layer
								result = satisfyNode(parAndNode->parent_);
							}
						}

						delete orNode;

						return result;
					}


					bool checkInclusion(const std::vector<StateType>& sm,
						const std::vector<SFTA::Private::ElemOrVector<StateType> >& bigger)
					{
						unsigned arity = sm.size();

						// the workqueue
						std::queue<OrNode*> nodeQueue;

						// create the root nodes
						OrNode* root = new OrNode();
						root->disjuncts_.push_back(new AndNode(root, 1, bigger.size()));

						typedef std::tr1::unordered_map<ChoiceFunctionType, OrNode*, HasherNnary>
						 	CFOrHashTableType;
						CFOrHashTableType orNodeCache;

						nodeQueue.push(root);

						bool inclusionHolds = false;
						while (!nodeQueue.empty())
						{
							OrNode* orNode = nodeQueue.front();
							assert(orNode != static_cast<OrNode*>(0));
							assert(isOrNodeLeaf(orNode));
							nodeQueue.pop();

							if (orNode->parents_.empty() && orNode != root)
							{	// in case the Or node does not need processing
								assert(isOrNodeLeaf(orNode));
								for (typename std::vector<AndNode*>::iterator itDis
									= orNode->disjuncts_.begin(); itDis != orNode->disjuncts_.end();
									++itDis)
								{
									AndNode* andNode = *itDis;
									assert(andNode != static_cast<AndNode*>(0));

									delete andNode;
								}

								delete orNode;
							}
							else
							{	// in case the Or node needs processing
								assert(!inclusionHolds);

								for (typename std::vector<AndNode*>::const_iterator itDisjuncts
									= orNode->disjuncts_.begin(); itDisjuncts != orNode->disjuncts_.end();
									++itDisjuncts)
								{	// for each disjunct in the Or
									AndNode* andNode = *itDisjuncts;
									assert(andNode != static_cast<AndNode*>(0));
									assert(andNode->choiceFunctions_.size() > 0);

									bool incrementIndex = true;
									for (size_t index = 0, realPosition = 0;
										index < andNode->choiceFunctions_.size();
										index = incrementIndex? index+1 : index, ++realPosition)
									{ // Note: 'index' is the pointer into the array
										// andNode->choiceFunctions_ and 'realPosition' is the
										// absolute position of the AndNode
										incrementIndex = true;

										const ChoiceFunctionType& cf = andNode->choiceFunctions_[index].first;
										assert(andNode->choiceFunctions_[index].second == static_cast<OrNode*>(0));

										StateSetType subset;
										for (size_t i = 0; i < cf.size(); ++i)
										{	// collect items in the choice function assigned to 'index'-th position
											if (cf[i] == realPosition + 1)
											{
												subset.insert(bigger[i].GetVector()[realPosition]);
											}
										}

										if (inclFunc_->expandDisjunct(std::make_pair(sm[realPosition], subset)))
										{	// in case the inclusion holds
											incrementIndex = false;
											andNode->choiceFunctions_.erase(andNode->choiceFunctions_.begin() + index);
										}
										else
										{	// in case the inclusion doesn't hold
											typename CFOrHashTableType::const_iterator itCache;
											if ((itCache = orNodeCache.find(andNode->choiceFunctions_[index].first))
												!= orNodeCache.end())
											{	// in case the OrNode is already cached
												OrNode* cachedOrNode = itCache->second;

												if (cachedOrNode != static_cast<OrNode*>(0))
												{	// in case we are not at leaves
													andNode->choiceFunctions_[index].second = cachedOrNode;
													cachedOrNode->parents_.push_back(andNode);
												}
											}
											else
											{	// in case we haven't seen the OrNode yet
												OrNode* newOrNode = new OrNode(andNode);
												andNode->choiceFunctions_[index].second = newOrNode;

												bool foundNew = false;
												for (size_t i = 0; i < cf.size(); ++i)
												{
													if (cf[i] == 0)
													{
														foundNew = true;
														AndNode* newAnd = new AndNode(newOrNode, arity, cf);
														for (size_t ind = 0; ind < arity; ++ind)
														{
															newAnd->choiceFunctions_[ind].first[i] = ind + 1;
														}

														assert(!newAnd->choiceFunctions_.empty());
														newOrNode->disjuncts_.push_back(newAnd);
													}
												}

												if (foundNew)
												{
													assert(!newOrNode->disjuncts_.empty());
													nodeQueue.push(newOrNode);
												}
												else
												{
													assert(newOrNode->disjuncts_.empty());
													delete newOrNode;
													newOrNode = static_cast<OrNode*>(0);
													andNode->choiceFunctions_[index].second = newOrNode;
												}

												orNodeCache.insert(std::make_pair(andNode->choiceFunctions_[index].first, newOrNode));
											}
										}
									}

									if (andNode->choiceFunctions_.empty())
									{	// in case the OrNode 'orNode' is satisfied by one AndNode
										// (in this case 'andNode')
										if (satisfyNode(orNode))
										{
											inclusionHolds = true;
											root = static_cast<OrNode*>(0);
										}

										break;
									}
								}
							}
						}

						recursivelyDeallocate(root, static_cast<AndNode*>(0));
						return inclusionHolds;
					}

					virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs)
					{
						LeafType result;

						if (!doesInclusionHold_ || lhs.empty())
						{	// in case it is pointless to compute anything
							return result;				// don't waste time
						}

						unsigned arity = lhs.begin()->GetVector().size();

						if (arity == 0)
						{	// the ``smaller'' state can make a nullary transition
							if (rhs.find(typename SFTA::Private::ElemOrVector<StateType>::VectorType())
								== rhs.end())
							{	// in case the ``bigger'' state cannot make such a transition
								doesInclusionHold_ = false;
								return result;
							}
							else
							{
								return result;
							}
						}

						const std::vector<SFTA::Private::ElemOrVector<StateType> >& rhsVector =
							rhs.ToVector();

						for (typename LeafType::const_iterator itLhs = lhs.begin();
							itLhs != lhs.end(); ++itLhs)
						{
							if (!checkInclusion(itLhs->GetVector(), rhsVector))
							{
								doesInclusionHold_ = false;
								break;
							}
						}
						return result;
					}
				};

				const StateType& smallerState = disjunct.first;
				const StateSetType& biggerSetOfStates = disjunct.second;

				SharedMTBDDType* mtbdd = smallerAut_->GetTTWrapper()->GetMTBDD();

				RootType unionBigger = mtbdd->CreateRoot();
				UnionApplyFunctor unionFunc;

				for (typename StateVector::const_iterator itBiggerStates =
					biggerSetOfStates.begin(); itBiggerStates != biggerSetOfStates.end();
					++itBiggerStates)
				{
					RootType biggerRoot = biggerAut_->getRoot(*itBiggerStates);
					RootType tmp = mtbdd->Apply(unionBigger,
						biggerRoot, &unionFunc);

					// Erase the following line for better performance ;-)
					//mtbdd->EraseRoot(unionBigger);

					unionBigger = tmp;
				}

				SetOfDisjunctsQueueType childrenQueue;

				addToWorkset(disjunct);
				ChildrenCollectorFunctor childColFunc(this);

				RootType tmp = mtbdd->Apply(smallerAut_->getRoot(smallerState),
					unionBigger, &childColFunc);
				mtbdd->EraseRoot(tmp);
				removeFromWorkset(disjunct);

				return childColFunc.DoesInclusionHold();
			}

		public:   // Public methods

			InclusionCheckingFunctor(const Type* smallerAut, const Type* biggerAut, const SimulationRelationType* simSmaller, const SimulationRelationType* simBigger)
				: smallerAut_(smallerAut),
					biggerAut_(biggerAut),
					workset_(),
					includedNodes_(),
					nonincludedNodes_(),
					simSmaller_(simSmaller),
					simBigger_(simBigger)
			{
				// Assertions
				assert(smallerAut_ != static_cast<Type*>(0));
				assert(biggerAut_ != static_cast<Type*>(0));
				assert(simSmaller_ != static_cast<SimulationRelationType*>(0));
				assert(simBigger_ != static_cast<SimulationRelationType*>(0));
			}

			bool operator ()()
			{
				// array of states
				StateVector smallerInitStates = smallerAut_->GetVectorOfInitialStates();
				StateVector biggerInitStates = biggerAut_->GetVectorOfInitialStates();

				for (typename StateVector::const_iterator itSmallerInitStates =
					smallerInitStates.begin(); itSmallerInitStates != smallerInitStates.end();
					++itSmallerInitStates)
				{
					if (!expandSubset(std::make_pair(*itSmallerInitStates, biggerInitStates)))
					{
						return false;
					}
				}

				return true;
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

		virtual SimulationRelationType* ComputeSimulationPreorder(
			const HierarchyRoot* aut) const
		{
			assert(aut != static_cast<const HierarchyRoot*>(0));

			throw std::runtime_error(__func__ + std::string(": not implemented"));
		}

		virtual bool CheckLanguageInclusion(const HierarchyRoot* a1,
			const HierarchyRoot* a2, const SimulationRelationType* simA1,
			const SimulationRelationType* simA2) const
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

			InclusionCheckingFunctor inclFunc(a1Sym, a2Sym, simA1, simA2);
			return inclFunc();
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
