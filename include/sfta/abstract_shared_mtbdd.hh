/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for AbstractSharedMTBDD.
 *
 *****************************************************************************/

#ifndef _ABSTRACT_SHARED_MTBDD_HH_
#define _ABSTRACT_SHARED_MTBDD_HH_


// Standard library headers
#include <string>
using std::string;


namespace SFTA
{
	template <typename RootType,
		class VariableAssignmentType,
		class LeafAllocator>
	class AbstractSharedMTBDD;
}


/**
 * @brief   Abstract class for shared multi-terminal BDD
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Abstract class that defines interface for classes implementing shared
 * multi-terminal BDD (MTBDD).
 *
 * @tparam  RootType                The type for the root of a MTBDD. Is used
 *                                  to reference the root.
 * @tparam  VariableAssignmentType  The type that is used for representation
 *                                  of Boolean variable assignment, i.e.
 *                                  representation of a path in the BDD.
 * @tparam  LeafAllocator           The allocator that is used to allocate
 *                                  memory for leafs and (in case it is
 *                                  necessary) provide binding between the
 *                                  leafs and handles stored in the MTBDD. The
 *                                  allocator should behave in such a way,
 *                                  that for leafs that are equal should
 *                                  generate handles that are also equal.
 */
template <typename RootType,
	class VariableAssignmentType,
 	class LeafAllocator>
class SFTA::AbstractSharedMTBDD
{
public:  // Public data types

	/**
	 * @brief  Type of Apply operation function pointers
	 *
	 * Data type for function pointers for functions that carry out the Apply
	 * operation on the MTBDD.
	 */
	typedef typename LeafAllocator::LeafType (*ApplyFunctionType)(
		typename LeafAllocator::LeafType, typename LeafAllocator::LeafType);


public:  // Public methods

	/**
	 * @brief  Sets the value of a leaf
	 *
	 * This function sets the value of a leaf at a position determined by the root
	 * of the MTBDD and assignments to boolean variables.
	 *
	 * @see  GetValue()
	 *
	 * @param[in]  root      The root of the MTBDD in which the method works
	 * @param[in]  position  The position of the leaf given by the assignment to
	 *                       Boolean variables of the MTBDD
	 * @param[in]  value     The value of the leaf to be set
	 */
	virtual void SetValue(const RootType& root, const VariableAssignmentType& position, const typename LeafAllocator::LeafType& value) = 0;


	/**
	 * @brief  Gets the reference to the leaf
	 *
	 * <TODO:  @todo  Verbose description>
	 *
	 * @see  SetValue()
	 *
	 * @param[in]  root      The root of the MTBDD in which the method works
	 * @param[in]  position  The position of the leaf given by the assignment to
	 *                       Boolean variables of the MTBDD
	 *
	 * @returns  Reference to the leaf at given position of given MTBDD
	 */
	virtual typename LeafAllocator::LeafType& GetValue(const RootType& root, const VariableAssignmentType& position) = 0;


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
	virtual RootType Apply(const RootType& lhs, const RootType& rhs, const ApplyFunctionType& func) = 0;


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
	 * @brief  Serializes the MTBDD into a string
	 *
	 * This function serializes the MTBDD into a string
	 *
	 * @returns  String that represents the data contained in the MTBDD
	 */
	virtual string Serialize() const = 0;


	/**
	 * @brief  Destructor
	 *
	 * The destructor of the MTBDD. Releases all allocated resources.
	 */
	virtual ~AbstractSharedMTBDD() { }
};

#endif
