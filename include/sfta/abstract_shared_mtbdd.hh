/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for AbstractSharedMTBDD class.
 *
 *****************************************************************************/

#ifndef _SFTA_ABSTRACT_SHARED_MTBDD_HH_
#define _SFTA_ABSTRACT_SHARED_MTBDD_HH_


// Standard library headers
#include <string>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename Root,
		typename Leaf,
		class VariableAssignment
	>
	class AbstractSharedMTBDD;
}


/**
 * @brief   Abstract class for shared multi-terminal BDD
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Abstract class that defines a high-level interface for classes implementing
 * shared multi-terminal BDD (MTBDD).
 *
 * @tparam  Root                The type of the root of a MTBDD. Is used to
 *                              reference the root.
 * @tparam  Leaf                The type of a leaf of a MTBDD.
 * @tparam  VariableAssignment  The type that is used for representation of
 *                              Boolean variable assignment, i.e.
 *                              representation of a path in the BDD.
 */
template
<
	typename Root,
	typename Leaf,
	class VariableAssignment
>
class SFTA::AbstractSharedMTBDD
{
public:  // Public data types


	/**
	 * @brief  Type of a leaf
	 *
	 * Type of a leaf of MTBDD.
	 */
	typedef Leaf LeafType;


	/**
	 * @brief  Type of a root
	 *
	 * Type of a root of MTBDD.
	 */
	typedef Root RootType;


	/**
	 * @brief  Type of variable assignment
	 *
	 * Type of variable assignment.
	 */
	typedef VariableAssignment VariableAssignmentType;


	/**
	 * @brief  The container type for leaves
	 *
	 * The type that serves as a container of several leaves. This type is used
	 * for example by the GetValue() method.
	 *
	 * @see  GetValue()
	 */
	typedef std::vector<LeafType*> LeafContainer;


	/**
	 * @brief  The base class for functors that perform @c Apply operations
	 *
	 * Abstract class that defines the interface for functors that carry out
	 * @c Apply operations.
	 */
	class AbstractApplyFunctorType
	{
	public:   // Public methods


		/**
		 * @brief  The operation of the functor
		 *
		 * Abstract method that performs the operation of the functor
		 *
		 * @param[in]  lhs  Left-hand side of the operation
		 * @param[in]  rhs  Right-hand side of the operation
		 *
		 * @returns  Result of the operation
		 */
		virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs) = 0;


		/**
		 * @brief  Destructor
		 *
		 * The destructor
		 */
		virtual ~AbstractApplyFunctorType()
		{ }

	};


	/**
	 * @brief  The base class for functors that perform monadic @c Apply
	 *         operations
	 *
	 * Abstract class that defines the interface for functors that carry out
	 * monadic @c Apply operations.
	 */
	class AbstractMonadicApplyFunctorType
	{
	public:   // Public methods


		/**
		 * @brief  The operation of the functor
		 *
		 * Abstract method that performs the operation of the functor
		 *
		 * @param[in]  val  Operand of the operation
		 *
		 * @returns  Result of the operation
		 */
		virtual LeafType operator()(const LeafType& val) = 0;


		/**
		 * @brief  Destructor
		 *
		 * The destructor
		 */
		virtual ~AbstractMonadicApplyFunctorType()
		{ }

	};


public:  // Public methods

	/**
	 * @brief  Sets the value of a leaf
	 *
	 * This function sets the value of a leaf at a position determined by the root
	 * of the MTBDD and assignments to Boolean variables.
	 *
	 * @see  GetValue()
	 *
	 * @param[in]  root   The root of the MTBDD in which the method works
	 * @param[in]  asgn   The position of the leaf given by the assignment to
	 *                    Boolean variables of the MTBDD
	 * @param[in]  value  The value of the leaf to be set
	 */
	virtual void SetValue(const RootType& root,
		const VariableAssignmentType& asgn, const LeafType& value) = 0;


	/**
	 * @brief  Gets references to leaves
	 *
	 * This function returns a container with all leaves that are reachable by
	 * given variable assignment. Note that the references in the container may
	 * be made invalid by following operations on the MTBDD.
	 *
	 * @see  LeafContainer
	 * @see  SetValue()
	 *
	 * @param[in]  root   The root of the MTBDD in which the method works
	 * @param[in]  asgn   The position of the leaf given by the assignment to
	 *                    Boolean variables of the MTBDD
	 *
	 * @returns  Container of references to leaves at given position of the MTBDD
	 */
	virtual LeafContainer GetValue(const RootType& root,
		const VariableAssignmentType& asgn) = 0;


	/**
	 * @brief  Apply function for two MTBDDs
	 *
	 * Performs given Apply operation on two MTBDD specified by their roots.
	 *
	 * @see  AbstractSharedMTBDD::AbstractApplyFunctorType
	 *
	 * @param[in]  lhs   Left-hand side MTBDD
	 * @param[in]  rhs   Right-hand side MTBDD
	 * @param[in]  func  The operation to be performed on respective leaves of
	 *                   given MTBDDs
	 *
	 * @returns  Root of the MTBDD with the result of the operation
	 */
	virtual RootType Apply(const RootType& lhs, const RootType& rhs,
		AbstractApplyFunctorType* func) = 0;


	/**
	 * @brief  Monadic Apply function for an MTBDD
	 *
	 * Performs given monadic Apply operation on an MTBDD specified by its root.
	 *
	 * @see  AbstractSharedMTBDD::AbstractMonadicApplyFunctorType
	 *
	 * @param[in]  root  Root of the MTBDD
	 * @param[in]  func  The operation to be performed on all leaves of
	 *                   given MTBDD
	 *
	 * @returns  Root of the MTBDD with the result of the operation
	 */
	virtual RootType MonadicApply(const RootType& root,
		AbstractMonadicApplyFunctorType* func) = 0;


	/**
	 * @brief  Creates a new root of a MTBDD
	 *
	 * Creates a new MTBDD represented by its root. All paths in the new MTBDD
	 * lead to the background value.
	 *
	 * @returns  A root of a new MTBDD
	 */
	virtual RootType CreateRoot() = 0;


	/**
	 * @brief  Sets the value of the bottom
	 *
	 * This method sets a new value of the leaf for the bottom of the MTBDD.
	 *
	 * @param[in]  bottom  The new value for the bottom of the MTBDD
	 */
	virtual void SetBottomValue(const LeafType& bottom) = 0;


	/**
	 * @brief  Serializes the MTBDD into a string
	 *
	 * This function serializes the MTBDD into a string
	 *
	 * @returns  String that represents the data contained in the MTBDD
	 */
	virtual std::string Serialize() const = 0;


	/**
	 * @brief  Dumps the MTBDD into a Dot file
	 *
	 * Dumps MTBDD into a file in Dot format. This file can then be used to
	 * generate a diagram using GraphViz (http://www.graphviz.org/) Dot tool.
	 *
	 * @param[in]  filename  Name of the output file (preferably with .dot
	 *                       extension)
	 */
	virtual void DumpToDotFile(const std::string& filename) const = 0;


	/**
	 * @brief  Destructor
	 *
	 * The destructor of the MTBDD. Releases all allocated resources.
	 */
	virtual ~AbstractSharedMTBDD() { }
};

#endif
