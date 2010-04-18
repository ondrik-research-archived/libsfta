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
		typename RootType,
		typename LeafType,
		class VariableAssignmentType
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
 * @tparam  RootType                The type of the root of a MTBDD. Is used
 *                                  to reference the root.
 * @tparam  LeafType                The type of a leaf of a MTBDD.
 * @tparam  VariableAssignmentType  The type that is used for representation
 *                                  of Boolean variable assignment, i.e.
 *                                  representation of a path in the BDD.
 */
template
<
	typename RootType,
	typename LeafType,
	class VariableAssignmentType
>
class SFTA::AbstractSharedMTBDD
{
public:  // Public data types

	/**
	 * @brief  Type of Apply operation function pointers
	 *
	 * Data type for function pointers for functions that carry out the Apply
	 * operation on the MTBDD.
	 */
	typedef LeafType (*ApplyFunctionType)(const LeafType&, const LeafType&);


	/**
	 * @brief  The container type for leafs
	 *
	 * The type that serves as a container of several leafs. This type is used
	 * for example byt the GetValue() method.
	 *
	 * @see  GetValue()
	 */
	typedef std::vector<LeafType*> LeafContainer;


public:  // Public methods

	/**
	 * @brief  Sets the value of a leaf
	 *
	 * This function sets the value of a leaf at a position determined by the root
	 * of the MTBDD and assignments to boolean variables.
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
	 * given variable assignment.
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
	 * @see  AbstractSharedMTBDD::ApplyFunctionType
	 *
	 * @param[in]  lhs   Left-hand side MTBDD
	 * @param[in]  rhs   Right-hand side MTBDD
	 * @param[in]  func  The operation to be performed on respective leaves of
	 *                   given MTBDDs
	 *
	 * @returns  Root of a MTBDD with the result of the operation
	 */
	virtual RootType Apply(const RootType& lhs, const RootType& rhs,
		const ApplyFunctionType& func) = 0;


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
	 * @brief  Returns root value for the bottom
	 *
	 * This method returns the value of the root that points to bottom
	 *
	 * @returns  Value of the root for bottom
	 */
	virtual RootType GetRootForBottom() const = 0;


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
	 *                       extension
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
