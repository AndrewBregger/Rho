#include <cstdio>
#include <iostream>

#include "scanner/Scanner.h"
#include "token/Token.h"
#include "token/Location.h"
#include "utils/File.h"
#include "utils/string.h"

#include "parser/Parser.h"
#include "parser/RhoContext.h"

bool debug = false;

#include <unistd.h>

using str::string;
using namespace std;
using namespace token;

void print_token(const Token& _token) {
 	printf("\t");
  if(_token.token() == TKN_ERROR)
    return;
  auto loc = _token.location();
  printf("%-8s\tat %3zu|%3zu  -%3zu ", token_string(_token.token()).c_str(),
    loc.m_line, loc.m_column, loc.m_ecolumn);
  switch(_token.token()) {
    case TKN_LSTRING:
      printf("(%s)\n", _token.get_string());
      break;
    case TKN_LINT:
      printf("(%d)\n", _token.get_int());
      break;
    case TKN_LCHAR:
      printf("(%c)\n", _token.get_char());
      break;
    case TKN_LDOUBLE:
      printf("(%lf)\n", _token.get_float());
      break;
    case TKN_IDENTIFIER:
      printf("(%s)\n", _token.get_string());
      break;
    case TKN_LBOOL:
      cout << "(" << _token.get_boolean() << ")\n";
      break;
    case TKN_ERROR:
      cout << "Error" << endl;
      break;
    default:
      putchar('\n');
  }
}

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

  std::cout << fileName << std::endl;
	sys::File* file = sys::File::read_file(fileName);

  // scanner::Scanner scanner(file);
  // Token t;
  // while((t = scanner.scan()).token() != TKN_EOF) {
  //   print_token(t);
  // }

  parse::Parser parser(file);
  parser.init();
  ast::AstNode* expr = parser.parse_expr(false);
  // parser.parse_files();
  ast::ast_print(expr, 0);
	// ast::AstFile* ast = parser.parse_file();
  // ast->print(0);
	return 0;
}
