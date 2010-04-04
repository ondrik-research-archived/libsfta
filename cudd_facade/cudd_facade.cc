/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 ****************************************************************************/


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


CUDDFacade::Node* CUDDFacade::AddIthVar(int i)
{
	// Assertions
	assert(i >= 0);

	return fromCUDD(Cudd_addIthVar(toCUDD(manager_), i));
}


SFTA::Private::CUDDFacade::Node* CUDDFacade::AddCmpl(Node* node)
{
	// Assertions
	assert(node != static_cast<Node*>(0));

	return fromCUDD(Cudd_addCmpl(toCUDD(manager_), toCUDD(node)));
}


CUDDFacade::Node* CUDDFacade::AddConst(CUDDFacade::ValueType value)
{
	return fromCUDD(Cudd_addConst(toCUDD(manager_), value));
}


void CUDDFacade::Ref(Node* node)
{
	// Assertions
	assert(node != static_cast<Node*>(0));

	Cudd_Ref(toCUDD(node));
}


void CUDDFacade::RecursiveDeref(Node* node)
{
	// Assertions
	assert(node != static_cast<Node*>(0));

	Cudd_RecursiveDeref(toCUDD(manager_), toCUDD(node));
}


void CUDDFacade::SetBackground(Node* bck)
{
	// Assertions
	assert(bck != static_cast<Node*>(0));

	Cudd_SetBackground(toCUDD(manager_), toCUDD(bck));
}


CUDDFacade::Node* CUDDFacade::ReadBackground()
{
	return fromCUDD(Cudd_ReadBackground(toCUDD(manager_)));
}


CUDDFacade::Node* CUDDFacade::Times(Node* lhs, Node* rhs)
{
	// Assertions
	assert(!(cuddIsConstant(toCUDD(lhs)) && cuddIsConstant(toCUDD(rhs))));

	return fromCUDD(Cudd_addApply(toCUDD(manager_), Cudd_addTimes,
		toCUDD(lhs), toCUDD(rhs)));
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
