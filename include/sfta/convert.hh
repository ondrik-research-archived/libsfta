/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Convert class declaration.
 *
 *****************************************************************************/

#ifndef _SFTA_CONVERT_HH_
#define _SFTA_CONVERT_HH_

// Standard library headers
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>


// insert class to proper namespace
namespace SFTA { namespace Private { class Convert; } }


/**
 *  @brief  A static class for conversions.
 *
 *  This is a static class that constains useful methods for various
 *  conversions.
 */
class SFTA::Private::Convert
{
private:

	/**
	 * @brief  Private default constructor
	 *
	 * Default constructor which is private to disable creating an instance
	 * of the class.
	 */
	Convert();


	/**
	 * @brief  Private copy constructor
	 *
	 * Copy constructor which is private to disable creating an instance
	 * of the class.
	 *
	 * @param[in]  convert  The instance to be copied
	 */
	Convert(const Convert& convert);


	/**
	 * @brief  Private assignment operator
	 *
	 * Assignment operator which is private to disable creating an instance
	 * of the class.
	 *
	 * @param[in]  convert  The instance to be copied
	 *
	 * @returns  The resulting object
	 */
	Convert& operator=(const Convert& convert);


	/**
	 * @brief  Private destructor
	 *
	 * Private destructor.
	 */
	~Convert();


public:

	/**
	 * @brief  Converts an object to string
	 *
	 * Static method for conversion of an object of any class with the << output
	 * operator into a string
	 *
	 * @param[in]  n  The object for the conversion
	 *
	 * @returns  The string representation of the object
	 */
	template <typename T>
	static std::string ToString(const T& n)
	{
		// the output stream for the string
		std::ostringstream oss;
		// insert the object into the stream
		oss << n;
		// return the string
		return oss.str();
	}


	/**
	 * @brief  Converts an object to string (pointer specialization)
	 *
	 * Static method for conversion of a pointer of an object of any class with
	 * the << output operator into a string
	 *
	 * @param[in]  ptr  Pointer for the conversion
	 *
	 * @returns  The string representation of the pointer
	 */
	template <typename T>
	static std::string ToString(T* ptr)
	{
		// the output stream for the string
		std::ostringstream oss;
		// insert the string of the underlying class into the stream
		oss << ToString(*ptr);
		// return the string
		return oss.str();
	}


	/**
	 * @brief  Converts an object to string (std::vector specialization)
	 *
	 * Static method for conversion of a vector of objects of any class with the
	 * << output operator into a string
	 *
	 * @param[in]  vec  The vector for the conversion
	 *
	 * @returns  The string representation of the vector
	 */
	template <typename T>
	static std::string ToString(const std::vector<T>& vec)
	{
		// the output stream for the string
		std::ostringstream oss;

		oss << "(";		// opening tag
		for (typename std::vector<T>::const_iterator it = vec.begin();
			it != vec.end(); ++it)
		{	// for each element of the vector
			if (it != vec.begin())
			{	// if we are not at the first element
				oss << ", ";
			}

			// the string of the element
			oss << ToString(*it);
		}

		oss << ")";		// closing tag

		// return the string
		return oss.str();
	}


	/**
	 * @brief  Converts an object to string (std::set specialization)
	 *
	 * Static method for conversion of a set of objects of any class with the
	 * << output operator into a string
	 *
	 * @param[in]  st  The set for the conversion
	 *
	 * @returns  The string representation of the set
	 */
	template <typename T>
	static std::string ToString(const std::set<T>& st)
	{
		// the output stream for the string
		std::ostringstream oss;

		oss << "{";		// opening tag
		for (typename std::set<T>::const_iterator it = st.begin();
			it != st.end(); ++it)
		{	// for each element of the set
			if (it != st.begin())
			{	// if we are not at the first element
				oss << ", ";
			}

			// the string of the element
			oss << ToString(*it);
		}

		oss << "}";		// closing tag

		// return the string
		return oss.str();
	}


	/**
	 * @brief  Converts a string to an object 
	 *
	 * Static method for conversion of a string to an object of any class with
	 * the >> input operator
	 *
	 * @param[in]  str  The string for the conversion
	 *
	 * @returns  The object that corresponds to the string
	 */
	template <typename T>
	static T FromString(const std::string& str)
	{
		T result;

		// the input stream for the string
		std::istringstream iss(str);
		if (!(iss >> result))
		{	// if there was an error
			throw std::invalid_argument("SFTA::Private::Convert::FromString: Invalid argument");
		}

		return result;
	}

};

#endif
