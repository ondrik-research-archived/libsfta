/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2011  Ondra Lengal <ilengal@fit.vutbr.cz>
 *
 *  Description:
 *    My own implementation of multi-terminal binary decision diagrams (MTBDDs)
 *
 *****************************************************************************/

#ifndef _ONDRIKS_MTBDD_HH_
#define _ONDRIKS_MTBDD_HH_


// Standard library headers
#include	<cassert>
#include	<stdint.h>
#include	<stdexcept>
#include	<vector>

// SFTA headers
#include	<sfta/compact_variable_assignment.hh>

// Loki headers
#include	<loki/SmartPtr.h>


namespace SFTA
{
	namespace Private
	{
		namespace MTBDDPkg
		{
			template <
				typename Data
			>
			class OndriksMTBDD;

			template <
				typename Data1,
				typename Data2,
				typename DataOut
			>
			class AbstractApply2Functor;

			namespace MTBDDNodePkg
			{
				/**
				 * @brief  MTBDD node
				 *
				 * Union for MTBDD node. It holds either data (for leaf (or sink)
				 * nodes), or pointers to low and high nodes together with variable
				 * name (for internal nodes). The type of the node (leaf or internal) is
				 * determined using the pointer to the node: if the address is even, then
				 * the node is internal, if it is odd, then it is leaf.
				 *
				 * @tparam  Data  The type of data to be stored into leaf nodes
				 */
				template
				<
					typename Data
				>
				union MTBDDNode
				{
					// Data types
					typedef Data DataType;
					typedef uintptr_t VarType;

					// Disjunct data
					struct
					{
						MTBDDNode* low;
						MTBDDNode* high;
						VarType var;
					} internal;

					DataType data;
				};


				template <class NodeType>
				inline NodeType* makeLeaf(NodeType* node)
				{
					// Assertions
					assert(isLeaf(node));

					return node | 1;
				}

				template <class NodeType>
				inline NodeType* makeInternal(NodeType* node)
				{
					// Assertions
					assert(isInternal(node));

					return node;
				}

				template <class NodeType>
				inline bool isLeaf(const NodeType* node)
				{
					return (reinterpret_cast<uintptr_t>(node) % 2);
				}

				template <class NodeType>
				inline bool isInternal(const NodeType* node)
				{
					return !(isLeaf(node));
				}

				template <class NodeType>
				inline NodeType* leafToNode(NodeType* node)
				{
					// Assertions
					assert(isLeaf(node));

					return reinterpret_cast<NodeType*>(
						reinterpret_cast<uintptr_t>(node) ^ 1);
				}

				template <class NodeType>
				inline NodeType* internalToNode(NodeType* node)
				{
					// Assertions
					assert(isInternal(node));

					return node;
				}


				template <class NodeType>
				NodeType* createLeaf(const typename NodeType::DataType& data)
				{
					// TODO: create allocator						
					NodeType* newNode = new NodeType;
					newNode->data = data;

					return makeLeaf(newNode);
				}

				template <class NodeType>
				NodeType* createInternal(
					NodeType* low, NodeType* high, typename NodeType::VarType var)
				{
					// Assertions
					assert(low != static_cast<NodeType*>(0));
					assert(high != static_cast<NodeType*>(0));

					// TODO: create allocator
					NodeType* newNode = new NodeType;
					newNode->low = low;
					newNode->high = high;
					newNode->var = var;

					return makeInternal(newNode);
				}

				template <class NodeType>
				void deleteNode(NodeType* node)
				{
					if (isLeaf(node))
					{
						delete leafToNode(node);
					}
					else if (isInternal(node))
					{
						delete internalToNode(node);
					}
					else
					{
						throw std::runtime_error("Invalid type of MTBDD node.");
					}
				}

				template <class NodeType>
				void deleteNodeRecursively(NodeType* node)
				{
					if (isInternal(node))
					{
						deleteNodeRecursively(makeInternal(node)->internal.low);
						deleteNodeRecursively(makeInternal(node)->internal.high);
					}

					deleteNode(node);
				}
			}
		}
	}
}


/**
 * @brief   Class representing MTBDD
 * @author  Ondra Lengal <ilengal@fit.vutbr.cz>
 * @date    2011
 *
 * This class represents a single multi-terminal binary decision diagram
 * (MTBDD).
 *
 * @tparam  Data  The type of leaves
 */
template <
	typename Data
>
class SFTA::Private::MTBDDPkg::OndriksMTBDD
{
public:   // public data types

	typedef Data DataType;

	typedef SFTA::Private::MTBDDPkg::MTBDDNodePkg::MTBDDNode<DataType> MTBDDNode;

	typedef std::vector<typename MTBDDNode::VarType> PermutationTable;

	typedef Loki::SmartPtr<PermutationTable> PermutationTablePtr;

	typedef CompactVariableAssignment VariableAssignment;

private: // private data members

	// TODO: add something different than simple pointer (e.g. smart pointer
	// with destructive copy
	MTBDDNode* root_;

	DataType defaultValue_;

	PermutationTablePtr varOrdering_;


public:  // public methods


	/**
	 * @brief  Constructor with default variable ordering
	 *
	 * This constructor creates a new MTBDD with the default variable ordering,
	 * such that the variable assignment @p asgn is set to @p value and all
	 * other assignments are set to @p defaultValue.
	 *
	 * @param  asgn          Variable assignment to be set to @p value
	 * @param  value         Value to be set for assignment @p asgn
	 * @param  defaultValue  Value to be set for all assignments other than @p
	 *                       asgn
	 */
	OndriksMTBDD(const VariableAssignment& asgn,
		const DataType& value, const DataType& defaultValue)
		: root_(static_cast<MTBDDNode*>(0)),
			defaultValue_(defaultValue),
			varOrdering_(static_cast<PermutationTable*>(0))
	{
		assert(false);
		assert(&asgn != 0);
		assert(&value != 0);
	}


	/**
	 * @brief  Constructor with given variable ordering
	 *
	 * This constructor creates a new MTBDD with the variable ordering given as
	 * @p table, such that the variable assignment @p asgn is set to @p value
	 * and all other assignments are set to @p defaultValue.
	 *
	 * @param  asgn          Variable assignment to be set to @p value
	 * @param  value         Value to be set for assignment @p asgn
	 * @param  defaultValue  Value to be set for all assignments other than @p
	 *                       asgn
	 * @param  table         Permutation table for variable ordering
	 */
	OndriksMTBDD(const VariableAssignment& asgn, const DataType& value,
		const DataType& defaultValue, const PermutationTablePtr& table)
		: defaultValue_(defaultValue)
	{
		if (asgn.VariablesCount() > table.size())
		{
			throw std::runtime_error(
				"Variable assignment contains variables with an unknown ordering");
		}

		assert(false);
	}

	OndriksMTBDD(const OndriksMTBDD&);
	OndriksMTBDD& operator=(const OndriksMTBDD&);

	inline const DataType& DefaultValue() const
	{
		return defaultValue_;
	}

	inline size_t MaxHeight() const
	{
		return varOrdering_.size();
	}

	inline PermutationTablePtr GetVarOrdering() const
	{
		return varOrdering_;
	}

	/**
	 * @brief  Returns value for assignment
	 *
	 * Thsi method returns a value in the MTBDD that corresponds to given
	 * variable assignment @p asgn.
	 *
	 * @note  If there are multiple values that correspond to given variable
	 * assignment (e.g., because the assignment is very nondeterministic), an
	 * arbitrary value is returned.
	 *
	 * @param  asgn  Variable assignment
	 *
	 * @return  Value corresponding to variable assignment @p asgn
	 */
	const DataType& GetValue(const VariableAssignment& asgn) const
	{
		assert(false);
		assert(&asgn != 0);
	}

	~OndriksMTBDD()
	{
		assert(root_ != static_cast<MTBDDNode*>(0));

		deleteNodeRecursively(root_);
	}
};

template <
	typename Data1,
	typename Data2,
	typename DataOut
>
class SFTA::Private::MTBDDPkg::AbstractApply2Functor
{
public:   // Public data types

	typedef Data1 Data1Type;
	typedef Data2 Data2Type;
	typedef DataOut DataOutType;

	typedef OndriksMTBDD<Data1Type> MTBDD1Type;
	typedef OndriksMTBDD<Data2Type> MTBDD2Type;
	typedef OndriksMTBDD<DataOutType> MTBDDOutType;

private:  // Private data members

	const MTBDD1Type* pMtbdd1;
	const MTBDD2Type* pMtbdd2;

private:  // Private methods

	AbstractApply2Functor(const AbstractApply2Functor&);
	AbstractApply2Functor& operator=(const AbstractApply2Functor&);

public:   // Public methods

	AbstractApply2Functor()
		: pMtbdd1(static_cast<MTBDD1Type*>(0)),
			pMtbdd2(static_cast<MTBDD2Type*>(0))
	{ }

	MTBDDOutType operator()(const MTBDD1Type& mtbdd1, const MTBDD2Type& mtbdd2)
	{
		assert(false);
		assert(&mtbdd1 != 0);
		assert(&mtbdd2 != 0);

		// check ordering
	}

	virtual DataOutType DataOperation(
		const Data1Type& data1, const Data2Type& data2) = 0;

protected:// Protected methods

	inline const MTBDD1Type& GetMTBDD1() const
	{
		assert(pMtbdd1 != static_cast<MTBDD1Type*>(0));
		return *pMtbdd1;
	}


	virtual ~AbstractApply2Functor()
	{ }
};

#endif
