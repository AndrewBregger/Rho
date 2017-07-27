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

void print_files(ast::AstFile* _file) {
  std::cout << "File: " << _file->m_file->GetPath() << std::endl;
  std::cout << "ID: " << _file->m_id << std::endl;
  for(auto i : _file->m_stmts)
    i->print(0);
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
	sys::File* file = sys::File::read_file(fileName);

  // scanner::Scanner scanner(file);
  // Token t;
  // while((t = scanner.scan()).token() != TKN_EOF) {
  //   print_token(t);
  // }

  parse::Parser parser;
  // ast::AstFile* ast = parser.parse_files();
  auto asts = parser.parse_files(file);
  for(const auto a : asts)
    print_files(a);
  // for(const auto& decl : ast->m_decls)
  //   ast::ast_print(decl, 0);
	// ast::AstFile* ast = parser.parse_file();
  // ast->print(0);
	return 0;
}
