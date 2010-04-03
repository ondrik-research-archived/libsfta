// Standard library headers
#include <stdexcept>

// Log4cpp headers
#include <log4cpp/Category.hh>

// CUDD headers
#include <util.h>
#include <cudd.h>
#include <cuddInt.h>

// Knihovna headers
#include <knihovna/cudd_facade.hh>

Knihovna::Private::CUDDFacade::CUDDFacade() : manager_(static_cast<DdManager*>(0))
{
	// Create the manager
	if ((manager_ = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0)) == static_cast<DdManager*>(0))
	{
		throw std::runtime_error("CUDD Manager could not be created");
	}
}

Knihovna::Private::CUDDFacade::~CUDDFacade()
{
	// Assertions
	assert(manager_ != static_cast<DdManager*>(0));

	// Check for nodes with non-zero reference count
	int unreffed = 0;
	if ((unreffed = Cudd_CheckZeroRef(manager_)) != 0)
	{
		log4cpp::Category::getInstance("pokus").warn(__FILE__ ":" "CUDDFacade: Ahoj" _FILE__);
		//std::cout << "Still " << unreffed << " unreffed!" << std::endl;
		// TODO: do something
	}

	// Delete the manager
	Cudd_Quit(manager_);
	manager_ = static_cast<DdManager*>(0);
}
