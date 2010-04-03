#ifndef _CONVERT_HH_
#define _CONVERT_HH_


#include <string>
#include <sstream>

namespace SFTA
{
	namespace Private
	{
		class Convert;
	}
}


/**
 *  @brief  A static class for conversions.
 *
 *  This is a static class that constains useful methods for various
 *  conversions.
 */
class SFTA::Private::Convert
{
	private:

		// Disable both constructors, destructor and assignment operator by making
		// them private
		Convert();
		~Convert();
		Convert(const Convert &);
		Convert & operator=(const Convert &);

	public:

		/**
		 * @brief  Converts an object to string
		 *
		 * Static method for conversion of object of any class with the << output
		 * operator into a string
		 *
		 * @param[in]  n  The object for the conversion
		 * @returns    The string representation of the object
		 */
		template <typename T>
		static std::string ToString(const T& n)
		{
			std::ostringstream oss;       // the output stream for the string
			oss << n;                     // insert the object into the stream

			return oss.str();             // return the string
		}


		/**
		 * @brief  Converts pointer to string
		 *
		 * Static method for conversion of a pointer to a string
		 *
		 * @param[in]  ptr  The pointer for the conversion
		 * @returns    The string representation of the pointer
		 */
		template <typename T>
		static std::string PointerToString(const T* ptr)
		{
			std::ostringstream oss;       // the output stream for the string
			oss << ptr;                   // insert the pointer into the stream

			return oss.str();             // return the string
		}

};

#endif
