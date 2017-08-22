#ifndef RHO_H_
#define RHO_H_

#include <vector>
#include <map>
#include <unordered_map>


#include "ast/Ast.h"
#include "parser/Parser.h"
#include "utils/File.h"


class Rho_Interpreter {
	public:
		Rho_Interpreter(sys::File* f);
		void compile();
		void print_asts();
	private:

		sys::File* file;
		parse::Parser* parser{nullptr};


		// TypeChecker* checker;
		// CodeGenerator* generator;

		std::unordered_map<size_t, ast::AstFile*> files;
};

#endif