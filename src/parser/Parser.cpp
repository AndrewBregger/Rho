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


		if(m_ast->m_tokens.empty())
			return ParseFile_Empty;
		m_ast->m_currToken = m_ast->m_tokens[m_ast->m_tokenIndex];

		// for(Token& t : m_ast->m_tokens) {
		// 	t.print();
		// }
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
			// handle better
			// std::cout << "Shouldnt happend\n";
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
		if(m_ast->m_currToken.token() != TKN_SEM) {
			report_error(m_ast, "Expecting ';' after %s\n", _where);
			return false;
		}
		next_token();
		return true;
	}

	ast::AstNodeList
	Parser::
	parse_stmt_list() {
		debug_print("parse stmt list\n");
		AstNodeList stmts;
		while(m_ast->m_currToken.token() != TKN_RBRACK
			 && m_ast->m_currToken.token() != TKN_EOF) {
			auto st = parse_stmt();
			if(st)
				add_node(stmts, st);
		}
		return stmts;
	}

	ast::AstNode*
	Parser::
	parse_stmt() {
		debug_print("parse stmt\n");
		Token token = m_ast->m_currToken;
		switch(token.token()) {
			case TKN_LET:
			case TKN_VAR:
			case TKN_IDENTIFIER:
			case TKN_LSTRING:
			case TKN_LCHAR:
			case TKN_LINT:
			case TKN_LDOUBLE:
			case TKN_LBOOL:
				return parse_simple_stmt();
			case TKN_IF: return parse_if_stmt();
			case TKN_WHILE: return parse_while_stmt();
			case TKN_FOR: return parse_for_stmt();
			case TKN_MATCH: return parse_match_stmt();
			case TKN_DEFER: return parse_defer_stmt();
			case TKN_HASH: return parse_hash_directive();
			default:
				break;
		}
		return nullptr;
	}

	ast::AstNode*
	Parser::
	parse_if_stmt() {
		return nullptr;
	}

	ast::AstNode*
	Parser::
	parse_while_stmt() {
		return nullptr;
	}

	ast::AstNode*
	Parser::
	parse_for_stmt() {
		return nullptr;
	}

	ast::AstNode*
	Parser::
	parse_match_stmt() {
		return nullptr;
	}

	ast::AstNode*
	Parser::
	parse_defer_stmt() {
		return nullptr;
	}

	ast::AstNode*
	Parser::
	parse_decl() {
		debug_print("parse decl expr\n");
		AstNode* decl;
		switch(m_ast->m_currToken.token()) {
			case TKN_LET:
			case TKN_VAR:
				decl = parse_variable_decl();
				expect_semicolon("variable declaration");
				break;
			case TKN_IDENTIFIER: {
				AstNode* id = parse_identifier();
				expect_token(TKN_DCOL);
				switch(m_ast->m_currToken.token()) {
					case TKN_STRUCT:
					case TKN_CLASS:
						decl = parse_type_decl(id);
						break;
					default: {
						decl = parse_function_decl(id);
						if(!decl) {
							report_error(m_ast, "Expecting struct, class, or function/method declaration\n");
							return ast_bad_decl();
						}
					}
				}
			} break;
			default:
				report_error(m_ast, "Unexpected token in declaration\n");
				return ast_bad_decl();
		}
		if(decl)
			m_ast->m_decls.push_back(decl);
		return decl;
	}

	ast::AstNode*
	Parser::
	parse_simple_stmt() {
		Token t = m_ast->m_currToken;
		switch(t.token()) {
			case TKN_VAR:
			case TKN_LET:
				return parse_decl();
			default:
				break;
		}
		if(t.token() == TKN_IDENTIFIER)
				if(peak_token().token() == TKN_DCOL)
					return parse_decl();

		AstNodeList lhs = parse_lhs_expr_list();
		t = m_ast->m_currToken;
		next_token();
		switch( t.token()) {
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
					return ast_bad_stmt();
				}
				else {
					AstNodeList rhs = parse_rhs_expr_list();
					if(rhs.size() == 0) {
						report_error(m_ast, "expecting expression on right side of equal expression\n");
						return ast_bad_stmt();
					}
					if(expect_semicolon("assignment statement")) {
						return ast_assign_stmt(t, lhs, rhs);
					}
					return ast_bad_stmt();
				}
			} break;
			default:
				report_error(m_ast, "Unexpected token '%s'\n", token::token_string(m_ast->m_currToken.token()).c_str());
				return ast_bad_stmt();
		}
		return nullptr;
	}

	ast::AstNode*
	Parser::
	parse_variable_decl() {
		Token token = m_ast->m_currToken;
		token.print(0);
		next_token();
		AstNodeList ids = parse_identifier_list();
		AstNodeList values;
		AstNode* type = nullptr;

		if(allow_token(TKN_COL)) {
			type = parse_type_or_ident();
		}
		if(allow_token(TKN_ASN)) {
			values = parse_rhs_expr_list();
		}
		if(!type && values.empty()) {
			// go to next stmt
			report_error(m_ast, "type must be specified or initialized\n");
			return ast_bad_decl();
		}

		AstNode* var = ast_variable_spec(token, ids, values, type);
		for(auto id : ids){
			if(!id->Ident.atom) {
				var->VariableSpec.flags |= VarHasIgnore;
				break;
			}
		}
		if(token.token() == TKN_LET) {
			var->VariableSpec.flags |= VarConst;
			if(values.empty()) {
				report_error(m_ast, "constant variables must initialized when declared\n");
				return ast_bad_decl();
			}
		}

		return var;
	}

	ast::AstNode*
	Parser::
	parse_body() {
		return parse_block_stmt();
	}

	ast::AstNode*
	Parser::
	parse_block_stmt() {
		Token begin = expect_token(TKN_LBRACK);
		AstNodeList stmts = parse_stmt_list();
		Token end = expect_token(TKN_RBRACK);
    std::cout << "Num Stmts: " << stmts.size() << std::endl;
		return ast_block_stmt(begin, end, stmts);
	}

	ast::AstNode*
	Parser::
	parse_type_decl(AstNode* id) {
		Token token = m_ast->m_currToken;
		AstNode* decl;
		next_token();
		switch(token.token()) {
			case TKN_CLASS:
			case TKN_STRUCT: {
				AstNodeList extends;
				bool oldClassDecl = m_ast->inClassDecl;
				if(token.token() == TKN_CLASS) {
					// parse inheritend types

					m_ast->inClassDecl = true;
				}
				expect_token(TKN_LBRACK);
				AstNodeList members;
				while(m_ast->m_currToken.token() != TKN_RBRACK) {
					AstNode* field = parse_field();
					field = parse_variable_tags(field);
					if(!expect_semicolon("member variable declaration"))
						return ast_bad_decl();
					add_node(members, field);
				}
				expect_token(TKN_RBRACK);
				if(token.token() == TKN_CLASS)
					decl = ast_class_type(token, id, extends, members, AstNodeList());
				else
					decl = ast_struct_type(token, id, members);

				m_ast->inClassDecl = oldClassDecl;
			} break;
			case TKN_ENUM:
			case TKN_UNION:
			default:
				report_error(m_ast, "Unexpected declaration token: '%s'\n", token_string(token.token()).c_str());
		}

		return decl;
	}

	ast::AstNode*
	Parser::
	parse_function_decl(AstNode* id) {
		bool isConst;
		AstNode* reciever = parse_function_reciever(isConst);
		AstNode* type = parse_function_type(id, reciever, isConst);
		
		if(!type) return nullptr;
		AstNode* body;
		AstNode* funct = ast_funct_method_decl(ast_token(type), type, nullptr);
		AstNode* oldParent = m_ast->currParent;
		m_ast->currParent = funct;
 
		if(check_token(TKN_LBRACK)) {
			body = parse_body(); // parse_block_stmt();
		}

		if(!body) {
			report_error(m_ast, "a function body must be declared when declaring a function\n");
		}
    ast_print(body, 0);
		funct->FunctMethodDecl.body = body;
		m_ast->currParent = oldParent;
		return parse_function_tags(funct);
	}

	ast::AstNode*
	Parser::
	parse_function_type(ast::AstNode* id, ast::AstNode* reciever, bool isConst) {
		AstNodeList returns;
		AstNodeList params;
		expect_token(TKN_LPAREN);
		if(check_token(TKN_IDENTIFIER))
			params = parse_field_list();
		expect_token(TKN_RPAREN);
    // debug_print("Function Type: num params: %d\n", params.size());


		AstNode* decl;
		if(check_token(TKN_RARR))
			returns = parse_function_return();
		// debug_print("Function Type: num returns: %d\n", returns.size());
		if(reciever)
			decl = ast_method_type(ast_token(id), id, reciever, params, returns);
		else
			decl = ast_function_type(ast_token(id), id, params, returns);
		if(reciever) {
			decl->MethodType.flags |= FunctMember;
			if(isConst)
				decl->MethodType.flags |= FunctConst;
		}
		decl = parse_function_tags(decl);
	
		return decl;
	}

	ast::AstNodeList
	Parser::
	parse_function_return() {
		AstNodeList list;
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
				list = parse_field_list();
			}
			else {
				// not named returns
				// debug_print("Parsing return type list\n");
				do {
					add_node(list, parse_type());
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

	ast::AstNode*
	Parser::
	parse_function_reciever(bool& isConst) {
		if(!allow_token(TKN_MULT))
			isConst = true;

		if(check_token(TKN_IDENTIFIER))
			return parse_identifier();
		else
			return nullptr;
	}

	ast::AstNode*
	Parser::
	parse_type_or_ident() {
		if(m_ast->m_currToken.type() == PrimativeType) {
			Token temp = m_ast->m_currToken;
			next_token();
			return ast_primative_type(temp);
		}
		switch(m_ast->m_currToken.token()) {
			case TKN_IDENTIFIER: {
				// same as selector; however, i only want identifiers
				// no other expressions.

				AstNode* s = parse_identifier();
				while(allow_token(TKN_PER)) {
					AstNode* expr = parse_identifier();
					s = ast_selector_expr(ast_token(s), s, nullptr, expr);
				}
				return ast_helper_type(s);
			}
			case TKN_MULT:
				return ast_pointer_type(m_ast->m_currToken, parse_type_or_ident());
			case TKN_LBRACE: {
				// parse arrays and map types
				// [][] | [..] arrays
				// [Type, Type] maps
				Token begin = expect_token(TKN_LBRACE);
				if(allow_token(TKN_DPER)) {
					expect_token(TKN_RBRACE);
					AstNode* type = parse_type_or_ident();
					return ast_dynamic_array_type(begin, type);
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
					debug_print("Is Map: %s counter: %d\n", (isMap?"true":"false"), counter);
					if(isMap) {
						// parse map
						AstNode* key,* value;

						key = parse_type_or_ident();
						expect_token(TKN_COM);
						value = parse_type_or_ident();

						Token end = expect_token(TKN_RBRACE);
						return ast_map_type(begin, end, key, value);
					}
					else {
						// parse array
						AstNode* expr = parse_expr(false);
						expect_token(TKN_RBRACE);
						AstNode* type = parse_type_or_ident();
						return ast_array_type(begin, expr, type);
					}
				}
			}
			// function type parsed here should only be pure functions, not methods.
			case TKN_LPAREN:
				return parse_function_type(nullptr, nullptr, false);
			default:
				break;
		}
		// no valid type found
		debug_print("return nullptr\n");
		return nullptr;
	}
	
	ast::AstNode*
	Parser::
	parse_type() {
		return parse_type_or_ident();
	}

	ast::AstNode*
	Parser::
	parse_field() {
		AstNodeList name = parse_identifier_list();
		expect_token(TKN_COL);
		AstNode* type = parse_type_or_ident();

		AstNode* field = ast_field_spec(name.front()->Ident.token, name, type);
		field = parse_variable_tags(field);
		return field;
	}

	ast::AstNodeList
	Parser::
	parse_field_list() {
		AstNodeList list;
		do {
			AstNode* e = parse_field();
			add_node(list, e);
		} while(allow_token(TKN_COM));
		return list;
	}

	ast::AstNode*
	Parser::
	parse_operand(bool _lhs) {
		// AstNode* operand = nullptr;
		Token token = m_ast->m_currToken;
		if(is_literal(token.token())) {
			next_token();
			return ast_basic_lit(token);
		}
    switch(token.token()) {
      case TKN_IDENTIFIER:
			  return parse_identifier();
		  case TKN_LPAREN: {
        Token begin = expect_token(TKN_LPAREN);
        AstNode* expr = parse_expr(false);
        Token end = expect_token(TKN_RPAREN);
        return ast_paren_expr(begin, end, expr);
      }
      case TKN_MULT: {
        Token op = expect_token(TKN_MULT);
        // paybe this should be 'parse_expr(_lhs)'
        AstNode* epxr = parse_operand(_lhs);
        return ast_deref_expr(op, epxr);
      }
      case TKN_AND: {
        Token op = expect_token(TKN_AND);
        AstNode* epxr = parse_operand(_lhs);
        return ast_address_expr(op, epxr);
      }
      default:
        break;
		}
		return nullptr;
	}

	ast::AstNode*
	Parser::
	parse_expr(bool _lhs) {
		if(_lhs)
			return parse_lhs_expr();
		else
			return parse_rhs_expr();
	}

	ast::AstNode*
	Parser::
	parse_lhs_expr() {
		return parse_binary_expr(true, 1);
	}

	ast::AstNode*
	Parser::
	parse_rhs_expr() {
  	return parse_binary_expr(false, 1);
	}

	ast::AstNodeList
	Parser::
	parse_lhs_expr_list() {
		AstNodeList list;
		do {
			// this check is probably unnessesary
			auto e = parse_lhs_expr();
			if(e)
				add_node(list, e);
		} while(allow_token(TKN_COM));

		return list;
	}

	ast::AstNodeList
	Parser::
	parse_rhs_expr_list() {
		AstNodeList list;
		do {
			// this check is probably unnessesary
			auto e = parse_rhs_expr();
			if(e)
				add_node(list, e);
		} while(allow_token(TKN_COM));

		return list;
	}

	ast::AstNode*
	Parser::
	parse_primary_expr(bool _lhs) {
		AstNode* operand = parse_operand(_lhs);

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
				case TKN_LBRACK:
					// initializer list
					return nullptr;
					break;
				case TKN_PER:
					// selector expr
					next_token();
					operand = ast_selector_expr(token, operand, nullptr, parse_primary_expr(_lhs));
					break;
				default:
					loop = false;
					break;
			}
		}
		return operand;
	}

  int operator_precedence(Token_Type _type) {
    switch(_type) {
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

	ast::AstNode*
	Parser::
	parse_binary_expr(bool _lhs, int _prec_in) {
  	AstNode* expr = parse_unary_expr(_lhs);
  	for(int prec = operator_precedence(m_ast->m_currToken.token()); prec >= _prec_in; --prec) {
  		for(;;) {
  			Token op = m_ast->m_currToken;
  			int op_prec = operator_precedence(op.token());
  			if(op_prec != prec)
  				break;
  			if(expect_operator()) {
  				AstNode* rhs = parse_binary_expr(false, prec + 1);
  				expr = ast_binary_expr(op, expr, rhs);
  			}
  			else
  				return ast_bad_expr(op, op);
  		}
  	}
  	return expr;
	}

	ast::AstNode*
	Parser::
	parse_unary_expr(bool _lhs) {
		Token token = m_ast->m_currToken;
		switch(token.token()) {
			case TKN_NOT:
			case TKN_SUB:
			case TKN_BNOT:
				next_token();
				return ast_unary_expr(token, parse_unary_expr(_lhs));
			default:
				break;
		}

		return parse_primary_expr(_lhs);
	}

	ast::AstNode*
	Parser::
	parse_index_expr(AstNode* operand) {
		// TODO(Andrew | Jonathan): handle slices
		Token begin = expect_token(TKN_LBRACE);
		AstNode* expr = parse_expr(false);
		Token end = expect_token(TKN_RBRACE);
		return ast_index_expr(begin, end, operand, expr);
	}

	ast::AstNode*
	Parser::
	parse_call_expr(AstNode* operand) {
		// opening of the parens
		expect_token(TKN_LPAREN);
		AstNodeList actuals = parse_rhs_expr_list();
		// custom expr list parser
		Token close_paren = expect_token(TKN_RPAREN);
		return ast_func_call(ast_token(operand), close_paren, operand, actuals);
	}

	ast::AstNode*
	Parser::
	parse_hash_directive() {
		debug_print("parse hash directive\n");
		expect_token(TKN_HASH);
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

/*--------------------------- Identifier Parsing -------------------------------*/

	ast::AstNode*
	Parser::
	parse_identifier() {
		debug_print("parse id\n");
		Token token = expect_token(TKN_IDENTIFIER);
		// if(token.token() == TKN_INVALID) {
// 			return nullptr;
// 		}
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

ast::AstNode*
Parser::
parse_function_tags(ast::AstNode* funct) {
	return funct;
}

ast::AstNode*
Parser::
parse_variable_tags(ast::AstNode* var) {
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
