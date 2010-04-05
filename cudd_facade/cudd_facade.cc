/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Implementation file of CUDDFacade. This file uses directly CUDD
 *    functions and provides their actions through the interface of
 *    CUDDFacade.
 *
 *****************************************************************************/


// Standard library headers
#include <stdexcept>

// SFTA headers
#include <sfta/cudd_facade.hh>
#include <sfta/sfta.hh>
#include <sfta/convert.hh>

// CUDD headers
#include <util.h>
#include <cudd.h>
#include <cuddInt.h>

using namespace SFTA::Private;


// Setting the logging category name for Log4cpp
const char* CUDDFacade::LOG_CATEGORY_NAME = "cudd_facade";


// A set of functions that converts between SFTA and CUDD manager and node
// types. 
namespace SFTA
{
	namespace Private
	{
		/**
		 * @brief  Converts manager from CUDD format
		 *
		 * Converts a pointer to manager from DdManager type to
		 * CUDDFacade::Manager type.
		 *
		 * @param[in]  cudd_value  CUDD DdManager pointer
		 *
		 * @returns  SFTA Manager pointer
		 */
		CUDDFacade::Manager* fromCUDD(DdManager* cudd_value)
		{
			return reinterpret_cast<CUDDFacade::Manager*>(cudd_value);
		}


		/**
		 * @brief  Converts node from CUDD format
		 *
		 * Converts a pointer to manager from DdNode type to
		 * CUDDFacade::Node type.
		 *
		 * @param[in]  cudd_value  CUDD DdNode pointer
		 *
		 * @returns  SFTA Node pointer
		 */
		CUDDFacade::Node* fromCUDD(DdNode* cudd_value)
		{
			return reinterpret_cast<CUDDFacade::Node*>(cudd_value);
		}


		/**
		 * @brief  Converts manager to CUDD format
		 *
		 * Converts a pointer to manager from CUDDFacade::Manager type to
		 * DdManager type.
		 *
		 * @param[in]  sfta_value  SFTA CUDDFacade::Manager pointer
		 *
		 * @returns  CUDD DdManager pointer
		 */
		DdManager* toCUDD(CUDDFacade::Manager* sfta_value)
		{
			return reinterpret_cast<DdManager*>(sfta_value);
		}


		/**
		 * @brief  Converts node to CUDD format
		 *
		 * Converts a pointer to manager from CUDDFacade::Node type to
		 * DdNode type.
		 *
		 * @param[in]  sfta_value  SFTA CUDDFacade::Node pointer
		 *
		 * @returns  CUDD DdNode pointer
		 */
		DdNode* toCUDD(CUDDFacade::Node* sfta_value)
		{
			return reinterpret_cast<DdNode*>(sfta_value);
		}
	}
}


CUDDFacade::CUDDFacade()
	: manager_(static_cast<Manager*>(0))
{
	// Create the manager
	if ((manager_ = fromCUDD(Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0)))
		== static_cast<Manager*>(0))
	{	// in case the manager could not be created
		std::string error_msg = "CUDD Manager could not be created";
		SFTA_LOGGER_FATAL(error_msg);
		throw std::runtime_error(error_msg);
	}
}


CUDDFacade::Node* CUDDFacade::AddIthVar(int i) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(i >= 0);

	Node* res = fromCUDD(Cudd_addIthVar(toCUDD(manager_), i));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


SFTA::Private::CUDDFacade::Node* CUDDFacade::AddCmpl(Node* node) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(node != static_cast<Node*>(0));

	Node* res = fromCUDD(Cudd_addCmpl(toCUDD(manager_), toCUDD(node)));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


CUDDFacade::Node* CUDDFacade::AddConst(CUDDFacade::ValueType value) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));

	Node* res = fromCUDD(Cudd_addConst(toCUDD(manager_), value));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


void CUDDFacade::Ref(Node* node) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(node != static_cast<Node*>(0));

	Cudd_Ref(toCUDD(node));
}


void CUDDFacade::RecursiveDeref(Node* node) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(node != static_cast<Node*>(0));

	Cudd_RecursiveDeref(toCUDD(manager_), toCUDD(node));
}


void CUDDFacade::SetBackground(Node* bck) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(bck != static_cast<Node*>(0));

	Cudd_SetBackground(toCUDD(manager_), toCUDD(bck));
}


CUDDFacade::Node* CUDDFacade::ReadBackground() const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));

	Node* res = fromCUDD(Cudd_ReadBackground(toCUDD(manager_)));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


CUDDFacade::Node* CUDDFacade::Times(Node* lhs, Node* rhs) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(!(cuddIsConstant(toCUDD(lhs)) && cuddIsConstant(toCUDD(rhs))));

	Node* res = fromCUDD(Cudd_addApply(toCUDD(manager_), Cudd_addTimes,
		toCUDD(lhs), toCUDD(rhs)));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


DdNode* applyCallback(DdManager* dd, DdNode** f, DdNode** g, void* data)
{
	// Assertions
	assert(dd   != static_cast<DdManager*>(0));
	assert(f    != static_cast<DdNode**>(0));
	assert(g    != static_cast<DdNode**>(0));
	assert(data != static_cast<void*>(0));

	// get values of the nodes
	DdNode* F = *f;
	DdNode* G = *g;

	// Further assertions
	assert(F    != static_cast<DdNode*>(0));
	assert(G    != static_cast<DdNode*>(0));

	// get callback parameters from the data container
	CUDDFacade::ApplyCallbackParameters& params =
		*(static_cast<CUDDFacade::ApplyCallbackParameters*>(data));

	// Even further assertions
	assert(params.Op != static_cast<CUDDFacade::ApplyOperationType>(0));

	if (cuddIsConstant(F) && cuddIsConstant(G))
	{	// in case we are at leaves
		DdNode* res = cuddUniqueConst(dd,
			params.Op(cuddV(F), cuddV(G), params.Data));

		assert(res != static_cast<DdNode*>(0));

		return res;
	}
	else
	{	// in case we are not at leaves
		return static_cast<DdNode*>(0);
	}
}


CUDDFacade::Node* CUDDFacade::Apply(Node* lhs, Node* rhs,
	ApplyCallbackParameters* cbParams) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(lhs != static_cast<Node*>(0));
	assert(rhs != static_cast<Node*>(0));
	assert(cbParams != static_cast<ApplyCallbackParameters*>(0));

	Node* res = fromCUDD(Cudd_addApplyWithData(
		toCUDD(manager_), applyCallback, toCUDD(lhs), toCUDD(rhs), cbParams));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


CUDDFacade::~CUDDFacade()
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));

	// Derefence the background
	RecursiveDeref(ReadBackground());

	// Check for nodes with non-zero reference count
	int unrefed = 0;
	if ((unrefed = Cudd_CheckZeroRef(toCUDD(manager_))) != 0)
	{	// in case there are still some nodes unreferenced
		SFTA_LOGGER_WARN("Still " + Convert::ToString(unrefed) + " nodes unreferenced!");
	}

	// Delete the manager
	Cudd_Quit(toCUDD(manager_));
	manager_ = static_cast<Manager*>(0);
}



