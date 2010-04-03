
#ifndef _CUDD_SHARED_MTBDD_HH_
#define _CUDD_SHARED_MTBDD_HH_

// Standard library headers
#include <cassert>

// SFTA headers
#include <sfta/abstract_shared_mtbdd.hh>

namespace SFTA
{
	template <typename RootType, class VariableAssignmentType, class LeafAllocator, class LeafType>
	class CUDDSharedMTBDD;
}

template <typename RootType, class VariableAssignmentType, class LeafAllocator, class LeafType>
class SFTA::CUDDSharedMTBDD : public AbstractSharedMTBDD<RootType, VariableAssignmentType, LeafAllocator, LeafType>
{
public:

	typedef AbstractSharedMTBDD<RootType, VariableAssignmentType, LeafAllocator, LeafType> ParentClass;

	CUDDSharedMTBDD() : smeti_(0)
	{
		//assert(false);

//		DdManager* manager = static_cast<DdManager*>(0);
//		if ((manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0)) != static_cast<DdManager*>(0))
//		{
//			// TODO: kill
//		}
	}

	virtual void SetValue(const RootType& /*root*/, const VariableAssignmentType& /*position*/, const LeafType& /*value*/)
	{
	}

	virtual LeafType& GetValue(const RootType& /*root*/, const VariableAssignmentType& /*position*/)
	{
		return reinterpret_cast<LeafType&>(smeti_);
	}

	virtual RootType Apply(const RootType& /*lhs*/, const RootType& /*rhs*/, const typename ParentClass::ApplyFunctionType& /*func*/)
	{
		return RootType();
	}

	virtual RootType CreateRoot()
	{
		return RootType();
	}

	//virtual std::string Serialize() const
	//{
	//	return "";
	//}

	virtual ~CUDDSharedMTBDD()
	{
	}

private:

	int smeti_;
};

#endif
