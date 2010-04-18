/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for FakeFile class.
 *
 *****************************************************************************/

#ifndef _SFTA_FAKE_FILE_HH_
#define _SFTA_FAKE_FILE_HH_

// Standard library header files
#include <cstdio>
#include <string>


// insert the class into proper namespace
namespace SFTA
{
	namespace Private
	{
		class FakeFile;
	}
}


class SFTA::Private::FakeFile
{
private:  // Private data members

	char* ptrBuffer_;

	size_t bufferSize_;

	FILE* ptrFile_;

	bool hasBeenOpened_;

	bool isClosed_;


private:  // Private methods

	FakeFile(const FakeFile& ff);
	FakeFile& operator=(const FakeFile& ff);

public:   // Public methods

	FakeFile();

	FILE* Open();

	void Close();

	std::string GetContent() const;

	~FakeFile();
};

#endif
