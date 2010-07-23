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


/**
 * @brief  Class that mocks a file and saves input to string
 *
 * This class provides a @c FILE* pointer that mocks an open file but saves
 * the input into a string that can be later read out.
 */
class SFTA::Private::FakeFile
{
private:  // Private data members

	/**
	 * @brief  Buffer
	 *
	 * Pointer to the buffer that stores input data
	 */
	char* ptrBuffer_;

	/**
	 * @brief  Size of buffer
	 *
	 * The size of buffer for input data
	 */
	size_t bufferSize_;

	/**
	 * @brief  @c FILE* pointer for the mock file
	 *
	 * @c FILE* pointer for the mock file
	 */
	FILE* ptrFile_;

	/**
	 * @brief  Denotes whether the mock file has been opened
	 *
	 * A Boolean value that denotes whether the mock file represented by the
	 * object has been opened.
	 */
	bool hasBeenOpened_;

	/**
	 * @brief  Denotes whether the mock file has been closed
	 *
	 * A Boolean value that denotes whether the mock file represented by the
	 * object has been closed (after being open).
	 */
	bool isClosed_;


private:  // Private methods


	/**
	 * @brief  Copy constructor
	 *
	 * Private copy constructor.
	 *
	 * @param[in]  ff  The object to be copied
	 */
	FakeFile(const FakeFile& ff);

	/**
	 * @brief  Assignment operator
	 *
	 * Private assignment operator.
	 *
	 * @param[in]  ff  The object to be assigned
	 *
	 * @returns  Assigned object
	 */
	FakeFile& operator=(const FakeFile& ff);


public:   // Public methods


	/**
	 * @brief  Constructor
	 *
	 * The constructor of the class
	 */
	FakeFile();


	/**
	 * @brief  The method that opens the mock stream
	 *
	 * Opens the mock stream and returns the @c FILE* pointer so that it can be
	 * handled using a UNIX file pointer in the common way
	 *
	 * @returns  UNIX @c FILE* pointer
	 */
	FILE* Open();


	/**
	 * @brief  Closes the stream
	 *
	 * This method closes a previously opened mock file. Note that the file
	 * needs to have been opened and haven't been closed yet.
	 */
	void Close();


	/**
	 * @brief  Gets the data written to the file
	 *
	 * Retuns a string that contains the data that has been written to the mock
	 * file.
	 *
	 * @returns  Data written to the mock file
	 */
	std::string GetContent() const;


	/**
	 * @brief  Destructor
	 *
	 * The destuctor
	 */
	~FakeFile();
};

#endif
