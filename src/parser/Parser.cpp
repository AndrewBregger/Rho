//
// Created by Andrew Bregger on 8/23/17.
//

#include <cassert>
#include <ast/Ast.h>
#include "Parser.h"
#include "scanner/Scanner.h"

parser::Parser::Parser() {
	stats = &error_data;
	atoms = new AtomTable;
}


AstFile *parser::Parser::parse_file(File *file) {
	this->file = file;
	AstFile* ast = ast_file(file);
	if(init()) {
		while(!check_token<TKN_EOF>()) {
			ast->m_stmts.push_back(parse_stmt());
		}
		post_proccess();
	}
	else {
		auto err = file->GetError();
		switch(err) {
			case sys::File_Empty:
				file_error("'%.*s' was empty\n", file->GetPath().size(), file->GetPath().c_str());
				break;
			case sys::File_Permissions:
				file_error("'%.*s' wrong permissions on file\n", file->GetPath().size(), file->GetPath().c_str());
				break;
			case sys::File_WrongExtension:
				file_error("'%.*s' wrong extension\n", file->GetPath().size(), file->GetPath().c_str());
				break;
			case sys::File_InvalidFile:
				file_error("'%.*s' file doen't exist\n", file->GetPath().size(), file->GetPath().c_str());
				break;
			default:
				break;
		}
	}
	return ast;
}

void parser::Parser::consume() {
	prev = token;
	// im not checking because it will naturally end
	// when there are no more tokens.
	token = tokens[++index];
}

Token parser::Parser::peek_token(int offset) {
	assert(index + offset < tokens.size());
	return tokens[index + offset];
}

bool parser::Parser::expect_operator() {
	auto type = token.token();
	if(is_operator(type)) {
		consume();
		return true;
	}
	syntax_error(token, file, "expecting an operator, found: '%s'",
	             token.error_string().c_str());
	return false;
}

bool parser::Parser::init() {
	sys::FileError err = file->GetError();
	if(err != sys::FileError::File_None)
		return false;

	scanner::Scanner scan(file);
	Token t;
	index = 0;
	while(1) {
		t = scan.scan();
		tokens.push_back(t);
		if(t.token() == TKN_EOF)
			break;
	}
	token = tokens[index];
	return !scan.error;
}

void parser::Parser::sync() {

}

void parser::Parser::post_proccess() {
}

parser::ParserState parser::Parser::save_state() {
	ParserState state = {prev, token, index, currParent, *stats};
	return state;
}

void parser::Parser::restore_state(parser::ParserState state) {
	prev = state.prev;
	token = state.token;
	index = state.index;
	currParent = state.parent;
	*stats = state.stats;
}

bool requires_semicolon(Ast_Node* node) {
	if(!node) return false;
	switch(node->kind) {
		case AstBlockStmt:
			return false;
		case AstSpecStmt: {
			auto stmt = dynamic_cast<Ast_SpecStmt*>(node);
			if(stmt->kind == AstVariableSpec)
				return true;
			return false;
		}
		default:
			return true;
	}
}

Ast_Stmt *parser::Parser::parse_stmt() {
	Token t = token;
	Ast_Stmt* stmt = nullptr;
	switch(t.token()) {
		case TKN_IDENTIFIER:
		case TKN_LBOOL:
		case TKN_LDOUBLE:
		case TKN_LINT:
		case TKN_LCHAR:
		case TKN_LSTRING:
		case TKN_TYPE:
			stmt =  parse_simple_stmt();
			break;
		case TKN_IF: stmt =  parse_if_stmt();break;
		case TKN_FOR: stmt =  parse_for_stmt();break;
		case TKN_WHILE: stmt =  parse_while_stmt();break;
		case TKN_DEFER: stmt =  parse_defer_stmt();break;
		case TKN_RETURN: stmt =  parse_return_stmt();break;
		case TKN_BREAK: consume(); stmt =  new Ast_BreakStmt(t);break;
		case TKN_CONTINUE: consume(); stmt =  new Ast_ContinueStmt(t);break;
		case TKN_SEM: consume(); stmt =  new Ast_EmptyStmt(t);break;
		case TKN_LBRACK: stmt =  parse_block_stmt();break;
		case TKN_HASH: stmt =  parse_hash_stmt();break;
//		case TKN_MATCH: stmt =  parse_match_stmt();break;
		default:
			break;
	}
	if(requires_semicolon(stmt))
		expect_token<TKN_SEM>("expecting semicolon before '%s'",
													token.error_string().c_str());
	else if(check_token<TKN_SEM>())
		consume();
	return stmt;
}

Ast_Stmt *parser::Parser::parse_simple_stmt() {
	auto tempState = save_state();
	Ast_Stmt* stmt = nullptr;
	if(check_token<TKN_IDENTIFIER>() || check_token<TKN_TYPE>()) {
		Ast_Decl* decl = parse_decl();
		// if decl parsing failed
		if(decl)
			stmt = new Ast_SpecStmt(decl);
		else if(tempState.stats.errors != stats->errors)
			// there was an error when reading a declaration
			return nullptr;
	}
	if(!stmt) {
		AstList<Ast_Expr *> lhs = parse_expr_list(true);
		switch (token.token()) {
			case TKN_ASN:
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
				Token op = token;
				consume();
				AstList<Ast_Expr*> values = parse_expr_list(false);
				if(values.empty()) {

				}
				stmt = new Ast_AssignmentStmt(op, lhs, values);
			} break;
			default: {
				if(lhs.size() > 1) {
					syntax_error(lhs.front()->token(), file, "expecting only one expression in statement");
				}
				else {
					stmt = new Ast_ExprStmt(lhs[0]);
				}
			} break;
		}
	}
	return stmt;
}

Ast_IfStmt *parser::Parser::parse_if_stmt() {
	auto t = expect_token<TKN_IF>("");
	Ast_Expr* cond = parse_expr(false);
	Ast_Stmt* body = parse_stmt();
	Ast_Stmt* elseif = nullptr;
	switch(token.token()) {
		case TKN_ELSE:
			consume();
			elseif = parse_if_stmt();
			break;
		case TKN_ELIF:
			consume();
			elseif = parse_stmt();
		default:
			break;
	}
	return new Ast_IfStmt(t, cond, body, elseif);
}

Ast_WhileStmt *parser::Parser::parse_while_stmt() {
	auto t = expect_token<TKN_WHILE>("");
	auto expr = parse_expr(false);
	auto body = parse_stmt();
	return new Ast_WhileStmt(t, expr, body);
}

Ast_ForStmt *parser::Parser::parse_for_stmt() {
	auto t = expect_token<TKN_FOR>("");
	Ast_Expr* op = parse_expr(false);
	auto cond = parse_in_expr(op);
	auto body = parse_stmt();
	return new Ast_ForStmt(t, cond, body);
}

Ast_DeferStmt *parser::Parser::parse_defer_stmt() {
	auto t = expect_token<TKN_DEFER>("");
	auto expr = parse_expr(false);
	return new Ast_DeferStmt(t, expr);
}

Ast_ReturnStmt *parser::Parser::parse_return_stmt() {
	auto t = expect_token<TKN_RETURN>("");
	AstList<Ast_Expr*> list = parse_expr_list(false);
	return new Ast_ReturnStmt(t, list);
}

Ast_BlockStmt *parser::Parser::parse_block_stmt() {
	expect_token<TKN_LBRACK>("");
	AstList<Ast_Stmt*> list;
	while(!check_token<TKN_RBRACK>()) {
		add_node(list, parse_stmt());
	}
	consume(); // consumes '}'
	return new Ast_BlockStmt(list);
}

Ast_Stmt *parser::Parser::parse_hash_stmt() {
	Token t = expect_token<TKN_HASH>("");
	Ast_Stmt* stmt = nullptr;
	Ast_Identifier* id = nullptr;
	if(check_token<TKN_IDENTIFIER>())
		id = parse_identifier();
	else {
		syntax_error(token, file, "expecting an identifier following a hash directive");
		sync();
	}

	std::string str = id->name->name();

	if(str == "load") {
		Ast_ImportSpec* spec = parse_import_spec();
		stmt = new Ast_SpecStmt(spec);
	}
	else if(str == "construct" || str == "destruct") {
		if(!inClassBody)
			syntax_error(t, file, "%s hash directive only allowed in class body", str.c_str());
		// if there is an error with the directive, I will
		// let it parse the rest of the function if it is one.
		if(check_token<TKN_DCOL>()) {
			consume();
			Ast_ProcSpec* proc = parse_proc_spec(id);
			stmt = new Ast_SpecStmt(proc);
		}
		else {
			syntax_error(t, file, "%s hash directive must be a function declaration", str.c_str());
			sync();
		}
	}
	return stmt;
}

Ast_Expr *parser::Parser::parse_expr(bool lhs) {
	return parse_binary_expr(1);
}

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
		case TKN_IN:
			return 0;
		default:
			return 0;
	}
}

Ast_Expr *parser::Parser::parse_binary_expr(int prec_in) {
	Ast_Expr* expr = parse_unary_expr();
	for(int prec = operator_precedence(token.token()); prec >= prec_in; --prec) {
		for(;;) {
			Token op = token;
			if(operator_precedence(op.token()) != prec)
				break;
			// @NOTE(Andrew): should be be parsed here?
			if(expect_operator()) {
				Ast_Expr* rhs = parse_binary_expr(prec + 1);
				expr = new Ast_BinaryExpr(op, expr, rhs);
			}
		}
	}
	return expr;
}

Ast_Expr *parser::Parser::parse_unary_expr() {
	Token op = token;
	switch(token.token()) {
		case TKN_SUB:
		case TKN_NOT:
		case TKN_BNOT:
			consume();
			return new Ast_UnaryExpr(op, parse_unary_expr());
		case TKN_NEW:
			consume();
//			return new Ast_NewExpr(op, );
		default:
			break;
	}
	return parse_atomic_expr();
}

Ast_Expr *parser::Parser::parse_operand() {
	if(is_literal(token.token()) && token.token() != TKN_LSTRING) {
		consume();
		return new Ast_BasicLiteral(prev);
	}
	if(token.token() == TKN_LSTRING) {
		Token t = token;
		std::string str = t.get_string();
		consume();
		while(token.token() == TKN_LSTRING) {
			str += std::string(token.get_string());
			consume();
		}
		Token new_token = Token(t.token(),
														Constant,
														t.location(),
														str.c_str());
		return new Ast_BasicLiteral(new_token);
	}
	switch(token.token()) {
		// if it is expecting a type, the identifier node
		// will be used as a type node.
		case TKN_NULL:
			consume();
			return new Ast_NullLiteral(prev);
		case TKN_MULT: {
			Token op = token;
			Ast_Expr* expr = parse_operand();
			return new Ast_DerefExpr(op, expr);
		} break;
		case TKN_LPAREN: {
			Token begin = expect_token<TKN_LPAREN>("");
			Ast_Expr *expr = parse_expr();
			Token end = expect_token<TKN_RPAREN>("");
			return new Ast_ParenExpr(begin, end, expr);
		}
		case TKN_IDENTIFIER:
			// if it isn't a generic type
			// foo<int>
			if(peek_token().token() != TKN_LESS)
				return parse_identifier();
		case TKN_ENUM:
		case TKN_UNION:
		case TKN_STRUCT:
		case TKN_CLASS:
		case TKN_INT:
		case TKN_I8:
		case TKN_I16:
		case TKN_I32:
		case TKN_I64:
		case TKN_U8:
		case TKN_U16:
		case TKN_U32:
		case TKN_U64:
		case TKN_STRING:
		case TKN_FLOAT:
		case TKN_FLOAT32:
		case TKN_FLOAT64:
		case TKN_BOOL:
		case TKN_CHAR:
		case TKN_BYTE:
		case TKN_VOID:
		case TKN_LBRACE: {
			Ast_Type* type = parse_type();
			switch(type->kind) {
				case AstClassType:
				case AstStructType:
				case AstEnumType:
				case AstUnionType:
				case AstPointerType:
					syntax_error(type->token(), file, "invalid type: %s", type->string().c_str());
				default:
					break;
			}
			return new Ast_TypeExpr(type);
		}
		default:
			break;
	}
	return nullptr;
}

Ast_Expr *parser::Parser::parse_atomic_expr() {
	Ast_Expr* operand = parse_operand();
	bool loop = true;
	while(loop) {
		switch(token.token()) {
			case TKN_PER:
				consume();
				if (check_token<TKN_IDENTIFIER>()) {
					operand = new Ast_SelectorExpr(operand, parse_identifier());
				} else
					syntax_error(token, file, "expecting an identifer in selector expression");
				break;
			case TKN_LPAREN: {
				operand = parse_function_call(operand);
			} break;
			case TKN_LBRACE:
				operand = parse_index_expr(operand);
				break;
			case TKN_LBRACK:
				operand = parse_compound_literal(nullptr);
				break;
			case TKN_DPER:
				operand = parse_range_expr(operand);
				break;
			default:
				loop = false;
		}
	}
	return operand;
}

Ast_FuncCall *parser::Parser::parse_function_call(Ast_Expr *operand) {
	expect_token<TKN_LPAREN>("");
	auto actuals = parse_expr_list(false);
	expect_token<TKN_RPAREN>("");
	return new Ast_FuncCall(operand, nullptr, actuals);
}

Ast_IndexExpr *parser::Parser::parse_index_expr(Ast_Expr *operand) {
	Token begin = expect_token<TKN_LBRACE>("");
	Ast_Expr* expr = parse_expr(false);
	Token end = expect_token<TKN_RBRACE>("");
	return new Ast_IndexExpr(begin, end, operand, expr);
}

AstList<Ast_Expr *> parser::Parser::parse_expr_list(bool lhs) {
	AstList<Ast_Expr*> list;
	do {
		add_node(list, parse_expr(lhs));
	} while(allow_token<TKN_COM>());
	return list;
}

Ast_Identifier *parser::Parser::parse_identifier() {
	Token id = expect_token<TKN_IDENTIFIER>("");
	// NOTE(Andrew): This shouldn't do this twice.
	if(id.token() != TKN_IDENTIFIER)
		return nullptr;

	std::string str = id.get_string();

	if(str.size() == 1 && str[0] == '_') {
		auto ident = new Ast_Identifier(id, nullptr);
		ident->ignoreId = true;
		return ident;
	}
	Atom* atom = atoms->insert(str);
	return new Ast_Identifier(id, atom);
}


AstList<Ast_Identifier *> parser::Parser::parse_identifier_list() {
	AstList<Ast_Identifier*> list;
	do {
		add_node(list, parse_identifier());
	} while(allow_token<TKN_COM>());
	return list;
}

Ast_CompoundLiteral *parser::Parser::parse_compound_literal(Ast_Expr *operand) {
	Token t = expect_token<TKN_LBRACK>("");
	AstList<Ast_Expr*> lits;
	while(!check_token<TKN_RBRACK>()) {
		add_node(lits, parse_compound_element(operand));
		if(check_token<TKN_COM>())
			consume();
		else break;
	}
	expect_token<TKN_RBRACK>("");
	return new Ast_CompoundLiteral(t, lits);
}

Ast_Expr *parser::Parser::parse_compound_element(Ast_Expr *operand) {
	if(check_token<TKN_LBRACK>())
		return parse_compound_literal(nullptr);
	if(check_token<TKN_IDENTIFIER>() && peek_token().token() == TKN_COL) {
		Ast_Identifier* id = parse_identifier();
		expect_token<TKN_COL>("");
		Ast_Expr* expr = parse_expr(false);
		return new Ast_NamedLiteral(id, expr);
	}
	return parse_expr(false);
}

// : 0..10,2
//   ^-> operand
Ast_RangeExpr *parser::Parser::parse_range_expr(Ast_Expr *operand) {
	auto t = expect_token<TKN_DPER>("");
	auto expr = parse_expr(false);
	Ast_Expr* step = nullptr;
	if(allow_token<TKN_COM>())
		step = parse_expr(false);
	return new Ast_RangeExpr(operand, expr, step);
}
// x : 0..10,2
// ^-> operand
Ast_InExpr *parser::Parser::parse_in_expr(Ast_Expr *operand) {
	auto t = expect_token<TKN_COL>("");
	auto expr = parse_expr(false);
	return new Ast_InExpr(operand, expr);
}

Ast_Decl *parser::Parser::parse_decl() {
	ParserState state = save_state();
	if(check_token<TKN_TYPE>()) {
		// type Vector3f = Vector<3, f32>
		consume();
		Ast_Identifier* alias = parse_identifier();
		expect_token<TKN_ASN>("expecting an assignment operator when creating a type alias");
		Ast_Type* type = parse_type();
		return new Ast_TypeAlias(alias, type);
	}

	AstList<Ast_Expr*> lhs = parse_expr_list(true);
	if(!lhs.empty() && lhs.all_of_type(ast::AstIdent)) {
		// all of the expressions are identifiers
		AstList<Ast_Identifier*> ids = lhs.convert_to<Ast_Identifier*>();
		switch(token.token()) {
			case TKN_DECL:
			case TKN_COL:
			case TKN_NOTNOT:
				return parse_variable_spec(ids);
			case TKN_DCOL: {
				consume(); // consumes the ::
				if(ids.size() != 1) {
					syntax_error(token, file, "only one name is allowed to be binded to a language structure definition");
					syntax_note(token, "use a type alias to give a type another name");
//						sync();
					return nullptr;
				}
				switch(token.token()) {
					case TKN_STRUCT:
					case TKN_CLASS:
						return parse_type_spec(ids[0]);
					default:
						return parse_proc_spec(ids[0]);
				}
			} break;
			default:
				break;
		}
	}
	restore_state(state);
	return nullptr;
}

Ast_VariableSpec *parser::Parser::parse_variable_spec(AstList<Ast_Identifier *> ids) {
	Token t;
	Ast_Type* type = nullptr;
	AstList<Ast_Expr*> values;
	if(check_token<TKN_COL>()) {
		consume(); // consumes :

		// types
		type = parse_type();

		// values
		if(check_token<TKN_ASN>() || check_token<TKN_NOTNOT>()) {
			t = token;
			consume();
			values = parse_expr_list(false);
		}
	}
	else if(check_token<TKN_DECL>()){
		consume();
		values = parse_expr_list(false);
		if(values.empty()) {
			syntax_error(token, file, "variables must be initialized when not given a type");
		}
	}
	else {
		syntax_error(token, file, "Compiler failed\n");
		return nullptr;
	}
	VariableFlags flags = 0;

	if(t.token() == TKN_NOTNOT)
		flags |= VarConst;
	if(!type)
		flags |= VarInferType;
	for(auto i : ids) {
		if(i->ignoreId) {
			flags |= VarHasIgnore;
			break;
		}
	}
	return new Ast_VariableSpec(ids.front()->token(), ids, values, type, flags);
}

Ast_ProcSpec *parser::Parser::parse_proc_spec(Ast_Identifier *id) {
	Ast_ProcType* type = parse_function_type();
	Ast_Stmt* body = parse_stmt();
	return new Ast_ProcSpec(type->token(), id, type, body);
}

Ast_TypeSpec *parser::Parser::parse_type_spec(Ast_Identifier *id) {
	Ast_Type* type;
	Token t = token;
	switch(token.token()) {
		case TKN_CLASS:
			type = parse_class_type();
			break;
		case TKN_STRUCT:
			type = parse_struct_type();
			break;
		default:
			syntax_error(token, file, "invalid type keyword: '%s'", t.error_string().c_str());
			break;
	}
	return new Ast_TypeSpec(id, type);
}

Ast_FieldSpec *parser::Parser::parse_field_spec() {
	AstList<Ast_Identifier*> ids = parse_identifier_list();
	if(ids.empty()) {
		syntax_error(token, file, "expecting to find an identifier in field specification");
	}
	expect_token<TKN_COL>("");
	Ast_Type* type = parse_type();
	return new Ast_FieldSpec(ids.front()->token(), ids, type);
}

Ast_FieldSpec *parser::Parser::parse_class_field() {
	AstList<Ast_Identifier*> ids = parse_identifier_list();
	if(check_token<TKN_COL>()) {

	}
	else if(check_token<TKN_DECL>()) {

	}
	return nullptr;
}

Ast_FieldSpec *parser::Parser::parse_param_field() {
	Ast_FieldSpec* field = parse_field_spec();
	AstList<Ast_Expr*> defaults;
	if(allow_token<TKN_EQ>())
		defaults = parse_expr_list(false);
	field->defaults = defaults;
	return field;
}

Ast_ImportSpec *parser::Parser::parse_import_spec() {
	return nullptr;
}

Ast_Type *parser::Parser::parse_type() {
	std::vector<Token> mods;
	for(;;) {
		switch(token.token()) {
			case TKN_REF:
			case TKN_CONST:
				mods.push_back(token);
				consume();
				continue;
			default:
				break;
		}
		break;
	}
	Ast_Type* type = nullptr;
	bool constant = false;
	if(token.token() >= TKN_INT && token.token() <= TKN_WCHAR) {
		consume();
		auto ty = new Ast_PrimativeType(prev);
		for (auto t : mods) {
			switch (t.token()) {
				case TKN_REF:
					ty->flags |= TypeRef;
					break;
				case TKN_CONST:
					ty->flags |= TypeConst;
					break;
				default:
					break;
			}
		}
		return ty;
	}
	switch(token.token()) {
		case TKN_IDENTIFIER: {
			Ast_Identifier *id = parse_identifier();
			auto name = new Ast_NamedType(id, AstList<Ast_Type*>());
			auto expr = new Ast_SelectedTypedExpr(id->token(), nullptr, name);
			switch (token.token()) {
				case TKN_PER: {
					expr = new Ast_SelectedTypedExpr(id->token(), expr, parse_type());
					auto ty = new Ast_NamedType(expr, AstList<Ast_Type*>());
					for (auto t : mods) {
						switch (t.token()) {
							case TKN_REF:
								ty->flags |= TypeRef;
								break;
							case TKN_CONST:
								ty->flags |= TypeConst;
								break;
							default:
								break;
						}
					}
					return ty;
				}
					break;
				case TKN_LESS:
					// generic parameters
					break;
				default:
					return name;
			}
		}
		case TKN_STRUCT:
			return parse_struct_type();
		case TKN_CLASS:
			return parse_class_type();
		case TKN_LBRACE: {
			Token t = expect_token<TKN_LBRACE>("");
			Ast_Expr* expr = parse_expr(false);
			if(allow_token<TKN_COM>()) {
				Ast_Type* key = nullptr;
				if(expr->kind != AstTypeExpr && expr->kind != AstSelectorExpr && expr->kind != AstIdent)
					syntax_error(expr->token(), file, "expecting a type expression for key type");
				else {
					if(expr->kind == AstTypeExpr)
						key = dynamic_cast<Ast_TypeExpr*>(expr)->type;
					else
						key = new Ast_NamedType(expr, AstList<Ast_Type*>());
				}
				Ast_Type* value = parse_type();
				expect_token<TKN_RBRACE>("");
				return new Ast_MapType(t, key, value);
			}
			else {
				expect_token<TKN_RBRACE>("");
				Ast_Type* type = parse_type();
				return new Ast_ArrayType(t, type, expr);
			}
		} break;
		case TKN_MULT: {
			Token t = token;
			consume();
			type = parse_type();
			constant = false;
			if(!check_token<TKN_LPAREN>())
				return new Ast_PointerType(t, type);
		}
		case TKN_LPAREN:
			return parse_function_type(type, constant);
		case TKN_DOLLAR: {
			Token t = expect_token<TKN_DOLLAR>("");
			Ast_Identifier *name = parse_identifier();
			AstList<Ast_Type *> specs;
			if (allow_token<TKN_DIV>()) {
				consume();
				do {
					Ast_Type *type = parse_type();
					add_node(specs, type);
				} while (allow_token<TKN_ADD>());
			}
			return new Ast_PolymorphicType(t, name, specs);
		}
		default:
			break;
	}
	return nullptr;
}

// @Todo(Andrew): Add type flags
Ast_StructType *parser::Parser::parse_struct_type() {
	AstList<Ast_Decl*> decls;
	auto t = expect_token<TKN_STRUCT>("");
	auto specs = parse_polymophic_type_list();
	expect_token<TKN_LBRACK>("");
	while(!check_token<TKN_RBRACK>()) {
		Ast_Stmt *stmt = parse_stmt();
		if (stmt->kind == AstSpecStmt) {
			auto decl = dynamic_cast<Ast_SpecStmt *>(stmt);
			add_node(decls, decl->decl);
		} else {
			syntax_error(stmt->token(), file, "expecting only declaration to be in a structure body");
		}
	}
	expect_token<TKN_RBRACK>("");
	return new Ast_StructType(t, decls, specs);
}

Ast_ClassType *parser::Parser::parse_class_type() {
	AstList<Ast_Decl*> decls;
	auto t = expect_token<TKN_CLASS>("");
	auto specs = parse_polymophic_type_list();
	AstList<Ast_Type*> extends;
	if(allow_token<TKN_COL>()) {
		do {
			add_node(extends, parse_type());
		} while(allow_token<TKN_COM>());
		for(int i = 0; i < extends.size(); ++i) {
			if(extends[i]->kind != AstNamedType ||
			   extends[i]->kind != AstPrimativeType) {
				syntax_error(extends[i]->token(), file, "%s are not an inheritable type",
				             extends[i]->string().c_str());
			}
		}
	}
	expect_token<TKN_LBRACK>("");
	bool prevBody = inClassBody;
	inClassBody = true;
	while(!check_token<TKN_RBRACK>()) {
		Ast_Stmt *stmt = parse_stmt();
		if (stmt->kind == AstSpecStmt) {
			auto decl = dynamic_cast<Ast_SpecStmt *>(stmt);
			add_node(decls, decl->decl);
		} else {
			syntax_error(stmt->token(), file, "expecting only declaration to be in a structure body");
		}
	}
	expect_token<TKN_RBRACK>("");
	inClassBody = prevBody;
	return new Ast_ClassType(t, decls, extends, specs);
}


/*
	foo :: inline (x, y: int) -> int const { return x + y; }
 ^------------------------------------------^   Function/method declaration
				        ^----------------^              Function Type
*/
Ast_ProcType *parser::Parser::parse_function_type(Ast_Type *reciever, bool constant) {

	// handles method reciever;
	Ast_Type* recieve = reciever;
	bool consts = constant;
	// check if there is a receiver
	if(recieve == nullptr) {
		// need to check for a reciever type.

		// the receiver can be surrounded by paranthesis
		// foo :: (*Bar) () {} @TODO(Andrew): implement parsing this later.
		// for :: *Bar () {}
		Token next = peek_token();
		if(check_token<TKN_MULT>() || check_token<TKN_IDENTIFIER>() /*||
				(check_token<TKN_LPAREN>() &&
						(next.token() == TKN_MULT || next.token() == TKN_IDENTIFIER))*/) {
//			bool found_lparen = false;
//			if(check_token<TKN_LPAREN>()) {
//				consume();
//				found_lparen = true;
//			}

			if (check_token<TKN_MULT>()) {
				consume();
				consts = true;
			}

			recieve = parse_type();
			if (recieve->kind != AstNamedType)
				syntax_error(recieve->token(), file, "method receiver cannot be %s",
				             recieve->string().c_str());

//			if (found_lparen)
//				expect_token<TKN_RPAREN>("");
		}

		/*
			foo :: (x, y: int) {}
						 ^
		*/
	}

	Token begin = expect_token<TKN_LPAREN>("");
	AstList<Ast_FieldSpec*> fields;
	AstList<Ast_ProcReturn*> rets;
	while(!check_token<TKN_RPAREN>()) {
		Ast_FieldSpec* field = parse_param_field();
		add_node(fields, field);
		if(check_token<TKN_COM>())
			consume();
		else
			break;
	}

	expect_token<TKN_RPAREN>("");
	if(prev.token() == TKN_COM && check_token<TKN_RPAREN>()) {
		syntax_error(prev, file, "expecting an identifier after '%s'",
		             prev.error_string().c_str());
	}
	if(allow_token<TKN_RARR>())
		rets = parse_parenthesis<AstList<Ast_ProcReturn*> >(&Parser::parse_function_return);
	Token t = recieve? recieve->token(): begin;
	FunctionFlags flags = 0;
	if(recieve)
		return new Ast_MethodType(t, recieve, constant, fields, rets, flags);
	else
		return new Ast_FunctionType(t, fields, rets, flags);
}

AstList<Ast_ProcReturn *> parser::Parser::parse_function_return() {
	auto state = save_state();
	AstList<Ast_Type*> list;
	AstList<Ast_ProcReturn*> rets;
	do {
		add_node(list, parse_type());
	} while(allow_token<TKN_COM>());

	if(check_token<TKN_COL>()) {
		restore_state(state);
		AstList<Ast_FieldSpec*> fields;
		do {
			add_node(fields, parse_field_spec());
		} while(allow_token<TKN_COM>());
		for(auto i : fields) {
			add_node(rets, (Ast_ProcReturn*) new Ast_NamedReturn(i));
		}
		return rets;
	}
	else {
		for(auto i : list) {
			add_node(rets, (Ast_ProcReturn*) new Ast_TypedReturn(i));
		}
		return rets;
	}
}

AstList<Ast_PolymorphicType *> parser::Parser::parse_polymophic_type_list() {
	AstList<Ast_PolymorphicType*> specs;
	if(allow_token<TKN_LESS>()) {
		AstList<Ast_Type*> temp;
		do {
			add_node(temp, parse_type());
		} while(allow_token<TKN_COM>());
		expect_token<TKN_GRET>("");
		int index = 0;
		if(temp.all_of_type(AstPolyType, index))
			specs = temp.convert_to<Ast_PolymorphicType*>();
		else {
			syntax_error(temp[index]->token(), file, "only polymophics types are allowed in type list");
		}
	}
	return specs;
}
