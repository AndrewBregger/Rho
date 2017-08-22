#include <iostream>
#include <string>
#include <queue>

#include "Rho.h"



using parse::Parser;
using namespace ast;


Rho_Interpreter::
Rho_Interpreter(sys::File* f) {
	file = f;
	parser = new Parser;
}

void
Rho_Interpreter::
compile() {
	std::unordered_map<std::string, ast::AstFile*> _files;
	std::queue<sys::File*> unParsed;

	// add the root file to the queue

	unParsed.push(file);


	// parses all of the needed file for root file.
	// it also will not reparse files that have already been parsed.
	while(!unParsed.empty()) {
		sys::File* f = unParsed.front();
		unParsed.pop();

		AstFile* ast = parser->parse_file(f);
		for(auto i : ast->m_imports) {
			if(_files.find(i->fullPath) == _files.end())
				unParsed.push(sys::File::read_file(i->fullPath));
		}

		_files.emplace(ast->m_file->GetFullPath(), ast);
	}

	// assosiate import asts to astfile ids.
	for(auto f : _files) {
		for(auto i : f.second->m_imports) {
			auto iter = _files.find(i->fullPath);
			if(iter == _files.end()) {
				std::cout << "Compiler error: Unparsed import file\n";
				exit(100);
			}
			i->ast = iter->second->m_id;
		}
	}

	// set all files in object
	for(auto f : _files)
		files.emplace(f.second->m_id, f.second);
}

void print_files(ast::AstFile* _file) {
  std::cout << "File: " << _file->m_file->GetPath() << std::endl;
  std::cout << "ID: " << _file->m_id << std::endl;
  for(auto i : _file->m_stmts)
    i->print(0);
}

void
Rho_Interpreter::
print_asts() {
	for(auto f : files) {
		print_files(f.second);
	}
}