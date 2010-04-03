
namespace Knihovna
{
	namespace Private
	{
		class CUDDFacade;
	}
}

// Declaration of structures from CUDD... We don't want to include CUDD header
// files into a header file, because they are horrible.
struct DdManager;

class Knihovna::Private::CUDDFacade
{
private:	// Private data members

	DdManager* manager_;


public:	// Public data members

	static const char* LOG_CATEGORY_NAME;

private:

	CUDDFacade(const CUDDFacade& facade);
	CUDDFacade& operator=(const CUDDFacade& lhs);

public:
	CUDDFacade();

	~CUDDFacade();

};
