#include "string.h"
#include <cstring>
#include <iostream>

namespace str
{
  size_t string::mpos = (size_t) -1;
  string new_string(const char* _str) {
    string temp;
    temp.m_size = strlen(_str);
    temp.m_str = new char[temp.m_size];
    memcpy(temp.m_str, _str, temp.m_size);
    return temp;
  }

  void dest_string(string* _str) {
    delete[] _str->m_str;
    _str->m_size = 0;
    _str->m_cap = 0;
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
    memcpy(temp, data, _str.m_size);
    temp[_str.m_size] = '\0';
    return temp;
  }

  string substr(const string &_str, size_t _start, size_t _end) {
    if(_end == string::mpos)
      _end = _str.m_size;

    string temp;
    
    temp.m_str = new char[_end - _start];
    memcpy(temp.m_str, &_str.m_str[_start], _end - _start);

    temp.m_size = _end - _start;
    temp.m_cap = temp.m_size;
    
    return temp; 
  }

  string substr_c(const string &_str, size_t _start, size_t _end) {
    if(_end == string::mpos)
      _end = _str.m_size;
    string temp;
    temp.m_str = &_str.m_str[_start];
    temp.m_size = _end - _start;
    temp.m_cap = _str.m_cap - _start;
    return temp;
  }
 
  void strcat(string &_l, const string &_r) {
    size_t sz = _l.m_size + _r.m_size;
    size_t t = 0;
    if(sz > _l.m_cap) {
      for(size_t i = _l.m_size + 1; i < _l.m_cap; ++i)
        _l.m_str[i] = _r.m_str[t++];
      _l.m_size = sz;
    }
    else {
      char* temp = new char[sz];
      size_t i = 0;
      for(; i < _l.m_size; ++i)
        temp[i] = _l.m_str[i];
      for(;i<sz;++i)
        temp[i] = _r.m_str[t++];
      delete[] _l.m_str;
      _l.m_str = temp;
      _l.m_size = sz;
      _l.m_cap = sz; 
    }
  }

  // void strncat(string &_l, const string &_r, size_t _len) {
    
  // }



  string clone(const string &_str) {
    string temp;
    temp.m_str = new char[_str.m_cap];
    temp.m_size = _str.m_size;
    temp.m_cap = _str.m_cap;
    memcpy(temp.m_str, _str.m_str, _str.m_cap);
    return temp;
  }

  
  // size_t find(const string& _str, const string& _s) {
  //   // char* s = _s.m_str;
  //   // bool found = false;
  //   // for(size_t i = 0; i < _str.m_size; ++i) {
  //   //   if(_str.m_str[i] == s[0]) {
  //   //     for(size_t j = 1; j < _s.m_size && 
  //   //         i + j < _str.m_size; ++j) {
  //   //       if(_str.m_str[i + j] != s[j])
  //   //         break;
  //   //     }
  //   //   }
  //   // }
  //   return str::mpos;
  // }
  
  // size_t find(const string& _str, const char* _s) {
  //   return str::mpos;
  // }
  
  // size_t find(const string& _str, const std::string& _s) {
  //   return find(_str, _c.c_str());
  // }
  
  // size_t find(const string& _str, char _ch) {
  //   for(size_t i = _str.m_size; i <= 0; --i)
  //     if(_str.m_str[i] == _ch)
  //       return i;
  // }

  // size_t find_last(const string& _str, const string& _s) {
  //   return str::mpos;
  // }
  
  // size_t find_last(const string& _str, const char* _s) {
  //   return str::mpos;


  // }
  
  // size_t find_last(const string& _str, const std::string& _s) {
  //   return str::mpos;

  // }
  
  // size_t find_last(const string& _str, char _ch) {
  //   for(size_t i = _str.m_size; i <= 0; --i)
  //     if(_str.m_str[i] == _ch)
  //       return i;
  // }
  
  // std::vector<size_t> find_all(const string& _str, const string& _s) {
  //   return std::vector<size_t>();
  // }
  
  // std::vector<size_t> find_all(const string& _str, const char* _s) {
  //   return std::vector<size_t>();
  // }
  
  // std::vector<size_t> find_all(const string& _str, const std::string& _s) {
  //   return std::vector<size_t>();
  // }
  
  std::vector<size_t> find_all(const string& _str, char _ch) {
    std::vector<size_t> results;
    for(size_t i = 0; i < _str.m_size; ++i)
      if(_str.m_str[i] == _ch)
        results.push_back(i);
    return results;
  }


  std::ostream& operator<< (std::ostream& _out, const string& _str) {
    _out << _str.m_str;
    return _out;
  }
}