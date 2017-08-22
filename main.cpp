#include <cstdio>
#include <iostream>

#include "scanner/Scanner.h"
#include "token/Token.h"
#include "token/Location.h"
#include "utils/File.h"
#include "utils/string.h"

#include "parser/Parser.h"
// #include "parser/RhoContext.h"
#include "Rho.h"

bool debug = false;

#include <unistd.h>

using str::string;
using namespace std;
using namespace token;


int main(int argc, char* const* argv) {

  std::string fileName;

  int c;
  while((c = getopt(argc, argv, "f:d")) != -1) {
    switch(c) {
      case 'f':
        fileName = std::string(optarg);
        break;
      case 'd':
        debug = true;

    }
  }
	sys::File* file = sys::File::read_file(fileName);
  Rho_Interpreter rho(file);
  rho.compile();
  rho.print_asts();
	return 0;
}
