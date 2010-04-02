
#ifndef _CUDD_SHARED_MTBDD_HH_
#define _CUDD_SHARED_MTBDD_HH_

#include <knihovna/abstract_shared_mtbdd.hh>

namespace Knihovna
{
	template <typename RootType, class VariableAssignmentType, class LeafAllocator, class LeafAllocatorLeafType>
	class CUDDSharedMTBDD;
}

template <typename RootType, class VariableAssignmentType, class LeafAllocator, class LeafAllocatorLeafType>
class Knihovna::CUDDSharedMTBDD : public AbstractSharedMTBDD<RootType, VariableAssignmentType, LeafAllocator, LeafAllocatorLeafType>
{
public:

	typedef AbstractSharedMTBDD<RootType, VariableAssignmentType, LeafAllocator, LeafAllocatorLeafType> ParentClass;

	CUDDSharedMTBDD()
		: smeti_(0)
	{
	}

	virtual void SetValue(const RootType& /*root*/, const VariableAssignmentType& /*position*/, const LeafAllocatorLeafType& /*value*/)
	{
	}

	virtual LeafAllocatorLeafType& GetValue(const RootType& /*root*/, const VariableAssignmentType& /*position*/)
	{
		return reinterpret_cast<LeafAllocatorLeafType&>(smeti_);
	}

	virtual RootType Apply(const RootType& /*lhs*/, const RootType& /*rhs*/, const typename ParentClass::ApplyFunctionType& /*func*/)
	{
		return RootType();
	}

	virtual RootType CreateRoot()
	{
		return RootType();
	}

	virtual std::string Serialize() const
	{
		return "";
	}

	virtual ~CUDDSharedMTBDD()
	{
	}

private:
	int smeti_;
};


#endif
