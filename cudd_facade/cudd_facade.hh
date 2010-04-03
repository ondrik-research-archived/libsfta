
namespace SFTA
{
	namespace Private
	{
		class CUDDFacade;
	}
}

// Declaration of structures from CUDD... We don't want to include CUDD header
// files into a header file, because they are horrible.
struct DdManager;
struct DdNode;

class SFTA::Private::CUDDFacade
{
private:	// Private data members

	DdManager* manager_;


public:	// Public data members

	static const char* LOG_CATEGORY_NAME;

	typedef unsigned ValueType;

private:

	CUDDFacade(const CUDDFacade& facade);
	CUDDFacade& operator=(const CUDDFacade& lhs);

public:
	CUDDFacade();

	DdNode* AddIthVar(int i);
	DdNode* AddCmpl(DdNode* node);
	DdNode* AddConst(ValueType value);
	void Ref(DdNode* node);
	DdNode* ReadBackground();
	void SetBackground(DdNode* bck);
	void RecursiveDeref(DdNode* node);
	DdNode* Times(DdNode* lsh, DdNode* rhs);


	~CUDDFacade();

};
