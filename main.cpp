#include "util/string.h"
#include <iostream>

using str::string;

int main() {
  string s = new_string("Test String");
  string st = new string("String");
  string sub = substr(s, 5);
  if(str_eq(sub, st))
    std::cout << "Strings are equal\n";
    
  return 0;
}
