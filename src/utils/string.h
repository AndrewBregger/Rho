#ifndef STRING_H_
#define STRING_H_
#include <string>


/// @breif A string implementation for the Rho compiler.
struct string {
  char* m_str;    ///< character array
  size_t m_size;  ///< size of the string content
  size_t m_cap;   ///< allocated size of the string
};

///////////////////////////////////////////////////////////////////////////////
/// @brief String equality function
/// @param _l left string
/// @param _r right string
/// @return true if equal, false otherwise
/// @note _l == _r
bool str_eq(const string& _l, const string& _r);

std::string to_string(const string& _str);
char* to_str(const string& _str);

#endif