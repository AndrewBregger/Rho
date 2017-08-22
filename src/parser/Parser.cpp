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

namespace parse {


	void report_error(AstFile* _file, const char* _msg, ...);
	void report_file_error(AstFile* _file, const char* _msg, ...);
	void report_warning(AstFile* _file, const char* _msg, ...);
	void __debug_print__(const char* file, int line, const char* msg, ...);

 #define debug_print(msg, ...) __debug_print__(__FUNCTION__, __LINE__, msg, ##__VA_ARGS__)

#define check_token(type) __check_token(__FUNCTION__, __LINE__, type)
#define allow_token(type) __allow_token(__FUNCTION__, __LINE__, type)

	Parser::
	Parser(ast::AtomTable* table) {
		if(table)
			m_atomTable = table;
		else
			m_atomTable = new ast::AtomTable;
	}

	/// Scans the entire file into a stream of tokens.
	ParseFileError
	Parser::
	init(sys::File* _file) {
		ParseFileError err = ParseFile_None;
		m_ast = ast_file(_file);
		if(_file->GetError() != sys::FileError::File_None)
			return static_cast<ParseFileError>(_file->GetError());
		scanner::Scanner scanner(_file);
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


		if(m_ast->m_tokens.empty())
			return ParseFile_Empty;
		m_ast->m_currToken = m_ast->m_tokens[m_ast->m_tokenIndex];
		return err;
	}

	ast::AstFile*
	Parser::
	parse_file() {
		// get the base file
		const std::string& file_path = m_ast->m_file->GetFullPath();
		std::string base_dir(file_path.c_str());
		for(size_t i = base_dir.size() - 1; i >= 0; --i) {
			if(base_dir[i] == '/' || base_dir[i] == '\\')
				break;
			base_dir.pop_back();
		}
		m_ast->baseDir = base_dir;
		// parse a list of stmts, decls.
		m_ast->m_stmts = parse_stmt_list();

		// set up for symantic checking

		return m_ast;
	}

	ast::AstFile*
	Parser::
	parse_file(sys::File* _file) {
		ParseFileError err = init(_file);
		switch(err) {
			case ParseFile_Empty:
				report_file_error(m_ast, "File Error: '%.*s' was empty\n", _file->GetPath().size(), m_file->GetPath().c_str());
				return nullptr;
			case ParseFile_Permissions:
				report_file_error(m_ast, "File Error: '%.*s' wrong permissions on file\n", _file->GetPath().size(), m_file->GetPath().c_str());
				return nullptr;
			case ParseFile_WrongExtension:
				report_file_error(m_ast, "File Error: '%.*s' wrong extension\n", _file->GetPath().size(), _file->GetPath().c_str());
				return nullptr;
			case ParseFile_InvalidFile:
				report_file_error(m_ast, "File Error: '%.*s' file doen't exist\n", _file->GetPath().size(), _file->GetPath().c_str());
				return nullptr;
			case ParseFile_InvalidToken:
				report_file_error(m_ast, "File Error: '%.*s' has invalid token\n", _file->GetPath().size(), _file->GetPath().c_str());
				return nullptr;
			default:
				break;
		}

		AstFile* ast = parse_file();

		setup_decls();

		return ast;
	}

	bool
	Parser::
	setup_decls() {
		for(const auto& i : m_ast->m_stmts) {
			if(!i) // TODO(Andrew) have proper bad node handling
				continue;
			if(i->kind == AstEmptyStmt || i->kind == AstBadStmt) {
				// report error
				printf("Error: empty statement can not be in file scope\n");
			}
			if(i->kind == AstSpecStmt) {
				// cast
				Ast_Decl* decl = dynamic_cast<Ast_SpecStmt*>(i)->decl;
				m_ast->m_decls.push_back(decl);
			}
			else {
				// TODO(Andrew): have a proper error message
				std::cout << "Only Decls are allowed to be in file scope\n";
			}
		}
		for(const auto& i : m_ast->m_decls) {
			if(i->kind == AstImportSpec) {
				Ast_ImportSpec* decl = dynamic_cast<Ast_ImportSpec*>(i);
				m_ast->m_imports.push_back(decl);
			}
		}

		return true;
	}

	void
	Parser::
	next_token() {
		if(++m_ast->m_tokenIndex >= m_ast->m_tokens.size()) {
			exit(1);
		}
		m_ast->m_prevToken = m_ast->m_currToken;
		m_ast->m_currToken = m_ast->m_tokens.at(m_ast->m_tokenIndex);
		// debug_print("Next Token: %s\n", token::token_string(m_ast->m_currToken.token()).c_str());
	}

	Token
	Parser::
	peak_token(int _num) {
		assert(m_ast->m_tokenIndex + _num < m_ast->m_tokens.size());
		return m_ast->m_tokens[m_ast->m_tokenIndex + _num];
	}

	bool
	Parser::
	__check_token(const char* where, int line, token::Token_Type _type) {
		__debug_print__(where, line, "Checking Token: Checking for: %s ; got: %s - returning %s\n",
      token_string(_type).c_str(),
      token_string(m_ast->m_currToken.token()).c_str(),
      (m_ast->m_currToken.token() == _type ? "true" : "false"));
		return m_ast->m_currToken.token() == _type;
	}

	bool
	Parser::
	__allow_token(const char* where, int line, token::Token_Type _type) {
		__debug_print__(where, line, "Allow Token: Checking for: %s ; got: %s - ",
			token_string(_type).c_str(),
      token_string(m_ast->m_currToken.token()).c_str());

		if(m_ast->m_currToken.token() == _type) {
			debug_print("Token Found, Moving to next token\n");
			next_token();
			return true;
		}
		else {
			debug_print("Token not found, continuing\n");
			return false;
		}
	}


	token::Token
	Parser::
	expect_token(token::Token_Type _type) {
		debug_print("Expected: '%s'; Got: '%s'\n", token_string(_type).c_str(),
			token_string(m_ast->m_currToken.token()).c_str());

		Token prev = m_ast->m_currToken;
		if(prev.token() != _type) {
			// report error
			report_error(m_ast, "Expecting token '%s', found '%s'\n",
				token_string(_type).c_str(), token_string(prev.token()).c_str());
			synchronize();
			return Token(TKN_INVALID, Special, prev.location());
		}
		next_token();
		return prev;
	}

	bool
	Parser::
	expect_operator() {
		if(is_operator(m_ast->m_currToken.token())) {
			next_token();
			return true;
		}
		else {
			report_error(m_ast, "Expecting operator, found '%s'\n",
				token_string(m_ast->m_currToken.token()).c_str());
			// resolve error and continue parsing
			return false;
		}
	}

	bool
	Parser::
	expect_semicolon(const char* _where) {
		debug_print("Expecting Semicolon: got %s\n", token_string(m_ast->m_currToken.token()).c_str());
		if(m_ast->m_currToken.token() != TKN_SEM) {
			report_error(m_ast, "Expecting ';' after %s\n", _where);
			return false;
		}
		next_token();
		return true;
	}

	void
	Parser::
	synchronize() {
		while(1) {
			if(m_ast->m_prevToken.token() == TKN_SEM) {
				m_ast->m_currToken.print(1);
				return;
			}

			switch(m_ast->m_currToken.token()) {
				case TKN_EOF:
				case TKN_IF:
				case TKN_WHILE:
				case TKN_FOR:
				case TKN_ELIF:
				case TKN_BREAK:
				case TKN_CONTINUE:
				case TKN_RETURN:
					//m_ast->m_currToken.print(1);
					return;
				default:
					break;
			}
			next_token();
		}
	}

	ast::AstList<ast::Ast_Stmt*>
	Parser::
	parse_stmt_list() {
		debug_print("parse stmt list\n");
		AstList<Ast_Stmt*> stmts;
		while(m_ast->m_currToken.token() != TKN_RBRACK
			 && m_ast->m_currToken.token() != TKN_EOF) {
			auto st = parse_stmt();
			if(st)
				add_node(stmts, st);
		}
		return stmts;
	}

	ast::Ast_Stmt*
	Parser::
	parse_stmt(StmtFlags flags) {
		debug_print("parse stmt\n");
		Token token = m_ast->m_currToken;
		switch(token.token()) {
			case TKN_LET:
			case TKN_VAR:
			case TKN_TYPE:
			case TKN_IDENTIFIER:
			case TKN_LSTRING:
			case TKN_LCHAR:
			case TKN_LINT:
			case TKN_LDOUBLE:
			case TKN_LBOOL:
				return parse_simple_stmt(flags);
			case TKN_IF: return parse_if_stmt(TKN_IF);
			case TKN_WHILE: return parse_while_stmt();
			case TKN_FOR: return parse_for_stmt();
			case TKN_MATCH: return parse_match_stmt();
			case TKN_DEFER: return parse_defer_stmt();
			case TKN_HASH: return parse_hash_directive();
			case TKN_LBRACK: return parse_block_stmt();
			case TKN_RETURN: return parse_return_stmt();
			case TKN_BREAK: return new Ast_BreakStmt(token);
			case TKN_CONTINUE: return new Ast_ContinueStmt(token);
			case TKN_SEM: next_token(); return new Ast_EmptyStmt(token);
			case TKN_DELETE: {
				Token token = expect_token(TKN_DELETE);
				Ast_Expr* name = parse_rhs_expr();
				if(expect_semicolon("delete expression") && name)
					return new Ast_ExprStmt(new Ast_DeleteExpr(token, name));
				else {
					// @NOTE(Andrew): the next token should be the start of a new stmt
					// no need to have error correction
					return nullptr;
				}
			} break;
			default:
				break;
		}
		return nullptr;
	}

	ast::Ast_IfStmt*
	Parser::
	parse_if_stmt(token::Token_Type expected) {
		if(!m_ast->currParent) {
			// should parse the if statement.
			report_error(m_ast, "if statements must be within a function's scope, for now\n");
		}
		Token t = expect_token(expected);
		Ast_Expr* cond = parse_rhs_expr();
		if(!cond) {
			// same as previous error. It should impead parsing
			// correct_error();
			report_error(m_ast, "expecting expression after if\n");
		}

		Ast_Stmt* stmt = parse_stmt();
		Ast_Stmt* elseif = nullptr;
		switch(m_ast->m_currToken.token()) {
			case TKN_ELIF:
				elseif = parse_if_stmt(TKN_ELIF);
				break;
			case TKN_ELSE:
				next_token();
				elseif = parse_stmt();
				break;
			default:
				break;
		}
		return new Ast_IfStmt(t, cond, stmt, elseif);
	}

	ast::Ast_WhileStmt*
	Parser::
	parse_while_stmt() {
		if(!m_ast->currParent) {
			report_error(m_ast, "while statements must be within a function's scope\n");
		}
		Token token = expect_token(TKN_WHILE);
		Ast_Expr* cond = parse_rhs_expr();
		if(!cond)
			report_error(m_ast, "expecting expression after while\n");

		Ast_Stmt* body = parse_stmt();
		return new Ast_WhileStmt(token, cond, body);
	}

	ast::Ast_ForStmt*
	Parser::
	parse_for_stmt() {
		if(!m_ast->currParent) {
			report_error(m_ast, "for statements must be within a function's scope\n");
		}

		Token token = expect_token(TKN_FOR);

		Ast_Expr* init = parse_simple_stmt(StmtNoSemi);
		// m_ast->m_currToken.print(1);
		expect_semicolon("initization statement");
		Ast_Expr* cond = parse_expr(false);

		expect_semicolon("conditional statement");
		Ast_Expr* step = parse_simple_stmt(StmtNoSemi);

		Ast_Stmt* body = parse_stmt();

		return new Ast_ForStmt(token, init, cond, step, body);
	}

	ast::Ast_Stmt*
	Parser::
	parse_match_stmt() {
		if(!m_ast->currParent) {
			report_error(m_ast, "match statements must be within a function's scope\n");
		}
		report_error(m_ast, "match statements not implemented at the moment\n");
		return nullptr;
	}

	ast::Ast_DeferStmt*
	Parser::
	parse_defer_stmt() {
		if(!m_ast->currParent) {
			report_error(m_ast, "defer statements must be within a function's scope\n");
		}
		Token token = expect_token(TKN_DEFER);
		Ast_Stmt* stmt = parse_stmt();

		// there should be some checking here.
		// but i will do it later.
		// TODO(Andrew): correctness checking.
		return new Ast_DeferStmt(token, stmt);
	}

	ast::Ast_ReturnStmt*
	Parser::
	parse_return_stmt() {
		if(!m_ast->currParent) {
			report_error(m_ast, "return statements must be within a function's scope\n");
		}
		Token token = expect_token(TKN_RETURN);
		AstList<Ast_Expr*> exprs = parse_rhs_expr_list();
		expect_semicolon("return statement");
		return new Ast_ReturnStmt(token, exprs);
	}

	ast::Ast_Decl*
	Parser::
	parse_decl() {
		debug_print("parse decl expr\n");
		Ast_Decl* decl = nullptr;
		switch(m_ast->m_currToken.token()) {
			case TKN_LET:
			case TKN_VAR:
				decl = parse_variable_decl();
				break;
			case TKN_TYPE: {
				expect_token(TKN_TYPE);
				Ast_Identifier* alias = parse_identifier();
				if(allow_token(TKN_ASN)) {
					Ast_Type* type = parse_type_or_ident();
					return new Ast_TypeAlias(alias, type);
				}
				else {
					report_error(m_ast, "expecting an '=' followed by the aliased type\n");
					synchronize();
					return nullptr; // ast_bad_decl();
				}
			}
			case TKN_IDENTIFIER: {
				Ast_Identifier * id = parse_identifier();
				expect_token(TKN_DCOL);
				switch(m_ast->m_currToken.token()) {
					case TKN_STRUCT:
					case TKN_CLASS:
					case TKN_ENUM:
					case TKN_UNION:
						decl = new Ast_TypeSpec(parse_type_decl(id));
						break;
					default: {
						decl = parse_function_decl(id);
						if(!decl) {
							report_error(m_ast, "Expecting struct, class, or function/method declaration\n");
							return nullptr; //ast_bad_decl();
						}
					}
				}
			} break;
			default:
				break;
		}
		return decl;
	}

	ast::Ast_Stmt*
	Parser::
	parse_simple_stmt(StmtFlags flags) {
		Token t = m_ast->m_currToken;
		switch(t.token()) {
			case TKN_TYPE:
			case TKN_VAR:
			case TKN_LET: {
				// these stmt will always be variable decls
				Ast_Decl* stmt = parse_decl();
				if(flags & StmtExpectSemi) {
					if(expect_semicolon("variable declaration"))
						return new Ast_SpecStmt(stmt);
					return nullptr; //ast_bad_decl();
				}
				else
					return new Ast_SpecStmt(stmt);
			}
			default:
				break;
		}
		if(t.token() == TKN_IDENTIFIER)
				if(peak_token().token() == TKN_DCOL)
					return new Ast_SpecStmt(parse_decl());

		AstList<Ast_Expr*> lhs = parse_lhs_expr_list();
		t = m_ast->m_currToken;
		switch( t.token()) {
			case TKN_IDENTIFIER:
			case TKN_ASN:
			case TKN_DECL:
			case TKN_ADDASN:
			case TKN_SUBASN:
			case TKN_MULTASN:
			case TKN_DIVASN:
			case TKN_REMASN:
			case TKN_XORASN:
			case TKN_ANDASN:
			case TKN_ORASN:
			case TKN_SHTLASN:
			case TKN_SHTRASN: {
				if(m_ast->currParent == nullptr) {
					report_error(m_ast, "explain error\n");
					return nullptr; //new Ast_BadNode(AstBadDecl);
				}
				else {
					next_token();
					AstList<Ast_Expr*> rhs = parse_rhs_expr_list();
					if(rhs.size() == 0) {
						report_error(m_ast, "expecting expression on right side of equal expression\n");
						return nullptr; //new Ast_BadNode(AstBadStmt);
					}
					if(flags & StmtExpectSemi) {
						if(expect_semicolon("assignment statement")) {
							return nullptr; //new Ast_AssignmentStmt(t, lhs, rhs);
						}
						return nullptr; //new Ast_BadNode(AstBadStmt);
					}
					else
						return new Ast_AssignmentStmt(t, lhs, rhs);
				}
			} break;
			default:
				break;
		}

		if(lhs.size() > 1)
			report_error(m_ast, "expecting only one expression in statement\n");

		if(expect_semicolon("expression statement"))
			return new Ast_ExprStmt(lhs[0]);
		else
			return nullptr; //new Ast_BadNode(AstBadStmt);
	}

	ast::Ast_VariableSpec*
	Parser::
	parse_variable_decl() {
		Token token = m_ast->m_currToken;
		// token.(0);
		next_token();
		AstList<Ast_Identifier*> ids = parse_identifier_list();
		AstList<Ast_Expr*> values;
		Ast_Type* type = nullptr;

		if(allow_token(TKN_COL)) {
			type = parse_type_or_ident();
		}
		if(allow_token(TKN_ASN)) {
			values = parse_rhs_expr_list();
		}
		if(!type && values.empty()) {
			// go to next stmt
			report_error(m_ast, "type must be specified or initialized\n");
			synchronize();
			return nullptr; //new Ast_BadNode(AstBadDecl);
		}

		Ast_VariableSpec* var = new Ast_VariableSpec(token, ids, values, type);
		for(auto id : ids){
			if(!id->name) {
				var->flags |= VarHasIgnore;
				break;
			}
		}
		if(!type)
			var->flags |= VarInferType;
		if(token.token() == TKN_LET) {
			var->flags |= VarConst;
			if(values.empty()) {
				report_error(m_ast, "constant variables must initialized when declared\n");
				synchronize();
				return nullptr; //new Ast_BadNode(AstBaDecl);
			}
		}
		return var;
	}

	ast::Ast_BlockStmt*
	Parser::
	parse_body() {
		return parse_block_stmt();
	}

	ast::Ast_BlockStmt*
	Parser::
	parse_block_stmt() {
		expect_token(TKN_LBRACK);
		AstList<Ast_Stmt*> stmts = parse_stmt_list();
		expect_token(TKN_RBRACK);
		return new Ast_BlockStmt(stmts);
	}

	ast::Ast_Type*
	Parser::
	parse_type_decl(Ast_Identifier* id) {
		Token token = m_ast->m_currToken;
		Ast_Type* type;
		next_token();
		switch(token.token()) {
			case TKN_CLASS:
			case TKN_STRUCT: {
				AstList<Ast_Type*> extends;
				bool oldClassDecl = m_ast->inClassDecl;
				if(token.token() == TKN_CLASS) {
					// parse inheritend types

					m_ast->inClassDecl = true;
				}
				expect_token(TKN_LBRACK);
				AstList<Ast_FieldSpec*> members;
				AstList<Ast_Decl*> decls;

				parse_type_body(decls, members);

				if(token.token() == TKN_CLASS)
					type = new Ast_ClassType(token, id, members, extends, AstList<Ast_ProcSpec*>(), decls);
				else
					type = new Ast_StructType(token, id, members, decls);

				if(token.token() == TKN_CLASS)
					m_ast->inClassDecl = oldClassDecl;

			} break;
			case TKN_ENUM: {
				// expect_token(TKN_ENUM);
				AstList<Ast_Decl*> members;
				if(!allow_token(TKN_LBRACK))
						report_error(m_ast, "expecting a block to define a structure\n");
				if(check_token(TKN_IDENTIFIER)) {
					if(peak_token().token() == TKN_COM ||
						 peak_token().token() == TKN_ASN) {

					}
				}
				expect_token(TKN_RBRACK);
				type = new Ast_EnumType(token, id, members);
			} break;
			case TKN_UNION: {
				// Token token = expect_token(TKN_UNION);
				if(!allow_token(TKN_LBRACK))
						report_error(m_ast, "expecting a block to define a structure\n");


			} break;
			default:
				report_error(m_ast, "Unexpected declaration token: '%s'\n", token_string(token.token()).c_str());
		}

		return type;
	}

	void
	Parser::
	parse_type_body(AstList<Ast_Decl*>& sub,
		AstList<Ast_FieldSpec*>& fields) {
		Ast_FieldSpec* field = parse_field(
		static_cast<FieldFlags>(
			(m_ast->inClassDecl? (FieldAllowView | FieldAllowDefault): FieldAllowDefault)
		));
	}

	ast::Ast_TypeSpec*
	Parser::
	parse_enum_union_struct() {
		Ast_Identifier* id = parse_identifier();
		AstList<Ast_FieldSpec*> list;
		if(allow_token(TKN_SEM)) {
			expect_token(TKN_STRUCT);
			do {
				Ast_FieldSpec* field = parse_field();
				field = parse_variable_tags(field, FieldNoDefault | FieldNoView);
				add_node(list, field);
				if(!expect_semicolon("struct member"))
					synchronize();
			} while(allow_token(TKN_RBRACK));
		}
		else {
			report_error(m_ast, "expecting semicolon when declaring a structure in a union or enum\n");
			synchronize();
			return nullptr;
		}
		Ast_StructType* s = new Ast_StructType(id->token(), id, list, AstList<Ast_Decl*>());
		return new Ast_TypeSpec(s);
	}

	ast::Ast_ProcSpec*
	Parser::
	parse_function_decl(Ast_Identifier* id) {
		bool isConst;
		Ast_Type* reciever = parse_function_reciever(isConst);
		Ast_ProcType* type = parse_function_type(id, reciever, isConst);

		// if(!type) return nullptr;
		// type = nullptr; //new Ast_Type(AstBadType);

		Ast_Stmt* body;
		Ast_ProcSpec* funct = new Ast_ProcSpec(id->token(), type, nullptr);
		Ast_Node* oldParent = m_ast->currParent;
		m_ast->currParent = funct;

		if(check_token(TKN_LBRACK)) {
			body = parse_body();
		}

		if(!body) {
			report_error(m_ast, "a function body must be declared when declaring a function\n");
		}
		funct->body = body;
		m_ast->currParent = oldParent;
		return funct;
	}

	ast::Ast_ProcType*
	Parser::
	parse_function_type(ast::Ast_Identifier* id, ast::Ast_Type* reciever, bool isConst) {
		AstList<Ast_ProcReturn*> returns;
		AstList<Ast_FieldSpec*> params;
		expect_token(TKN_LPAREN);
		if(check_token(TKN_IDENTIFIER))
			params = parse_field_list(FieldAllowDefault);
		expect_token(TKN_RPAREN);
    // debug_print("Function Type: num params: %d\n", params.size());


		Ast_ProcType* type;
		if(check_token(TKN_RARR))
			returns = parse_function_return();
		// debug_print("Function Type: num returns: %d\n", returns.size());
		if(reciever)
			type = new Ast_MethodType(id->token(), id, reciever, params, returns, 0);
		else
			type = new Ast_FunctionType(id->token(), id, params, returns, 0);
		if(reciever) {
			type->flags |= FunctMember;
			if(isConst)
				type->flags |= FunctConst;
		}
		type = parse_function_tags(type);

		return type;
	}

	// @note(Andrew): Should return have a special
	// Ast Node?
	ast::AstList<Ast_ProcReturn*>
	Parser::
	parse_function_return() {
		// AstList<Ast_Node*> list;
		AstList<Ast_ProcReturn*> list;
		expect_token(TKN_RARR);
		bool expectRParen{false};
		if(allow_token(TKN_LPAREN))
			expectRParen = true;
		if(check_token(TKN_IDENTIFIER) || m_ast->m_currToken.type() == PrimativeType) {
			int counter = 1;
			bool foundCol = false;
			Token t;
			// NOTE(Andrew): this is temporary
			// TODO(Andrew): Fix this
			while((t = peak_token(counter)).token() != TKN_LBRACK) {
				if(t.token() == TKN_COL)
					foundCol = true;
				counter++;
			}
			if(foundCol) {
				// named returns
				// debug_print("Parsing return field list\n");
				auto temp = parse_field_list();
				for(auto t : temp)
					list.push_back(new Ast_NamedReturn(t));
			}
			else {
				// not named returns
				// debug_print("Parsing return type list\n");
				do {
					list.push_back(new Ast_TypedReturn(parse_type()));
				} while(allow_token(TKN_COM));
			}
		}
		else {
			report_error(m_ast, "error parsing return types\n");
		}

		if(expectRParen)
			expect_token(TKN_RPAREN);

		return list;
	}

	ast::Ast_Type*
	Parser::
	parse_function_reciever(bool& isConst) {
		if(!allow_token(TKN_MULT))
			isConst = true;

		if(check_token(TKN_IDENTIFIER)) {
			return parse_type_or_ident();
		}
		else
			return nullptr;
	}

	ast::Ast_Type*
	Parser::
	parse_type_or_ident() {
		if(m_ast->m_currToken.type() == PrimativeType) {
			Token temp = m_ast->m_currToken;
			next_token();
			return new Ast_PrimativeType(temp);
		}
		switch(m_ast->m_currToken.token()) {
			case TKN_IDENTIFIER: {
				// same as selector; however, i only want identifiers
				// no other expressions.

				Ast_Identifier* s = parse_identifier();
				Ast_Expr* ss = new Ast_Operand(s);
				while(allow_token(TKN_PER)) {
					Ast_Operand* expr = new Ast_Operand(parse_identifier());
					ss = new Ast_SelectorExpr(ss, expr);
				}
				return new Ast_NamedType(ss);
			}
			case TKN_MULT: {
				Token token = expect_token(TKN_MULT);
				return new Ast_PointerType(token, parse_type_or_ident());
			}
			case TKN_LBRACE: {
				// parse arrays and map types
				// [][] | [..] arrays
				// [Type, Type] maps
				Token begin = expect_token(TKN_LBRACE);
				if(allow_token(TKN_DPER)) {
					expect_token(TKN_RBRACE);
					Ast_Type* type = parse_type_or_ident();
					return new Ast_DynamicArrayType(begin, type);
				}
				else {
					// TODO(Andrew): Find a better way to parse maps, this is bad!!!!
					Token curr = m_ast->m_currToken;
					bool isMap = false;
					int counter = 1;
					while(curr.token() != TKN_RBRACE) {
						if(curr.token() == TKN_COM) {
							isMap = true;
							break;
						}
						curr = peak_token(counter++);
					}
					if(isMap) {
						// parse map
						Ast_Type* key,* value;

						key = parse_type_or_ident();
						expect_token(TKN_COM);
						value = parse_type_or_ident();

						expect_token(TKN_RBRACE);
						return new Ast_MapType(begin, key, value);
					}
					else {
						// parse array
						Ast_Expr* expr = parse_expr(false);
						expect_token(TKN_RBRACE);
						Ast_Type* type = parse_type_or_ident();
						if(!type)
							report_error(m_ast, "array type must be specified\n");
						return new Ast_ArrayType(begin, type, expr);
					}
				}
			}
			// function type parsed here should only be pure functions, not methods.
			case TKN_LPAREN:
				return parse_function_type(nullptr, nullptr, false);
			case TKN_CLASS:
			case TKN_STRUCT:
			case TKN_ENUM:
			case TKN_UNION:
				return parse_type_decl(nullptr);
			default:
				break;
		}
		// no valid type found
		debug_print("return nullptr\n");
		return nullptr;
	}

	ast::Ast_Type*
	Parser::
	parse_type() {
		return parse_type_or_ident();
	}

	ast::Ast_FieldSpec*
	Parser::
	parse_field(FieldFlags flags) {
		AstList<Ast_Identifier*> name = parse_identifier_list();
		expect_token(TKN_COL);
		Ast_Type* type = parse_type_or_ident();

		Ast_FieldSpec* field = new Ast_FieldSpec(name.front()->token(), name, type);
		field = parse_variable_tags(field, flags);
		return field;
	}

	ast::AstList<Ast_FieldSpec*>
	Parser::
	parse_field_list(FieldFlags flags) {
		AstList<Ast_FieldSpec*> list;
		do {
			auto e = parse_field(flags);
			add_node(list, e);
		} while(allow_token(TKN_COM));
		return list;
	}

	ast::Ast_Expr*
	Parser::
	parse_operand(bool _lhs) {
		// AstNode* operand = nullptr;
		Token token = m_ast->m_currToken;

		if(is_literal(token.token())) {
			next_token();
			return new Ast_BasicLiteral(token);
		}
    switch(token.token()) {
			case TKN_TRUE:
			case TKN_FALSE:
				next_token();
				return new Ast_BasicLiteral(token);
			case TKN_NULL:
				next_token();
				return new Ast_NullLiteral(token);
      case TKN_IDENTIFIER:
			  return new Ast_Operand(parse_identifier());
		  case TKN_LPAREN: {
        Token begin = expect_token(TKN_LPAREN);
        Ast_Expr* expr = parse_expr(false);
        Token end = expect_token(TKN_RPAREN);
        return new Ast_ParenExpr(begin, end, expr);
      }
      case TKN_MULT: {
        Token op = expect_token(TKN_MULT);
        // paybe this should be 'parse_expr(_lhs)'
        Ast_Expr* epxr = parse_operand(_lhs);
        return new Ast_DerefExpr(op, epxr);
      }
      case TKN_AND: {
        Token op = expect_token(TKN_AND);
        Ast_Expr* epxr = parse_operand(_lhs);
        return new Ast_AddressExpr(op, epxr);
      }
      case TKN_LBRACK: {
      	// Token token = expect_token(TKN_LBRACK);
      	return parse_compound_literal();
      }
      default:
        break;
		}
		return nullptr;
	}

	ast::Ast_Expr*
	Parser::
	parse_expr(bool _lhs) {
		if(_lhs)
			return parse_lhs_expr();
		else
			return parse_rhs_expr();
	}

	ast::Ast_Expr*
	Parser::
	parse_lhs_expr() {
		return parse_binary_expr(true, 1);
	}

	ast::Ast_Expr*
	Parser::
	parse_rhs_expr() {
  	return parse_binary_expr(false, 1);
	}

	ast::AstList<Ast_Expr*>
	Parser::
	parse_lhs_expr_list() {
		ast::AstList<Ast_Expr*> list;
		do {
			// this check is probably unnessesary
			auto e = parse_lhs_expr();
			if(e)
				add_node(list, e);
		} while(allow_token(TKN_COM));

		return list;
	}

	ast::AstList<Ast_Expr*>
	Parser::
	parse_rhs_expr_list() {
		ast::AstList<Ast_Expr*> list;
		do {
			// this check is probably unnessesary
			auto e = parse_rhs_expr();
			if(e)
				add_node(list, e);
		} while(allow_token(TKN_COM));

		return list;
	}

	ast::Ast_Expr*
	Parser::
	parse_primary_expr(bool _lhs) {
		Ast_Expr* operand = parse_operand(_lhs);
		if(!operand) return operand;
		bool loop = true;
		while(loop) {
			Token token = m_ast->m_currToken;
			switch(token.token()) {
				case TKN_LPAREN:
					// function;
					operand = parse_call_expr(operand);
					break;
				case TKN_LBRACE:
					// index
					operand = parse_index_expr(operand);
					break;
				case TKN_PER:
					// selector expr
					next_token();
					operand = new Ast_SelectorExpr(operand, parse_primary_expr(_lhs));
					break;
				default:
					loop = false;
					break;
			}
		}
		return operand;
	}


	ast::Ast_Expr*
	Parser::parse_compound_literal() {
		Token token = expect_token(TKN_LBRACK);
		AstList<Ast_Expr*> lits;
		do {
			if(check_token(TKN_RBRACK))
				break;
			add_node(lits, parse_element());
		} while(allow_token(TKN_COM));
		expect_token(TKN_RBRACK);
		return new Ast_CompoundLiteral(token, lits);
	}

	ast::Ast_Expr*
	Parser::parse_element() {
		if(check_token(TKN_LBRACK))
			return parse_compound_literal();
		return parse_rhs_expr();
	}

	// ast::Ast_Expr*
	// Parser::parse_element_list() {

	// }

  int operator_precedence(Token_Type _type) {
    switch(_type) {
    	case TKN_NEW:
    		return 12;
    	case TKN_DIV:
      case TKN_REM:
      case TKN_POW:
      case TKN_MULT:
      	return 11;
      case TKN_ADD:
      case TKN_SUB:
      	return 10;
      case TKN_SHTL:
      case TKN_SHTR:
     		return 9;
      case TKN_LESS:
      case TKN_GRET:
      case TKN_LEQ:
      case TKN_GEQ:
      	return 8;
      case TKN_EQ:
      case TKN_NEQ:
      	return 7;
      case TKN_AND:
      	return 6;
      case TKN_XOR:
      	return 5;
      case TKN_OR:
      	return 4;
      case TKN_LITAND:
      	return 3;
      case TKN_LITOR:
      	return 2;
      case TKN_QEST:
			case TKN_ELS:
			case TKN_DPER:
				return 1;
			case TKN_ASN:
			case TKN_DECL:
			case TKN_ADDASN:
			case TKN_SUBASN:
			case TKN_MULTASN:
			case TKN_DIVASN:
			case TKN_REMASN:
			case TKN_XORASN:
			case TKN_ANDASN:
			case TKN_ORASN:
			case TKN_SHTLASN:
			case TKN_SHTRASN:
				return 0;
			default:
				return 0;
    }
  }

	ast::Ast_Expr*
	Parser::
	parse_binary_expr(bool _lhs, int _prec_in) {
  	Ast_Expr* expr = parse_unary_expr(_lhs);
  	for(int prec = operator_precedence(m_ast->m_currToken.token()); prec >= _prec_in; --prec) {
  		for(;;) {
  			Token op = m_ast->m_currToken;
  			int op_prec = operator_precedence(op.token());
  			if(op_prec != prec)
  				break;
  			if(expect_operator()) {
  				Ast_Expr* rhs = parse_binary_expr(false, prec + 1);
  				expr = new Ast_BinaryExpr(op, expr, rhs);
  			}
  			else
  				return nullptr; // new Ast_BadNode(AstBadExpr);
  		}
  	}
  	return expr;
	}

	ast::Ast_Expr*
	Parser::
	parse_unary_expr(bool _lhs) {
		Token token = m_ast->m_currToken;
		switch(token.token()) {
			case TKN_NOT:
			case TKN_SUB:
			case TKN_BNOT:
				next_token();
				return new Ast_UnaryExpr(token, parse_unary_expr(_lhs));
			case TKN_NEW: {
				Token token = expect_token(TKN_NEW);
				Ast_Type* type = parse_type_or_ident();
				// for now thats all that is implemeneted
				AstList<Ast_Expr*> expr;
				if(allow_token(TKN_LPAREN)) {
					expr = parse_rhs_expr_list();
					expect_token(TKN_RPAREN);
				}

				return new Ast_NewExpr(token, type, expr);
			} break;
			default:
				break;
		}
		return parse_primary_expr(_lhs);
	}

	ast::Ast_IndexExpr*
	Parser::
	parse_index_expr(Ast_Expr* operand) {
		// TODO(Andrew | Jonathan): handle slices
		Token begin = expect_token(TKN_LBRACE);
		Ast_Expr* expr = parse_expr(false);
		Token end = expect_token(TKN_RBRACE);
		return new Ast_IndexExpr(begin, end, operand, expr);
	}

	ast::Ast_FuncCall*
	Parser::
	parse_call_expr(Ast_Expr* operand) {
		// opening of the parens
		expect_token(TKN_LPAREN);
		AstList<Ast_Expr*> actuals = parse_rhs_expr_list();
		// custom expr list parser
		expect_token(TKN_RPAREN);
		return new Ast_FuncCall(operand, nullptr, actuals);
	}

	ast::Ast_Stmt*
	Parser::
	parse_hash_directive() {
		debug_print("parse hash directive\n");
		expect_token(TKN_HASH);
		Token token = expect_token(TKN_IDENTIFIER);
		std::string dir(token.get_string());
		// if(dir == "load")
		if(!strcmp(token.get_string(), "load"))
			return new Ast_SpecStmt(parse_import_stmt());
		else
			report_error(m_ast, "directive '%s' is not supported\n\tOnly #load is supported", dir.c_str());
		return nullptr; // new Ast_BadNode(AstBadStmt);
	}

	ast::Ast_ImportSpec*
	Parser::
	parse_import_stmt() {
		debug_print("parse import stmt\n");

		if(m_ast->currParent) {
			report_error(m_ast, "load stmt must be within file scope\n");
			return nullptr;// new Ast_BadNode(AstBadDecl);
		}

		// filename
		Token token = expect_token(TKN_LSTRING);

		ast::Ast_Identifier* id;
		if(allow_token(TKN_COL)) {
			if(check_token(TKN_IDENTIFIER))
				id = parse_identifier();
			else if(check_token(TKN_MULT)) {
				id = nullptr;
				next_token();
			}
			else {
				report_error(m_ast, "expecting an identifeir or '*' after ':'\n");
			}
		}
		else {
			// handle getting the name of file
			std::string name = std::string(token.get_string());

			size_t indexp = name.find_last_of('.');
			size_t indexs = name.find_last_of('/');
			if(indexs == std::string::npos)
				indexs = name.find_last_of('\\');
			// the file isnt in a sperate directory
			if(indexs == std::string::npos)
				indexs = 0;
			std::string id_str = name.substr(indexs, indexp - indexs);
			ast::Atom* atom = m_atomTable->insert(id_str);
			id = new Ast_Identifier(token, atom);
		}

		AstList<Ast_Identifier*> ids;
		if(allow_token(TKN_LBRACK)) {
			ids = parse_identifier_list();
			expect_token(TKN_RBRACK);
		}

		if(allow_token(TKN_IF)) {
			// not implementing at the moment
			report_error(m_ast, "conditional import are not implemented\n");
			synchronize();
		}
		std::string path = m_ast->baseDir + std::string(token.get_string());
		std::string fullPath = sys::File::FullPath(path);
		auto import =  new Ast_ImportSpec(token, fullPath, id, ids);
		if(!fullPath.empty())
			import->valid = true;
		return import;
	}

/*--------------------------- Identifier Parsing -------------------------------*/

	ast::Ast_Identifier*
	Parser::
	parse_identifier() {
		debug_print("parse id\n");
		Token token = expect_token(TKN_IDENTIFIER);

		if(token.token() == TKN_INVALID) {
			return nullptr;
		}
		const std::string& str = token.get_string();
		ast::Atom* atom;
		// check if it is the ignore id
		if(str.size() == 1 && str[0] == '_')
			atom = nullptr;
		else
			atom = m_atomTable->insert(str);

		return new Ast_Identifier(token, atom);
	}

	ast::AstList<Ast_Identifier*>
	Parser::
	parse_identifier_list() {
		debug_print("parse id list\n");
		ast::AstList<Ast_Identifier*> list;
		do {
			add_node(list, parse_identifier());
		} while(allow_token(TKN_COM));
		return list;
	}

/*--------------------------- Tag Parsing -------------------------------*/

/*
ast::AstNode*
Parser::
parse_field_tags(ast::AstNode* field) {
	if(!m_ast->inClassDecl && m_ast->m_currToken.token() != TKN_COM)
		report_error(m_ast, "function parameters are not permmited to have tags\n");
	return field;
}
*/

ast::Ast_ProcType*
Parser::
parse_function_tags(ast::Ast_ProcType* funct) {
	return funct;
}

ast::Ast_FieldSpec*
Parser::
parse_variable_tags(ast::Ast_FieldSpec* var, FieldFlags flags) {
	for(;;) {
		if(flags & FieldAllowDefault) {

		}
		else {

		}
	}
	return var;
}

/*--------------------------- Error Reporting -------------------------------*/

void print_spaces(int spaces) {
	std::string temp;
	while(--spaces)
		temp += ' ';
	std::cout << temp;
}

	void report_error(AstFile* _file, const char* _msg, ...) {
		Location loc = _file->m_currToken.location();
		printf("%s\t%lu|%lu-%lu: ", _file->m_file->GetPath().c_str(), loc.m_line, loc.m_column, loc.m_ecolumn);
		printf("Error: ");
		va_list va;
		va_start(va, _msg);
		vprintf(_msg, va);
		printf("%zu\t%s", loc.m_line, _file->m_file->GetLine(loc.m_line).c_str());
		printf("\t");
		print_spaces(loc.m_column);
		printf("^\n");
		++_file->m_errorCount;
	}

	void report_file_error(AstFile* _file, const char* _msg, ...) {
		va_list va;
		va_start(va, _msg);
		vprintf(_msg, va);
	}

	void report_warning(AstFile* _file, const char* _msg, ...) {
		Location loc = _file->m_currToken.location();
		printf("%s\t%lu|%lu-%lu: ", _file->m_file->GetPath().c_str(), loc.m_line, loc.m_column, loc.m_ecolumn);
		printf("Warning: ");
		va_list va;
		va_start(va, _msg);
		vprintf(_msg, va);
		++_file->m_warningCount;
	}


	void __debug_print__(const char* file, int line, const char* _msg, ...) {
		if(debug) {
      printf("%s: %d| ", file, line);
			va_list va;
			va_start(va, _msg);
			vprintf(_msg, va);
			va_end(va);
		}
	}
} // parse
