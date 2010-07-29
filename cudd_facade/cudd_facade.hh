/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file of CUDDFacade. This file defines the interface of the facad
 *    for the outside world and hides everything related to CUDD.
 *
 *****************************************************************************/

#ifndef _SFTA_CUDD_FACADE_HH_
#define _SFTA_CUDD_FACADE_HH_

#include <vector>
#include <string>


// insert the class into proper namespace
namespace SFTA { namespace Private { class CUDDFacade; } }


/**
 * @brief   Facade for CUDD package
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * A class that serves as a facade to the CUDD package. It provides means to
 * work with CUDD with no CUDD declarations, which means that no CUDD header
 * need to be included in files using CUDDFacade. This facade provides access
 * to a single CUDD manager and handles its construction and destruction. It
 * also takes care not to let any CUDD-specific type spoil global namespace.
 */
class SFTA::Private::CUDDFacade
{
public:  // Public types

	/**
	 * @brief  MTBDD sink node type
	 *
	 * The type in sink nodes of the MTBDD. Declared here to be easily modifiable.
	 * Note that it needs to correspond to CUDD_VALUE_TYPE in CUDD.
	 */
	typedef unsigned ValueType;


	/**
	 * @brief  Type of MTBDD manager
	 *
	 * The type used for manipulation with MTBDD manager. Note that this type is
	 * a correct-namespace substitute for CUDD's DdManager and is always
	 * transformed to this inside the facade. This type is never defined!
	 */
	struct Manager;


	/**
	 * @brief  Type of MTBDD node
	 *
	 * The type used for manipulation with MTBDD node. Note that this type is
	 * a correct-namespace substitute for CUDD's DdNode and is always
	 * transformed to this inside the facade. This type is never defined!
	 */
	struct Node;


	/**
	 * @brief  The abstract class for an MTBDD Apply functor
	 *
	 * This class is an abstract class that defines the interface that all
	 * functors that perform binary Apply operation on CUDD MTBDD need to
	 * implement.
	 */
	class AbstractApplyFunctor
	{
	public:   // Public methods

		/**
		 * @brief  Constructor
		 *
		 * The constructor of the class
		 */
		AbstractApplyFunctor()
		{ }

		/**
		 * @brief  The operator
		 *
		 * The operator of the functor, that is the method that performs the
		 * operation.
		 *
		 * @param[in]  lhs  Left-hand operand of the operation
		 * @param[in]  rhs  Right-hand operand of the operation
		 *
		 * @returns  Result of the operation
		 */
		virtual ValueType operator()(const ValueType& lhs, const ValueType& rhs) = 0;

		/**
		 * @brief  Destructor
		 *
		 * Virtual destructor
		 */
		virtual ~AbstractApplyFunctor()
		{ }
	};


	/**
	 * @brief  The abstract class for an MTBDD monadic Apply functor
	 *
	 * This class is an abstract class that defines the interface that all
	 * functors that perform monadic Apply operation on CUDD MTBDD need to
	 * implement.
	 */
	class AbstractMonadicApplyFunctor
	{
	public:   // Public methods


		/**
		 * @brief  Constructor
		 *
		 * The constructor of the class
		 */
		AbstractMonadicApplyFunctor()
		{ }


		/**
		 * @brief  The operator
		 *
		 * The operator of the functor, that is the method that performs the
		 * operation.
		 *
		 * @param[in]  val  The operand of the operation
		 *
		 * @returns  Result of the operation
		 */
		virtual ValueType operator()(const ValueType& val) = 0;


		/**
		 * @brief  Destructor
		 *
		 * Virtual destructor
		 */
		virtual ~AbstractMonadicApplyFunctor()
		{ }
	};


public:  // Public data members

	/**
	 * @brief  Logging category name for Log4cpp
	 *
	 * Name of the logging category that this class uses for logging using
	 * Log4cpp.
	 */
	static const char* LOG_CATEGORY_NAME;


private: // Private data members

	/**
	 * @brief  MTBDD manager
	 *
	 * Manager that takes care of used MTBDD.
	 */
	Manager* manager_;


private: // Private methods

	/**
	 * Private copy constructor that disables copying.
	 *
	 * @param[in]  facade  CUDD facade to be copied
	 */
	CUDDFacade(const CUDDFacade& facade);


	/**
	 * Private assignment operator that disables assignment.
	 *
	 * @param[in]  rhs  Right-hand side of the assignment
	 *
	 * @returns  Copied instance
	 */
	CUDDFacade& operator=(const CUDDFacade& rhs);


public:  // Public methods

	/**
	 * @brief  Implicit constructor
	 *
	 * Implicit constructor of CUDDFacade. Initializes the CUDD manager.
	 */
	CUDDFacade();


	/**
	 * @brief  Adds a variable to MTBDD
	 *
	 * Adds a variable on the i-th index to the MTBDD.
	 *
	 * @see  AddConst()
	 *
	 * @param[in]  i  Index of the variable
	 *
	 * @returns  Node representing the variable
	 */
	Node* AddIthVar(int i) const;


	/**
	 * @brief  Adds the complement of a variable
	 *
	 * For variable \f$ x \f$ generates the complement of \f$ x \f$: \f$ \neg x \f$.
	 *
	 * @see  AddIthVar()
	 *
	 * @param[in]  node  Node of the variable to be complemented
	 *
	 * @returns  Complement of \p node
	 */
	Node* AddCmpl(Node* node) const;


	/**
	 * @brief  Adds a new constant
	 *
	 * This function creates a new sink node of the MTBDD of type ValueType and
	 * returns pointer to it.
	 *
	 * @see  AddIthVar()
	 *
	 * @param[in]  value  Value of the constant
	 *
	 * @returns  Pointer to the node that contains the value
	 */
	Node* AddConst(ValueType value) const;


	/**
	 * @brief  References a node
	 *
	 * Increments the reference counter of given node by one. The reference
	 * counter is used for garbage collection of nodes when the nodes are
	 * dereferenced by RecursiveDeref(Node* node).
	 *
	 * @see  RecursiveDeref()
	 *
	 * @param[in]  node  The node to be referenced
	 */
	void Ref(Node* node) const;


	/**
	 * @brief  Recursively dereferences a node
	 *
	 * Decrements the reference counter of given node previously referenced by
	 * Ref(Node* node) and recursively executes the same step on all other nodes
	 * pointed by the node.
	 *
	 * @see  Ref()
	 *
	 * @param[in]  node  The node to be recursively dereferenced
	 */
	void RecursiveDeref(Node* node) const;


	/**
	 * @brief  Sets the background value of MTBDD
	 *
	 * Given a node, sets the background value of the MTBDD (i.e. the default
	 * value for all assignments that are not explicitly given) to this node.
	 * The value can be later read by ReadBackground().
	 *
	 * @see  ReadBackground()
	 *
	 * @param[in]  bck  The background value
	 */
	void SetBackground(Node* bck) const;


	/**
	 * @brief  Reads the background value of MTBDD
	 *
	 * Reads the node with the background value of MTBDD set previously by
	 * SetBackground(Node* bck).
	 *
	 * @see  SetBackground()
	 *
	 * @returns  The background value of MTBDD
	 */
	Node* ReadBackground() const;


	/**
	 * @brief  Gets the count of variables of MTBDD
	 *
	 * Returns the number of variables of MTBDD.
	 *
	 * @returns  The number of variables of MTBDD
	 */
	unsigned GetVarCount() const;


	/**
	 * @brief  Gets the count of nodes of MTBDD of given node
	 *
	 * Returns the number of nodes of MTBDD that are rooted at given node
	 *
	 * @param[in]  node  The root of the MTBDD
	 *
	 * @returns  The number of nodes of MTBDD
	 */
	unsigned GetDagSize(Node* node) const;


	/**
	 * @brief  Multiplication of two nodes
	 *
	 * Performs either:
	 *   @li  logical conjunction of two Boolean expressions, or
	 *   @li  multiplication of a value of ValueType by a Boolean expression
	 *
	 * depending on the types of nodes passed to the method. Note that two nodes
	 * of ValueType type should never be passed.
	 *
	 * @param[in]  lhs  Left-hand side of multiplication
	 * @param[in]  rhs  Right-hand side of multiplication
	 *
	 * @returns  The node with the product
	 */
	Node* Times(Node* lhs, Node* rhs) const;


	/**
	 * @brief  Apply operation
	 *
	 * Performs the Apply operation passed as a callback function in @c cbParams
	 * on two MTBDDs.
	 *
	 * @see  ApplyCallbackParameters
	 * @see  MonadicApply()
	 *
	 * @param[in]  lhs   Left-hand side MTBDD of Apply operation
	 * @param[in]  rhs   Right-hand side MTBDD of Apply operation 
	 * @param[in]  func  Functor with Apply operation
	 *
	 * @returns  The resulting MTBDD
	 */
	Node* Apply(Node* lhs, Node* rhs, AbstractApplyFunctor* func) const;


	/**
	 * @brief  Monadic Apply operation
	 *
	 * Performs the monadic Apply operation passed as a callback function in @c
	 * cbParams on two MTBDDs.
	 *
	 * @see  ApplyCallbackParameters
	 * @see  Apply()
	 *
	 * @param[in]  root  Root of the MTBDD of monadic Apply operation
	 * @param[in]  func  Functor with monadic Apply operation
	 *
	 * @returns  The resulting MTBDD
	 */
	Node* MonadicApply(Node* root, AbstractMonadicApplyFunctor* func) const;


	/**
	 * @brief  Gets node's "then" child
	 *
	 * Returns the "then" child (i.e. the child for the 1 arc) of a node. Note
	 * that constant nodes cannot be passed.
	 *
	 * @param[in]  node  The node the child of which is to be got (cannot be
	 *                   constant)
	 *
	 * @returns  The "then" child of the node
	 */
	Node* GetThenChild(Node* node) const;


	/**
	 * @brief  Gets node's "else" child
	 *
	 * Returns the "else" child (i.e. the child for the 0 arc) of a node. Note
	 * that constant nodes cannot be passed.
	 *
	 * @param[in]  node  The node the child of which is to be got (cannot be
	 *                   constant)
	 *
	 * @returns  The "else" child of the node
	 */
	Node* GetElseChild(Node* node) const;


	/**
	 * @brief  Returns whether the node is constant
	 *
	 * This method returns a Boolean value determining whether the input node is
	 * constant.
	 *
	 * @param[in]  node  The input node
	 *
	 * @returns  Is the node constant?
	 */
	bool IsNodeConstant(Node* node) const;


	/**
	 * @brief  Gets the variable index of the node
	 *
	 * Returns the index of the Boolean variable that is represented by the
	 * node. Note that constant node cannot be passed to the method.
	 *
	 * @param[in]  node  The input node
	 *
	 * @returns  The index of the Boolean variable represented by the node.
	 */
	unsigned GetNodeIndex(Node* node) const;


	/**
	 * @brief  Changes the index of a variable
	 *
	 * Changes the index of all nodes that represent the Boolean variable with
	 * given index in an MTBDD rooted at given node. Note that the MTBDD needs
	 * to be correctly formed, the behaviour is undefined otherwise.
	 *
	 * @param[in]  root      The root of the MTBDD
	 * @param[in]  oldIndex  The old index of the variable
	 * @param[in]  newIndex  The new index of the variable
	 *
	 * @returns  MTBDD with reindexed variables
	 */
	Node* ChangeVariableIndex(Node* root, unsigned oldIndex,
		unsigned newIndex) const;


	/**
	 * @brief  Gets the value of a node
	 *
	 * Returns the value of a constant node. Note that the input node really
	 * needs to be constant.
	 *
	 * @param[in]  node  The input node
	 *
	 * @returns  The value of the node
	 */
	ValueType GetNodeValue(Node* node) const;


	/**
	 * @brief  Stores the MTBDDs in an internal format into a string
	 *
	 * Stores all given MTBDDs into a std::string using an internal format.
	 *
	 * @param[in]  nodes      List of nodes to be stored
	 * @param[in]  rootNames  Names of roots of the MTBDD
	 *
	 * @returns  String with the internal representation of given MTBDDs
	 *
	 * @todo TODO: a function that performs inverse operation is missing: JUST
	 * DO IT!
	 */
	std::string StoreToString(const std::vector<Node*>& nodes,
		const std::vector<std::string>& rootNames) const;


	/**
	 * @brief  Dumps the MTBDD in Dot format
	 *
	 * Creates a Dot (http://www.graphviz.org) file with a representation of
	 * the MTBDD.
	 *
	 * @param[in]  nodes      Array of root nodes to be output to the file
	 * @param[in]  rootNames  Array of names of root nodes
	 * @param[in]  sinkNames  Array of names of sink nodes; these needs to be in
	 *                        such an order that if the value stored in a sink
	 *                        node is @f$i@f$, then the name of the node is at
	 *                        the @f$i@f$-th element of the array
	 * @param[in]  filename   The name of the output file
	 */
	void DumpDot(const std::vector<Node*>& nodes,
		const std::vector<std::string>& rootNames,
		const std::vector<std::string>& sinkNames,
		const std::string& filename) const;


	/**
	 * @brief  The destructor
	 *
	 * Destructor of CUDDFacade. Releases any resources used by the CUDD
	 * manager.
	 */
	~CUDDFacade();
};

#endif
