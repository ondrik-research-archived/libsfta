




#ifndef _ABSTRACT_SHARED_MTBDD_HH_
#define _ABSTRACT_SHARED_MTBDD_HH_

#include <string>


namespace Knihovna
{
	template <typename RootType, class VariableAssignmentType, class LeafAllocator, class LeafAllocatorLeafType>
	class AbstractSharedMTBDD;
}


template <typename RootType, class VariableAssignmentType, class LeafAllocator, class LeafAllocatorLeafType>
class Knihovna::AbstractSharedMTBDD
{
public:

	typedef LeafAllocatorLeafType (*ApplyFunctionType)(LeafAllocatorLeafType, LeafAllocatorLeafType);

	virtual void SetValue(const RootType& root, const VariableAssignmentType& position, const LeafAllocatorLeafType& value) = 0;
	virtual LeafAllocatorLeafType& GetValue(const RootType& root, const VariableAssignmentType& position) = 0;
	virtual RootType Apply(const RootType& lhs, const RootType& rhs, const ApplyFunctionType& func) = 0;
	virtual RootType CreateRoot() = 0;
	virtual std::string Serialize() const = 0;
	virtual ~AbstractSharedMTBDD() { }
};

#endif
