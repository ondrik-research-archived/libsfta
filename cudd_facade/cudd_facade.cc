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


const char* CUDDFacade::LOG_CATEGORY_NAME = "cudd_facade";


CUDDFacade::CUDDFacade() : manager_(static_cast<DdManager*>(0))
{
	// Create the manager
	if ((manager_ = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0)) == static_cast<DdManager*>(0))
	{
		throw std::runtime_error("CUDD Manager could not be created");
	}
}

CUDDFacade::~CUDDFacade()
{
	// Assertions
	assert(manager_ != static_cast<DdManager*>(0));

	// Check for nodes with non-zero reference count
	int unrefed = 0;
	if ((unrefed = Cudd_CheckZeroRef(manager_)) != 0)
	{	// in case there are still some nodes unreferenced
		SFTA_LOGGER_WARN("Still " + Convert::ToString(unrefed) + " nodes unreferenced!");
	}

	// Delete the manager
	Cudd_Quit(manager_);
	manager_ = static_cast<DdManager*>(0);
}
