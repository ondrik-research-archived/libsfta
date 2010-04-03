
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
private:	// Data members
	DdManager* manager_;


public:


private:

	CUDDFacade(const CUDDFacade& facade);
	CUDDFacade& operator=(const CUDDFacade& lhs);

public:
	CUDDFacade();

	~CUDDFacade();

};
