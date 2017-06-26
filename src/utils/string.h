#ifndef STRING_H_
#define STRING_H_
#include <string>
#include <vector>
#include <fstream>


/// @breif A string implementation for the Rho compiler.

namespace str {

struct string;

bool str_eq(const string& _l, const string& _r);

struct string {
  char* m_str;    ///< character array
  size_t m_size;  ///< size of the string content
  size_t m_cap;   ///< allocated size of the string

  char& operator[] (size_t _index) {
  	return m_str[_index];
  }

  const char& operator[] (size_t _index) const {
    return m_str[_index];
  }

  bool operator== (const str::string& _r) const {
    return str::str_eq(*this, _r);
  }
  
  static size_t mpos;
};

///////////////////////////////////////////////////////////////////////////////
/// @brief String equality function
/// @param _str Inputed c-string.
str::string new_string(const char* _str);

void dest_string(str::string* _str);

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
std::string to_string(const str::string& _str);

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
/// @return string containing sub-string.
/// @note Returns a copied data.
string substr(const string& _str, size_t _start, size_t _end = str::string::mpos);

///////////////////////////////////////////////////////////////////////////////
/// @brief gets a substring.
/// @param _str string being search
/// @param _start the starting point
/// @param _end The end point.
/// @return string containing sub=string.
/// @note The memory is the same.
string substr_c(const string& _str, size_t _start, size_t _end = str::string::mpos);

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

///////////////////////////////////////////////////////////////////////////////
/// @brief Find a substring within _str
/// @param _str String being copied.
/// @return Index of where it starts
size_t find(const string& _str, const string& _s);
size_t find(const string& _str, const char* _s);
size_t find(const string& _str, const std::string& _s);
size_t find(const string& _str, char _ch);

///////////////////////////////////////////////////////////////////////////////
/// @brief Find a substring within _str
/// @param _str String being copied.
/// @return Index of where it starts
size_t find_last(const string& _str, const string& _s);
size_t find_last(const string& _str, const char* _s);
size_t find_last(const string& _str, const std::string& _s);
size_t find_last(const string& _str, char _ch);

///////////////////////////////////////////////////////////////////////////////
/// @brief Finds all substring within _str
/// @param _str String being copied.
/// @return all of the indexs
std::vector<size_t> find_all(const string& _str, const string& _s);
std::vector<size_t> find_all(const string& _str, const char* _s);
std::vector<size_t> find_all(const string& _str, const std::string& _s);
std::vector<size_t> find_all(const string& _str, char _ch);


///////////////////////////////////////////////////////////////////////////////
/// @brief Output operator
/// @param _out Output stream
/// @param _str String being operated on.
/// @return modified stream.
std::ostream& operator<< (std::ostream& _out, const string& _str);

}

#endif