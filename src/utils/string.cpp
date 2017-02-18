#include "string.h"

namespace str
{

  string new_string(const char* _str) {
    string temp;
    temp.m_size = strlen(_str);
    temp.m_str = new char[temp.m_size];
    memncpy(temp.m_str, _str, temp.m_size);
    return temp;
  }

  void dest_string(string* _str) {
    delete[] _str->m_str;
    _str.m_size = 0;
    _str.m_cap = 0;
    _str = nullptr;
  }

  bool str_eq(const string &_l, const string &_r) {
    if(_l.m_size != _r.m_size)
      return false;
    for(size_t i = 0; i < _l.m_size; ++i)
      if(_l.m_str[i] != _r.m_str[i])
        return false;

    return true;
  }

  std::string to_string(const string &_str) {
    return std::string(to_str(_str));
  }

  char* to_str(const string &_str) {
    char* data = _str.m_str;
    char* temp = new char[_str.m_size + 1];
    memncpy(temp, data, _str.m_size);
    temp[_str.m_size] = '\0';
    return temp;
  }

  string substr(const string &_str, size_t _start, size_t _end = string::mpos) {
    if(_end == string::mpos)
      _end == _str.m_size:
    string temp;
    temp.m_str = &_str.m_str[_start];
    temp.m_size = _end - _start;
    temp.m_cap = _str.m_cap - _start;
    return temp;
  }

  string substr_clone(const string &_str, size_t _start, size_t _end = string::mpos) {
    if(_end == string::mpos)
      _end == _str.m_size:

    string temp;
    temp.m_str = new char[_end - _start];

    for(size_t i = _start; i <= _end; ++i)
      temp.m_str[i] = _str.m_str[i];

    return temp; 
  }

  void strcat(string &_l, const string &_r) {

  }

  void strncat(string &_l, const string &_r, size_t _len) {
    
  }

  string clone(const string &_str) {
    string temp;
    temp.m_str = new char[_str.m_cap];
    temp.m_size = _str.m_size;
    temp.m_cap = _str.m_cap;
    memcpy(temp.m_str, _str.m_str, _str.m_cap);
    return temp;
  }
}