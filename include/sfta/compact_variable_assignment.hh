/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    File with CompactVariableAssignment structure.
 *
 *****************************************************************************/

#ifndef _COMPACT_VARIABLE_ASSIGNMENT_HH_
#define _COMPACT_VARIABLE_ASSIGNMENT_HH_


// insert class into correct namespace
namespace SFTA
{
	namespace Private
	{
		template
		<
			size_t MaxSize
		>
		struct CompactVariableAssignment;
	}
}


/**
 * @brief   Compact variable assignment
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * A class that represents assignments to Boolean variables in a compact way.
 * Assigned values can be one of '0', '1' and 'X', where 'X' means <em>don't
 * care</em>.
 *
 * @tparam  Variables  The number of Boolean variables.
 */
template
<
	size_t Variables
>
struct SFTA::Private::CompactVariableAssignment
{
public:   // Public data types

	enum
	{
		VariablesCount = Variables
	};


	enum
	{
		ZERO = 0x01,
		ONE = 0x02,
		DONT_CARE = 0x03
	};


private:  // Private data types

	enum
	{
		BitsPerVariable = 2
	};


	enum
	{
		// needs to be multiple of BitsPerVariable
		BitsInChar = 8
	};


	enum
	{
		NumberOfChars = (VariablesCount*BitsPerVariable - 1) / BitsInChar + 1
	};


	enum
	{
		DefaultMask = 0x03
	};


private:  // Private data members

	/**
	 * @brief  The value of the assignment
	 *
	 * Array of characters representing the value of the assignment.
	 */
	char vars_[NumberOfChars];


	/**
	 * @brief  The name of the Log4cpp category for logging
	 *
	 * The name of the Log4cpp category used for logging messages from this
	 * class.
	 */
	static const char* LOG_CATEGORY_NAME;


private:  // Private methods


	/**
	 * @brief  Gets index of @c char at given variable index
	 *
	 * Returns index of the @c char that holds the value of variable at given
	 * index.
	 *
	 * @see  getIndexInsideChar()
	 *
	 * @param[in]  index  Index of the Boolean variable
	 *
	 * @returns  Index of the @c char in which the variable has value
	 */
	static inline size_t getIndexOfChar(size_t index)
	{
		return (index * BitsPerVariable) / BitsInChar;
	}


	/**
	 * @brief  Gets index of bit inside a @c char for given variable index
	 *
	 * Returns index of a bit in a @c char that starts the block of bits that
	 * hold the value of variable at given index.
	 *
	 * @see  getIndexOfChar()
	 *
	 * @param[in]  index  Index of the Boolean variable
	 *
	 * @returns  Index of the bit that holds the value of the variable
	 */
	static inline size_t getIndexInsideChar(size_t index)
	{
		return (index * BitsPerVariable) % BitsInChar;
	}


public:   // Public methods

	/**
	 * @brief  Constructor from std::string
	 *
	 * A constructor that creates an instance of the class from a std::string
	 * that desribes the assignments to variable. The string can only hold
	 * values '0', '1' and 'X', where 'X' means <em>don't care</em>.
	 *
	 * @param[in]  value  The string with the value of variables
	 */
	explicit CompactVariableAssignment(const std::string& value)
		: vars_()
	{
		// Assertions
		assert(value.length() == VariablesCount);

		for (size_t i = 0; i < VariablesCount; ++i)
		{	// load the string into the array of variables
			char val = 0;
			switch (value[i])
			{
				case '0': val = ZERO;      break;
				case '1': val = ONE;       break;
				case 'X': val = DONT_CARE; break;
				default: throw std::runtime_error("Invalid input value!");
			}

			// prepare the mask
			char mask = (DefaultMask << getIndexInsideChar(i)) ^ static_cast<char>(-1);

			// mask out bits at given position by the mask
			vars_[getIndexOfChar(i)] &= mask;

			// prepare new value of given bits
			val <<= getIndexInsideChar(i);

			// insert the value of given bits
			vars_[getIndexOfChar(i)] |= val;
		}
	}


	/**
	 * @brief  Returns value of variable at given index
	 *
	 * Returns the value of variable at given index.
	 *
	 * @param[in]  i  Index of the variable
	 *
	 * @returns  Value of the variable
	 */
	inline char GetIthVariableValue(size_t i) const
	{
		// Assertions
		assert(i <= VariablesCount);

		return (vars_[getIndexOfChar(i)] >> getIndexInsideChar(i)) & DefaultMask;
	}


	/**
	 * @brief  The number of variables
	 *
	 * Returns the number of variables of the variable assignment.
	 *
	 * @returns  The number of variables of the variable assignment
	 */
	inline size_t Size() const
	{
		return VariablesCount;
	}


	/**
	 * @brief  Returns string representation
	 *
	 * Transforms the variable assignment to its string representation.
	 *
	 * @see  operator<<()
	 *
	 * @returns  The string representation of the variable assignment
	 */
	std::string ToString() const
	{
		std::string result;

		for (size_t i = 0; i < VariablesCount; ++i)
		{	// append all variables to the string
			switch (GetIthVariableValue(i))
			{
				case ZERO:      result += '0'; break;
				case ONE:       result += '1'; break;
				case DONT_CARE: result += 'X'; break;
				default: throw std::runtime_error("Invalid bit value!");
			}
		}

		return result;
	}


	/**
	 * @brief  Overloaded << operator
	 *
	 * Overloaded << operator for output stream.
	 *
	 * @see  ToString()
	 *
	 * @param[in]  os    The output stream
	 * @param[in]  asgn  Assignment to the variables
	 *
	 * @returns  Modified output stream
	 */
	friend std::ostream& operator<<(std::ostream& os, const CompactVariableAssignment& asgn)
	{
		return (os << asgn.ToString());
	}

};


// Setting the logging category name for Log4cpp
template
<
	size_t V
>
const char* SFTA::Private::CompactVariableAssignment<V>::LOG_CATEGORY_NAME = "compact_variable_assignment";

#endif
