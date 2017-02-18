#ifndef STRING_H_
#define STRING_H_
#include <string>


/// @breif A string implementation for the Rho compiler.

namespace str {

struct string {
  char* m_str;    ///< character array
  size_t m_size;  ///< size of the string content
  size_t m_cap;   ///< allocated size of the string
  static size_t mpos = (size_t) -1;
};

///////////////////////////////////////////////////////////////////////////////
/// @brief String equality function
/// @param _str Inputed c-string.
string new_string(const char* _str);

void dest_string(string* _str);

///////////////////////////////////////////////////////////////////////////////
/// @brief String equality function
/// @param _l left string
/// @param _r right string
/// @return true if equal, false otherwise
/// @note _l == _r
bool str_eq(const string& _l, const string& _r);

///////////////////////////////////////////////////////////////////////////////
/// @brief Convert string to std::string
/// @param _str String being converted.
/// @return std::string
std::string to_string(const string& _str);

///////////////////////////////////////////////////////////////////////////////
/// @brief Converrt String to c-string.
/// @param _str String being converted.
/// @return true returns a c-style string.
char* to_str(const string& _str);

///////////////////////////////////////////////////////////////////////////////
/// @brief gets a substring.
/// @param _str string being search
/// @param _start the starting point
/// @param _end The end point.
/// @return string containing sub=string.
/// @note The memory is the same.
string substr(const string& _str, size_t _start, size_t _end = string::mpos);

///////////////////////////////////////////////////////////////////////////////
/// @brief Creates a sub string from the inputted string.
/// @note Returns a copied data.
string substr_clone(const string& _str, size_t _start, size_t _end = string::mpos);

///////////////////////////////////////////////////////////////////////////////
/// @brief Concatinates Strings
/// @param _l left string
/// @param _r right string
/// @note contatinates _r to _l
void strcat(string& _l, const string& _r);

///////////////////////////////////////////////////////////////////////////////
/// @brief Concatinates String
/// @param _l left string
/// @param _r right string
/// @param _len the length to concatinate
/// @return true if equal, false otherwise
void strncat(string& _l, const string& _r, size_t _len);

///////////////////////////////////////////////////////////////////////////////
/// @brief String equality function
/// @param _str String being copied.
/// @return copied string.
string clone(const string& _str);

}

#endif