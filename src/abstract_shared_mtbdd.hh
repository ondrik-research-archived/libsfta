






namespace Knihovna
{
	class AbstractSharedMTBDD;
}





template <typename RootType, class VariableAssignment, class LeafAllocator>
class Knihovna::AbstractSharedMTBDD
{
public:

	void SetValue(const RootType& root, const VariableAssignment& position, const LeafAllocator::LeafType& value) = 0;
	LeafAllocator::LeafType& GetValue(const RootType& root, const VariableAssignment& position) = 0;
	RootType Apply(const RootType& lhs, const RootType& rhs, ApplyFunction func) = 0;
	RootType CreateRoot() = 0;
	std::string Serialize() = 0;
};
