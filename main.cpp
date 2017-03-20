#include <cstdio>
#include <iostream>

#include "scanner/Scanner.h"
#include "token/Token.h"
#include "token/Location.h"
#include "utils/File.h"
#include "utils/string.h"

#include "parser/RhoContext.h"



using str::string;
using namespace std;
using namespace token;

void print_token(Token* _token) {
 	printf("\t");
  if(_token->token() == TKN_ERROR)
    return;
  auto loc = _token->location();
  printf("%-8s\tat %3zu|%3zu  -%3zu ", Token::get_str(_token->token()), 
    loc.m_line, loc.m_column, loc.m_ecolumn);
  switch(_token->token()) {
    case TKN_LSTRING:
      printf("(%s)\n", to_str(_token->get_string()));
      break;
    case TKN_LINT:
      printf("(%d)\n", _token->get_int());
      break;
    case TKN_LCHAR:
      printf("(%c)\n", _token->get_char());
      break;
    case TKN_LDOUBLE:
      printf("(%lf)\n", _token->get_float());
      break;
    case TKN_IDENTIFIER:
      printf("(%s)\n", to_str(_token->get_string()));
      break;
    case TKN_LBOOL:
      cout << "(" << _token->get_boolean() << ")\n";
      break;
    case TKN_ERROR:
      cout << "Error" << endl;
      break;
     /*
    case TKN_DIRECTIVE:
      cout << "(" << _token.m_values.directiveString << ")\n";
      break;
      */
    default:
      putchar('\n');
  }
}

int main(int argc, const char** argv) {
	if(argc < 2) {
		cout << "Error: Usage ./rhoc {file}" << endl;
    cout << *argv << endl;
		return 1;
	}

  str::string test = str::new_string("module main;");
  auto m = str::substr(test, 7, 11);
  cout << m << endl;
	str::string path = str::new_string(argv[1]);
	sys::File* file = sys::File::read_file(path);
	scanner::Scanner scanner(file);
	Token* t;
	while( (t = scanner.scan())->token() != TKN_EOF) {
		print_token(t);
	}
	return 0;
}
