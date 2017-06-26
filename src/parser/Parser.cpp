#include "Parser.h"
#include "utils/File.h"
#include "scanner/Scanner.h"
#include <iostream>
#include <vector>
#include <cstdarg>
#include <cstring>

#include <cassert>

using namespace ast;
using namespace parse;
using namespace scanner;
using namespace token;


extern bool debug;

// macros for getting the  token


namespace parse {


	void report_error(AstFile* _file, const char* _msg, ...);
	void report_file_error(AstFile* _file, const char* _msg, ...);
	void report_warning(AstFile* _file, const char* _msg, ...);
	void debug_print(const char* msg, ...);

	Parser::
	Parser(sys::File* _file) {
		m_file = _file;
		m_atomTable = new ast::AtomTable;
	}

	/// Scans the entire file into a stream of tokens.
	ParseFileError
	Parser::
	init() {
		ParseFileError err = ParseFile_None;
		m_ast = ast_file(m_file);
		if(m_file->GetError() != sys::FileError::File_None)
			return static_cast<ParseFileError>(m_file->GetError());
		scanner::Scanner scanner(m_file);
		Token t;
		while((t = scanner.scan()).token() != TKN_EOF) {
			// combines consecutive string literals into one string
			if(t.token() == TKN_LSTRING) {
				Token s = t;
				std::string temp(s.m_values.str);
				while((t = scanner.scan()).token() == TKN_LSTRING) {
					temp += std::string(t.m_values.str);
				}
				m_ast->m_tokens.push_back(s);
			}

			if(t.token() == TKN_ERROR)
				err = ParseFile_InvalidToken;
			m_ast->m_tokens.push_back(t);
		}
		m_ast->m_tokens.push_back(t);

		m_ast->m_currToken = m_ast->m_tokens[m_ast->m_tokenIndex];

		for(Token& t : m_ast->m_tokens) {
			t.print();
		}
		return err;
	}

	ast::AstFile*
	Parser::
	parse_file() {
		// get the base file
		const std::string& file_path = m_file->GetFullPath();
		std::string base_dir(file_path.c_str());
		for(size_t i = base_dir.size() - 1; i >= 0; --i) {
			if(base_dir[i] == '/' || base_dir[i] == '\\')
				break;
			base_dir.pop_back();
		}
		// parse a list of stmts, decls.
		m_ast->m_decls = parse_stmt_list();

		// set up for symantic checking

		return m_ast;
	}

	ast::AstFile*
	Parser::
	parse_files() {
		ParseFileError err = init();
		switch(err) {
			case ParseFile_Empty:
				report_file_error(m_ast, "File Error: '%.*s' was empty\n", m_file->GetPath().size(), m_file->GetPath().c_str());
				break;
			case ParseFile_Permissions:
				report_file_error(m_ast, "File Error: '%.*s' wrong permissions on file\n", m_file->GetPath().size(), m_file->GetPath().c_str());
				break;
			case ParseFile_WrongExtension:
				report_file_error(m_ast, "File Error: '%.*s' wrong extension\n", m_file->GetPath().size(), m_file->GetPath().c_str());
				break;
			case ParseFile_InvalidFile:
				report_file_error(m_ast, "File Error: '%.*s' file doen't exist\n", m_file->GetPath().size(), m_file->GetPath().c_str());
				break;
			case ParseFile_InvalidToken:
				report_file_error(m_ast, "File Error: '%.*s' has invalid token\n", m_file->GetPath().size(), m_file->GetPath().c_str());
				break;
			default:
				return parse_file();
		}
		return nullptr;
	}

	void
	Parser::
	next_token() {
		if(++m_ast->m_tokenIndex >= m_ast->m_tokens.size()) {
			// report error
			std::cout << "Shouldnt happend\n";
			exit(1);
		}
		m_ast->m_prevToken = m_ast->m_currToken;
		m_ast->m_currToken = m_ast->m_tokens.at(m_ast->m_tokenIndex);
	}

	Token
	Parser::
	peak_token(int _num) {
		assert(m_ast->m_tokenIndex + _num < m_ast->m_tokens.size());
		return m_ast->m_tokens[m_ast->m_tokenIndex + _num];
	}

	bool
	Parser::
	check_token(token::Token_Type _type) {
		return m_ast->m_currToken.token() == _type;
	}
	bool
	Parser::
	allow_token(token::Token_Type _type) {
		if(m_ast->m_currToken.token() == _type) {
			next_token();
			return true;
		}
		else
			return false;
	}


	token::Token
	Parser::
	expect_token(token::Token_Type _type) {
		// std::cout << "Expected: " << token_string(_type) << " "
		          // << "Got: " << token_string(m_ast->m_currToken.token()) << std::endl;
		Token prev = m_ast->m_currToken;
		if(prev.token() != _type) {
			// report error
			report_error(m_ast, "Expecting token '%s', found '%s'\n",
			 token_string(_type).c_str(), token_string(prev.token()).c_str());
		}
		next_token();
		return prev;
	}

	ast::AstNodeList
	Parser::
	parse_stmt_list() {
		debug_print("parse stmt list\n");
		AstNodeList stmts;
		while(m_ast->m_currToken.token() != TKN_EOF) {
			ast::add_node(stmts, parse_stmt());
		}
		return stmts;
	}

	ast::AstNode*
	Parser::
	parse_stmt() {
		debug_print("parse stmt\n");
		Token token = m_ast->m_currToken;
		next_token();
		ast::AstNode* stmt = nullptr;
		switch(token.token()) {
			case TKN_LET:
			case TKN_VAR:
				stmt = parse_decl_expr();
			case TKN_IDENTIFIER: {
				Token t = peak_token();
				if(t.token() == TKN_DCOL)
					stmt = parse_decl_expr();
				else
					stmt = parse_expr();
				} break;
			case TKN_HASH:
				stmt = parse_hash_directive();
				break;
			default:
				return nullptr;
		}
		return stmt;
	}

	ast::AstNode*
	Parser::
	parse_decl_expr() {
		debug_print("parse decl expr\n");
		Token prev = m_ast->m_prevToken;
		if(prev.token() == TKN_LET || prev.token() == TKN_VAR)
				return parse_variable_decl();

		// this is the :: for a function of type definition
		// we do not care about the token itself so i am not getting it.
		expect_token(TKN_DCOL);

		Token curr = m_ast->m_currToken;

		if(curr.token() == TKN_STRUCT || curr.token() == TKN_CLASS)
			return parse_type_decl();



		return nullptr;
	}

	ast::AstNode*
	Parser::
	parse_variable_decl() {
		AstNodeList ids = parse_identifier_list();
		AstNodeList values;
		AstNode* type = nullptr;



		if(allow_token(TKN_COL)) {
			type = parse_type_or_ident();
		}
		if(allow_token(TKN_ASN)) {
			return parse_primary_expr();
		}
		else {
			// go to next stmt
			report_error(m_ast, "type must be specified or initialized\n");
		}


		return ast_variable_spec(ids, values, type);
	}

	ast::AstNode*
	Parser::
	parse_type_decl() {
		return nullptr;
	}

	ast::AstNode*
	Parser::
	parser_function_spec() {
		return nullptr;
	}

	ast::AstNode*
	Parser::
	parse_type_or_ident() {
		if(m_ast->m_currToken.type() == PrimativeType)
			return ast_primative_type(m_ast->m_currToken);
		switch(m_ast->m_currToken.token()) {
			case TKN_IDENTIFIER:
				return parse_selector_expr();
			case TKN_MULT:
			case TKN_LBRACE:
			default:
				break;
		}

		// no valid type found
		return nullptr;
	}

	ast::AstNode*
	Parser::
	parse_field() {
		AstNode* name = parse_identifier();
		expect_token(TKN_COL);
		AstNode* type = parse_type_or_ident();
		return ast_field_spec(name->Ident.token, name, type);
	}

	ast::AstNode*
	Parser::
	parse_expr() {
		debug_print("parse expr\n");
		return nullptr;
	}

	ast::AstNode*
	Parser::
	parse_primary_expr() {
		return nullptr;
	}

	ast::AstNode*
	Parser::
	parse_selector_expr() {
		return nullptr;	
	}

	ast::AstNode*
	Parser::
	parse_hash_directive() {
		debug_print("parse hash directive\n");
		Token token = expect_token(TKN_IDENTIFIER);
		std::string dir(token.get_string());
		// if(dir == "load")
		if(!strcmp(token.get_string(), "load"))
			return parse_import_stmt();
		else
			report_error(m_ast, "directive '%s' is not supported\n\tOnly #load is supported", dir.c_str());
		return ast_bad_decl();
	}

	ast::AstNode*
	Parser::
	parse_import_stmt() {
		debug_print("parse import stmt\n");

		if(m_ast->currParent) {
			report_error(m_ast, "load stmt must be within file scope\n");
			return  ast_bad_decl();
		}

		// filename
		Token token = expect_token(TKN_LSTRING);
		std::cout << "Filename: " << token.get_string() << std::endl;
		ast::AstNode* id;

		if(allow_token(TKN_COL)) {
			if(check_token(TKN_IDENTIFIER))
				id = parse_identifier();
			else if(check_token(TKN_MULT)) {
				id = nullptr;
				next_token();
			}
			else {
				// TODO(Andrew): reword the error message
				report_error(m_ast, "expecting an identifeir or '*' after ':'\n");
			}
		}
		else {
			// handle getting the name of file
			std::string name = std::string(token.get_string());
		
			size_t indexp = name.find_last_of('.');
			size_t  indexs = name.find_last_of('/');
			if(indexs == std::string::npos)
				indexs = name.find_last_of('\\');
			// the file isnt in a sperate directory
			if(indexs == std::string::npos)
				indexs = 0;
			std::string id_str = name.substr(indexs, indexp - indexs);
			ast::Atom* atom = m_atomTable->insert(id_str);
			id = ast_ident(token, atom);
		}

		AstNodeList ids;
		if(allow_token(TKN_LBRACK)) {
			ids = parse_identifier_list();
			expect_token(TKN_RBRACK);
		}
		
		if(allow_token(TKN_IF)) {
			// not implementing at the moment
			report_error(m_ast, "conditional import are not implemented\n");
		}

		return ast_import_spec(token, sys::File::FullPath(std::string(token.get_string())), id, ids);
	}


	ast::AstNode*
	Parser::
	parse_identifier() {
		debug_print("parse id\n");
		Token token = expect_token(TKN_IDENTIFIER);
		const std::string& str = token.get_string();
		ast::Atom* atom;
		
		// check if it is the ignore id
		if(str.size() == 1 && str[0] == '_')
			atom = nullptr;
		else
			atom = m_atomTable->insert(str);

		return ast_ident(token, atom);
	}

	ast::AstNodeList
	Parser::
	parse_identifier_list() {
		debug_print("parse id list\n");
		AstNodeList list;
		do {
			add_node(list, parse_identifier());
		} while(allow_token(TKN_COM));
		return list;
	}


/*--------------------------- Error Reporting -------------------------------*/

	void report_error(AstFile* _file, const char* _msg, ...) {
		Location loc = _file->m_currToken.location();
		printf("%s\t%lu|%lu-%lu: ", _file->m_file->GetPath().c_str(), loc.m_line, loc.m_column, loc.m_ecolumn);
		printf("Error: ");
		va_list va;
		va_start(va, _msg);
		printf(_msg, va);
		++_file->m_errorCount;
	}

	void report_file_error(AstFile* _file, const char* _msg, ...) {
		va_list va;
		va_start(va, _msg);
		printf(_msg, va);
	}

	void report_warning(AstFile* _file, const char* _msg, ...) {
		Location loc = _file->m_currToken.location();
		printf("%s\t%lu|%lu-%lu: ", _file->m_file->GetPath().c_str(), loc.m_line, loc.m_column, loc.m_ecolumn);
		printf("Warning: ");
		va_list va;
		va_start(va, _msg);
		printf(_msg, va);
		++_file->m_warningCount;
	}

	void debug_print(const char* _msg, ...) {
		if(debug) {
			va_list va;
			va_start(va, _msg);
			printf(_msg, va);
		}
	}
} // parse