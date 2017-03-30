#include "Parser.h"
#include "utils/File.h"
#include "ast/AstDecl.h"
#include <iostream>
using namespace ast;
using namespace parse;
using namespace scanner;
using namespace token;

namespace parse {

	Parser::
	Parser(sys::File* _file) {
		m_scanner 		 	= new Scanner(_file);
		m_currentToken 	= m_scanner->scan();
		m_nextToken		 	= m_scanner->peak();
		m_scope  	 			= new Scope(File);
		m_file 					= _file;
		m_atomTable 		= new ast::AtomTable;
	}

	Token*
	Parser::
	consume_token() {
		delete m_previousToken;
		m_previousToken = m_currentToken;
		m_currentToken = m_nextToken;
		m_nextToken = m_scanner->peak();
		return m_currentToken;
	}	

	Scope* 
	Parser::
	get_scope() {
		return m_scope;
	}
	AstFile*
	Parser::
	parse_file() {
		AstFile* file = new AstFile(m_file);
		while(m_currentToken->token() != TKN_ERROR) {
			auto astDecl = parse_decl();
			file->AddDecl(astDecl);
		}

		return file;
	}
	
	void
	Parser::
	begin_scope(ScopeKind _kind) {
		m_scope = new Scope(this, _kind);
		++m_depth;	
	}	

	void
	Parser::
	end_scope() {
		m_scope = m_scope->Parent();
		--m_depth;
	}

	size_t
	Parser::
	depth() {
		return m_depth;
	}

	ast::decl::AstDecl*
	Parser::
	parse_decl() {
		switch(m_currentToken->token()) {
			case TKN_LOAD:
				consume_token();
				return parse_load();
			case TKN_IDENTIFIER:
				break;
			default:
				std::cout << "error:\n";
		}
		return nullptr;
	}

	ast::decl::AstLoad*
	Parser::
	parse_load() {
		return nullptr;
	}



} // parse