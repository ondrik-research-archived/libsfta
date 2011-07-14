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

// SFTA headers
#include	<sfta/compact_variable_assignment.hh>


namespace SFTA
{
	namespace Private
	{
		template <
			typename Data
		>
		class OndriksMTBDD;

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
				};

				DataType data;
			};


			template <class NodeType>
			inline NodeType* makeLeaf(NodeType* node)
			{
				// Assertions
				assert(node % 2 == 0);

				return node | 1;
			}

			template <class NodeType>
			inline NodeType* makeInternal(NodeType* node)
			{
				// Assertions
				assert(node % 2 == 0);

				return node;
			}

			template <class NodeType>
			inline bool isLeaf(const NodeType* node)
			{
				return (node % 2);
			}

			template <class NodeType>
			inline bool isInternal(const NodeType* node)
			{
				return !(node % 2);
			}

			template <class NodeType>
			inline NodeType* leafToNode(NodeType* node)
			{
				// Assertions
				assert(isLeaf(node));

				return node ^ 1;
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
					deleteNodeRecursively(makeInternal(node)->low);
					deleteNodeRecursively(makeInternal(node)->high);
				}

				deleteNode(node);
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
class SFTA::Private::OndriksMTBDD
{
public:   // public data types

	typedef Data DataType;

	typedef SFTA::Private::MTBDDNodePkg::MTBDDNode<DataType> MTBDDNode;

private: // private data members

	MTBDDNode* root_;

public:  // public methods

	explicit OndriksMTBDD(const CompactVariableAssignment& asgn)
	{
		assert(false);
	}

	~OndriksMTBDD()
	{
		assert(root_ != static_cast<MTBDDNode*>(0));

		deleteNodeRecursively(root_);
	}
                                                
};

#endif
