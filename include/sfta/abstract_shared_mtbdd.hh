




#ifndef _ABSTRACT_SHARED_MTBDD_HH_
#define _ABSTRACT_SHARED_MTBDD_HH_

// Standard library headers
#include <string>


namespace SFTA
{
	template <typename RootType, class VariableAssignmentType, class LeafAllocator, class LeafType>
	class AbstractSharedMTBDD;
}


template <typename RootType, class VariableAssignmentType, class LeafAllocator, class LeafType>
class SFTA::AbstractSharedMTBDD
{
public:

	typedef LeafType (*ApplyFunctionType)(LeafType, LeafType);

	virtual void SetValue(const RootType& root, const VariableAssignmentType& position, const LeafType& value) = 0;
	virtual LeafType& GetValue(const RootType& root, const VariableAssignmentType& position) = 0;
	virtual RootType Apply(const RootType& lhs, const RootType& rhs, const ApplyFunctionType& func) = 0;
	virtual RootType CreateRoot() = 0;
	//virtual std::string Serialize() const = 0;
	virtual ~AbstractSharedMTBDD() { }
};

#endif
