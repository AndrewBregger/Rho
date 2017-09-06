#include "Ast.h"
#include <iostream>
#include <cassert>

namespace ast {

#define delete_content(l) for(auto i : l) delete i;

#define print_content(l) for(auto i : l) i->print(indent + 1);


//--------------------------------- Helper Functions --------------------------
	bool ast_is_decl(Ast_Node *node) {
		return Ast_BeginDecl < node->kind && node->kind < Ast_EndDecl;
	}

	bool ast_is_expr(Ast_Node *node) {
		return Ast_BeginExpr < node->kind && node->kind < Ast_EndExpr;
	}

	bool ast_is_stmt(Ast_Node *node) {
		return Ast_BeginStmt < node->kind && node->kind < Ast_EndStmt;
	}

	bool ast_is_type(Ast_Node *node) {
		return Ast_BeginType < node->kind && node->kind < Ast_EndType;
	}
//----------------------------------- Ast Creation -------------------------------

	static size_t fileID = 0;

	Token Ast_Node::emptyReturn = {};

	AstFile *ast_file(sys::File *_file) {
		AstFile *file = new AstFile;
		file->m_id = fileID++;
		file->m_file = _file;
		return file;
	}

	const std::vector<std::string> node_strings = {
#define Ast_Node_Kind(_kind, str) str,
			Ast_Node_Kinds
#undef Ast_Node_Kind
	};


	void indent_print(const std::string &str, int indent = 0, bool nl = true) {
		while (indent--)
			std::cout << "  ";
		std::cout << str;
		if (nl)
			std::cout << std::endl;
		else
			std::cout << std::flush;
	}


	Ast_Node::
	Ast_Node(AstNodeKind k) {
		kind = k;
	}

	Ast_Node::
	~Ast_Node() {}

	void
	Ast_Node::
	print(int indent) {
		indent_print(string(), indent);
	}

	const std::string &Ast_Node::string() {
		return node_strings[kind];
	}

	Ast_BadNode::
	Ast_BadNode(AstNodeKind k) : Ast_Node(k) {}

	const token::Token &
	Ast_BadNode::
	token() {
		static Token t = Token();
		return t;
	}

	Ast_Identifier::
	Ast_Identifier(Token token, ast::Atom *a) : Ast_Expr(AstIdent) {
		tok = token;
		name = a;
		ignoreId = (name == nullptr);
	}

	Ast_Identifier::
	~Ast_Identifier() {
		//TODO(): implement

	}

	void
	Ast_Identifier::
	print(int indent) {
		Ast_Node::print(indent);
		if (name)
			indent_print(name->name(), indent + 1);
	}

	const token::Token &
	Ast_Identifier::
	token() {
		//TODO(): implement
		return tok;
	}

	Ast_BasicLiteral::
	Ast_BasicLiteral(Token t) : Ast_Expr(AstBasicLit) {
		tok = t;
	}

	void
	Ast_BasicLiteral::
	print(int indent) {
		//TODO(): implement
		Ast_Node::print(indent);
		tok.print(indent + 1);
	}

	const token::Token &
	Ast_BasicLiteral::
	token() {
		//TODO(): implement
		return tok;
	}

	Ast_NullLiteral::
	Ast_NullLiteral(Token t) : Ast_Expr(AstNullLit) {
		tok = t;
	}

	void
	Ast_NullLiteral::
	print(int indent) {
		//TODO(): implement
		Ast_Node::print(indent);
		tok.print(indent + 1);
	}

	const token::Token &
	Ast_NullLiteral::
	token() {
		//TODO(): implement
		return tok;
	}

	Ast_Decl::
	Ast_Decl(AstNodeKind k) : Ast_Node(k) {}

	Ast_Decl::
	~Ast_Decl() {}

	void
	Ast_Decl::
	print(int indent) {
		//TODO(): implement
	}

	const token::Token &
	Ast_Decl::
	token() {
		//return n->token();
		//TODO(): implement
		static token::Token t = token::Token();
		return t;
	}

	Ast_TypeAlias::
	Ast_TypeAlias(Ast_Identifier *a, Ast_Type *t) : Ast_Decl(AstTypeAliasSpec) {
		alias = a;
		type = t;
	}

	Ast_TypeAlias::
	~Ast_TypeAlias() {
		delete alias;
		delete type;
	}

	void
	Ast_TypeAlias::
	print(int indent) {
		Ast_Node::print(indent);
		alias->print(indent + 1);
		type->print(indent + 1);
	}

	const token::Token &
	Ast_TypeAlias::
	token() {
		return alias->token();
	}

	Ast_VariableSpec::
	Ast_VariableSpec(Token t, AstList<Ast_Identifier *> n,
	                 AstList<Ast_Expr *> v, Ast_Type *ty, VariableFlags f) : Ast_Decl(AstVariableSpec) {
		tok = t;
		names = n;
		values = v;
		type = ty;
		flags = f;
	}

	Ast_VariableSpec::
	~Ast_VariableSpec() {
		delete_content(names)
		delete_content(values)
		delete type;
	}

	void
	Ast_VariableSpec::
	print(int indent) {
		Ast_Node::print(indent);
		print_content(names)
		print_content(values)
		if (type) type->print(indent + 1);
		indent_print("Flags: " + std::to_string(flags), indent + 1);
	}

	const token::Token &
	Ast_VariableSpec::
	token() {
		return tok;
	}

	Ast_TypeSpec::
	Ast_TypeSpec(Ast_Identifier *n, Ast_Type *t) : Ast_Decl(AstTypeSpec) {
		type = t;
		name = n;
	}

	Ast_TypeSpec::
	~Ast_TypeSpec() {
		delete type;
		delete name;
	}

	void
	Ast_TypeSpec::
	print(int indent) {
		Ast_Node::print(indent);
		if (type) type->print(indent + 1);
		if (name) name->print(indent + 1);
	}

	const token::Token &
	Ast_TypeSpec::
	token() {
		return type->token();
	}

	Ast_ProcSpec::
	Ast_ProcSpec(Token t, Ast_Identifier *n, Ast_ProcType *ty, Ast_Stmt *b) : Ast_Decl(AstProcSpec) {
		tok = t;
		name = n;
		type = ty;
		body = b;
	}

	Ast_ProcSpec::
	~Ast_ProcSpec() {
		delete type;
		delete body;
		delete name;
	}

	void
	Ast_ProcSpec::
	print(int indent) {
		Ast_Node::print(indent);
		name->print(indent + 1);
		type->print(indent + 1);
		body->print(indent + 1);
	}

	const token::Token &
	Ast_ProcSpec::
	token() {
		return name->token();
	}

	Ast_ImportSpec::
	Ast_ImportSpec(Token p, const std::string &fp,
	               Ast_Identifier *n, const AstList<Ast_Identifier *> &i) : Ast_Decl(AstImportSpec) {
		relPath = p;
		fullPath = fp;
		name = n;
		imports = i;
	}

	Ast_ImportSpec::
	~Ast_ImportSpec() {
		delete name;
		delete_content(imports)
	}

	void
	Ast_ImportSpec::
	print(int indent) {
		Ast_Node::print(indent);
		if (name) name->print(indent + 1);
		relPath.print(indent + 1);
		indent_print(fullPath, indent + 1);
		print_content(imports);
		static std::string t = "true";
		static std::string f = "false";
		indent_print("Valid: " + (valid ? t : f), indent + 1);
		indent_print("Ast: " + std::to_string(ast), indent + 1);
	}

	const token::Token &
	Ast_ImportSpec::
	token() {
		return relPath;
	}

	Ast_FieldSpec::
	Ast_FieldSpec(Token t, const AstList<Ast_Identifier *> &n,
	              Ast_Type *ty, VariableFlags f) : Ast_Decl(AstFieldSpec) {
		tok = t;
		names = n;
		type = ty;
		flags = f;
	}

	Ast_FieldSpec::
	~Ast_FieldSpec() {
		delete_content(names);
		delete type;
	}

	void
	Ast_FieldSpec::
	print(int indent) {
		Ast_Node::print(indent);
		print_content(names);
		type->print(indent + 1);
	}

	const token::Token &
	Ast_FieldSpec::
	token() {
		return tok;
	}

	Ast_Stmt::
	Ast_Stmt(AstNodeKind k) : Ast_Expr(k) {}

// void print(int indent = 0) {
// }
// const token::Token& token(){
// }

	Ast_SpecStmt::
	Ast_SpecStmt(Ast_Decl *d) : Ast_Stmt(AstSpecStmt) {
		assert(d);
		decl = d;
	}

	Ast_SpecStmt::
	~Ast_SpecStmt() {
		delete decl;
	}

	void Ast_SpecStmt::
	print(int indent) {
		Ast_Node::print(indent);
		decl->print(indent + 1);
	}

	const token::Token &Ast_SpecStmt::
	token() {
		return decl->token();
	}

	Ast_ExprStmt::
	Ast_ExprStmt(Ast_Expr *e) : Ast_Stmt(AstExprStmt) {
		expr = e;
	}

	Ast_ExprStmt::
	~Ast_ExprStmt() {
		delete expr;
	}

	void
	Ast_ExprStmt::
	print(int indent) {
		Ast_Node::print(indent);
		expr->print(indent + 1);
	}

	const token::Token &
	Ast_ExprStmt::
	token() {
		return expr->token();
	}

	Ast_EmptyStmt::
	Ast_EmptyStmt(Token t) : Ast_Stmt(AstEmptyStmt) {
		tok = t;
	}

	void
	Ast_EmptyStmt::
	print(int indent) {
		Ast_Node::print(indent);
		tok.print(indent + 1);
	}

	const token::Token &
	Ast_EmptyStmt::
	token() {
		return tok;
	}

	Ast_BreakStmt::
	Ast_BreakStmt(Token t) : Ast_Stmt(AstBreakStmt) {
		tok = t;
	}

	void
	Ast_BreakStmt::
	print(int indent) {
		Ast_Node::print(indent);
		tok.print(indent + 1);
	}

	const token::Token &
	Ast_BreakStmt::
	token() {
		return tok;
	}

	Ast_ContinueStmt::
	Ast_ContinueStmt(Token t) : Ast_Stmt(AstContinueStmt) {
		tok = t;
	}

	void
	Ast_ContinueStmt::
	print(int indent) {
		Ast_Node::print(indent);
		tok.print(indent + 1);
	}

	const token::Token &
	Ast_ContinueStmt::
	token() {
		return tok;
	}

	Ast_AssignmentStmt::
	Ast_AssignmentStmt(Token o, const AstList<Ast_Expr *> &n,
	                   const AstList<Ast_Expr *> &e) : Ast_Stmt(AstAssignStmt) {
		op = o;
		names = n;
		exprs = e;
	}

	Ast_AssignmentStmt::
	~Ast_AssignmentStmt() {
		delete_content(names)
		delete_content(exprs)
	}

	void
	Ast_AssignmentStmt::
	print(int indent) {
		Ast_Node::print(indent);
		op.print(indent + 1);
		print_content(names)
		print_content(exprs)
	}

	const token::Token &
	Ast_AssignmentStmt::
	token() {
		return op;
	}

	Ast_BlockStmt::
	Ast_BlockStmt(const AstList<Ast_Stmt *> &s) : Ast_Stmt(AstBlockStmt) {
		stmts = s;
	}

	Ast_BlockStmt::
	~Ast_BlockStmt() {
		delete_content(stmts)
	}

	void
	Ast_BlockStmt::
	print(int indent) {
		Ast_Node::print(indent);
		print_content(stmts)
	}

	const token::Token &
	Ast_BlockStmt::
	token() {
		// return
		return emptyReturn;
	}

	Ast_IfStmt::
	Ast_IfStmt(Token t, Ast_Expr *c, Ast_Stmt *b, Ast_Stmt *ef) : Ast_Stmt(AstIfStmt) {
		tok = t;
		cond = c;
		body = b;
		else_if = ef;
	}

	Ast_IfStmt::
	~Ast_IfStmt() {
		delete cond;
		delete body;
		delete else_if;
	}

	void
	Ast_IfStmt::
	print(int indent) {
		Ast_Node::print(indent);
		cond->print(indent + 1);
		body->print(indent + 1);
		if (else_if) else_if->print(indent + 1);
	}

	const token::Token &
	Ast_IfStmt::
	token() {
		return tok;
	}

	Ast_ForStmt::
	Ast_ForStmt(token::Token t, Ast_Expr *c, Ast_Stmt *b) : Ast_Stmt(AstForStmt) {
		tok = t;
		cond = c;
		body = b;
	}

	Ast_ForStmt::
	~Ast_ForStmt() {
		delete cond;
		delete body;
		delete body;
	}

	void
	Ast_ForStmt::
	print(int indent) {
		Ast_Node::print(indent);
		cond->print(indent + 1);
		body->print(indent + 1);
	}

	const token::Token &
	Ast_ForStmt::
	token() {
		return tok;
	}

	Ast_WhileStmt::
	Ast_WhileStmt(Token t, Ast_Expr *c, Ast_Stmt *b) : Ast_Stmt(AstWhileStmt) {
		tok = t;
		cond = c;
		body = b;
	}

	Ast_WhileStmt::
	~Ast_WhileStmt() {
		delete cond;
		delete body;
	}

	void
	Ast_WhileStmt::
	print(int indent) {
		Ast_Node::print(indent);
		cond->print(indent + 1);
		body->print(indent + 1);
	}

	const token::Token &
	Ast_WhileStmt::
	token() {
		return tok;
	}

	Ast_ReturnStmt::
	Ast_ReturnStmt(Token t, const AstList<Ast_Expr *> v) : Ast_Stmt(AstReturnStmt) {
		tok = t;
		values = v;
	}

	Ast_ReturnStmt::
	~Ast_ReturnStmt() {
		delete_content(values);
	}

	void
	Ast_ReturnStmt::
	print(int indent) {
		Ast_Node::print(indent);
		print_content(values);
	}

	const token::Token &
	Ast_ReturnStmt::
	token() {
		return tok;
	}

	Ast_DeferStmt::
	Ast_DeferStmt(Token t, Ast_Expr *e) : Ast_Stmt(AstDeferStmt) {
		tok = t;
		expr = e;
	}

	Ast_DeferStmt::
	~Ast_DeferStmt() {
		delete expr;
	}

	void
	Ast_DeferStmt::
	print(int indent) {
		//TODO(): implement
		Ast_Node::print(indent);
		expr->print(indent + 1);
	}

	const token::Token &
	Ast_DeferStmt::
	token() {
		return tok;
	}

	Ast_Expr::
	Ast_Expr(AstNodeKind k) : Ast_Node(k) {}

	Ast_CompoundLiteral::
	Ast_CompoundLiteral(Token t, const AstList<Ast_Expr *> &lits, Ast_Type *type) : Ast_Expr(AstCompoundLiteral) {
		tok = t;
		literals = lits;
		this->type = type;
	}

	Ast_CompoundLiteral::
	~Ast_CompoundLiteral() {
		delete_content(literals)
	}

	void
	Ast_CompoundLiteral::
	print(int indent) {
		Ast_Node::print(indent);
		print_content(literals)
		if (type) type->print(indent + 1);
	}

	const token::Token &
	Ast_CompoundLiteral::
	token() {
		return tok;
	}

	Ast_FuncCall::
	Ast_FuncCall(Ast_Expr *n, Ast_Type *t, const AstList<Ast_Expr *> &a) : Ast_Expr(AstFuncCall) {
		name = n;
		type = t;
		actuals = a;
	}

	Ast_FuncCall::
	~Ast_FuncCall() {
		//TODO(): implement
		delete name;
		delete type;
		delete_content(actuals)
	}

	void
	Ast_FuncCall::
	print(int indent) {
		//TODO(): implement
		Ast_Node::print(indent);
		name->print(indent + 1);
		if (type) type->print(indent + 1);
		print_content(actuals);
	}

	const token::Token &
	Ast_FuncCall::
	token() {
		return name->token();
	}

	Ast_UnaryExpr::
	Ast_UnaryExpr(Token t, Ast_Expr *e) : Ast_Expr(AstUnaryExpr) {
		op = t;
		expr = e;
	}

	Ast_UnaryExpr::
	~Ast_UnaryExpr() {
		delete expr;
	}

	void
	Ast_UnaryExpr::
	print(int indent) {
		Ast_Node::print(indent);
		op.print(indent + 1);
		expr->print(indent + 1);
	}

	const token::Token &
	Ast_UnaryExpr::
	token() {
		return op;
	}

	Ast_BinaryExpr::
	Ast_BinaryExpr(Token t, Ast_Expr *l, Ast_Expr *r) : Ast_Expr(AstBinaryExpr) {
		op = t;
		lhs = l;
		rhs = r;
	}

	Ast_BinaryExpr::
	~Ast_BinaryExpr() {
		//TODO(): implement
		delete lhs;
		delete rhs;
	}

	void
	Ast_BinaryExpr::
	print(int indent) {
		//TODO(): implement
		Ast_Node::print(indent);
		op.print(indent + 1);
		lhs->print(indent + 1);
		rhs->print(indent + 1);
	}

	const token::Token &
	Ast_BinaryExpr::
	token() {
		return op;
	}

	Ast_ParenExpr::
	Ast_ParenExpr(Token b, Token e, Ast_Expr *ex) : Ast_Expr(AstParenExpr) {
		begin = b;
		end = e;
		expr = ex;
	}


	Ast_ParenExpr::
	~Ast_ParenExpr() {
		delete expr;
	}

	void
	Ast_ParenExpr::
	print(int indent) {
		//TODO(): implement
		Ast_Node::print(indent);
		expr->print(indent + 1);
	}

	const token::Token &
	Ast_ParenExpr::
	token() {
		return begin;
	}

// this is for array and map indexing
	Ast_IndexExpr::
	Ast_IndexExpr(Token b, Token e, Ast_Expr *op, Ast_Expr *i) : Ast_Expr(AstIndexExpr) {
		begin = b;
		end = e;
		operand = op;
		index = i;
	}

	Ast_IndexExpr::
	~Ast_IndexExpr() {
		delete operand;
		delete index;
	}

	void
	Ast_IndexExpr::
	print(int indent) {
		//TODO(): implement
		Ast_Node::print(indent);
		operand->print(indent + 1);
		index->print(indent + 1);
	}

	const token::Token &
	Ast_IndexExpr::
	token() {
		return begin;
	}

// Ast_SliceExpr
//	void print(int indent = 0) {
//	}
//	const token::Token& token(){
//	}

	Ast_DerefExpr::
	Ast_DerefExpr(Token t, Ast_Expr *e) : Ast_Expr(AstDerefExpr) {
		tok = t;
		expr = e;
	}

	Ast_DerefExpr::
	~Ast_DerefExpr() {
		delete expr;
	}

	void
	Ast_DerefExpr::
	print(int indent) {
		//TODO(): implement
		Ast_Node::print(indent);
		expr->print(indent + 1);
	}

	const token::Token &
	Ast_DerefExpr::
	token() {
		return tok;
	}

	Ast_SelectorExpr::
	Ast_SelectorExpr(Ast_Expr *ex, Ast_Expr *el) : Ast_Expr(AstSelectorExpr) {
		expr = ex;
		elem = el;
	}

	Ast_SelectorExpr::
	~Ast_SelectorExpr() {
		delete expr;
		delete elem;
	}

	void
	Ast_SelectorExpr::
	print(int indent) {
		//TODO(): implement
		Ast_Node::print(indent);
		expr->print(indent + 1);
		elem->print(indent + 1);
	}

	const token::Token &
	Ast_SelectorExpr::
	token() {
		return expr->token();
	}

	Ast_CastExpr::
	Ast_CastExpr(Ast_Type *t, Ast_Expr *e) : Ast_Expr(AstCastExpr) {
		type = t;
		expr = e;
	}

	Ast_CastExpr::
	~Ast_CastExpr() {
		delete type;
		delete expr;
	}

	void
	Ast_CastExpr::
	print(int indent) {
		Ast_Node::print(indent);
		type->print(indent + 1);
		expr->print(indent + 1);
	}

	const token::Token &
	Ast_CastExpr::
	token() {
		return type->token();
	}

	Ast_AddressExpr::
	Ast_AddressExpr(Token o, Ast_Expr *e) : Ast_Expr(AstAddressExpr) {
		op = o;
		expr = e;
	}

	Ast_AddressExpr::
	~Ast_AddressExpr() {
		delete expr;
	}

	void
	Ast_AddressExpr::
	print(int indent) {
		//TODO(): implement
		Ast_Node::print(indent);
		expr->print(indent + 1);
	}

	const token::Token &
	Ast_AddressExpr::
	token() {
		return op;
	}

	Ast_IncDecExpr::
	Ast_IncDecExpr(Token o, Ast_Expr *e) : Ast_Expr(AstIncDecExpr) {
		op = o;
		expr = e;
	}

	Ast_IncDecExpr::
	~Ast_IncDecExpr() {
		delete expr;
	}

	void
	Ast_IncDecExpr::
	print(int indent) {
		//TODO(): implement
		Ast_Node::print(indent);
		op.print(indent + 1);
		expr->print(indent + 1);
	}

	const token::Token &
	Ast_IncDecExpr::
	token() {
		return op;
	}

	Ast_NewExpr::
	Ast_NewExpr(Token t, Ast_Type *ty, const AstList<Ast_Expr *> &i) : Ast_Expr(AstNewExpr) {
		tok = t;
		type = ty;
		initExpr = i;
	}

	Ast_NewExpr::
	~Ast_NewExpr() {
		//TODO(): implement
		delete type;
		delete_content(initExpr)
	}


	void
	Ast_NewExpr::
	print(int indent) {
		Ast_Node::print(indent);
		type->print(indent + 1);
		print_content(initExpr)
	}

	const token::Token &
	Ast_NewExpr::
	token() {
		return tok;
	}

	Ast_DeleteExpr::
	Ast_DeleteExpr(Token t, Ast_Expr *e) : Ast_Expr(AstDeleteExpr) {
		tok = t;
		expr = e;
	}

	Ast_DeleteExpr::
	~Ast_DeleteExpr() {
		delete expr;
	}

	void
	Ast_DeleteExpr::
	print(int indent) {
		//TODO(): implement
		Ast_Node::print(indent);
		expr->print(indent + 1);
	}

	const token::Token &
	Ast_DeleteExpr::
	token() {
		return tok;
	}

	Ast_NamedLiteral::Ast_NamedLiteral(Ast_Identifier *name, Ast_Expr *value) : Ast_Expr(AstNamedLiteral) {
		this->name = name;
		this->value = value;
	}

	void Ast_NamedLiteral::print(int indent) {
		Ast_Node::print(indent);
		name->print(indent + 1);
		value->print(indent + 1);
	}

	const token::Token &Ast_NamedLiteral::token() {
		return name->token();
	}

	Ast_Type::
	Ast_Type(AstNodeKind k) : Ast_Node(k) {}

	Ast_NamedType::
	Ast_NamedType(Ast_Expr *id, const AstList<Ast_Type*>& ty) : Ast_Type(AstNamedType) {
		idExpr = id;
		type_parameters = ty;
	}

	Ast_NamedType::
	~Ast_NamedType() {
		delete idExpr;
		delete_content(type_parameters);
	}

	void
	Ast_NamedType::
	print(int indent) {
		//TODO(): implement
		Ast_Node::print(indent);
		idExpr->print(indent + 1);
		print_content(type_parameters);
	}

	const token::Token &
	Ast_NamedType::
	token() {
		return idExpr->token();
	}

	Ast_PrimativeType::
	Ast_PrimativeType(Token t) : Ast_Type(AstPrimativeType) {
		type = t;
	}

	void
	Ast_PrimativeType::
	print(int indent) {
		Ast_Node::print(indent);
		type.print(indent + 1);
	}

	const token::Token &
	Ast_PrimativeType::
	token() {
		return type;
	}

	Ast_ProcReturn::
	Ast_ProcReturn(AstNodeKind kind) : Ast_Node(kind) {}

	Ast_NamedReturn::
	Ast_NamedReturn(Ast_FieldSpec *f) : Ast_ProcReturn(AstNamedReturn) {
		field = f;
	}

	Ast_NamedReturn::
	~Ast_NamedReturn() {
		delete field;
	}

	void
	Ast_NamedReturn::
	print(int indent) {
		Ast_Node::print(indent);
		field->print(indent + 1);
	}

	const token::Token &
	Ast_NamedReturn::
	token() {
		return field->token();
	}

	Ast_TypedReturn::
	Ast_TypedReturn(Ast_Type *t) : Ast_ProcReturn(AstTypeReturn) {
		type = t;
	}

	Ast_TypedReturn::
	~Ast_TypedReturn() {
		delete type;
	}

	void
	Ast_TypedReturn::
	print(int indent) {
		Ast_Node::print(indent);
		type->print(indent + 1);
	}

	const token::Token &
	Ast_TypedReturn::
	token() {
		return type->token();
	}

	Ast_ProcType::
	Ast_ProcType(Token t, AstList<Ast_FieldSpec *> p,
	             AstList<Ast_ProcReturn *> r, FunctionFlags f, AstNodeKind kind) : Ast_Type(kind) {
		tok = t;
		params = p;
		returns = r;
		flags = f;
	}

	Ast_ProcType::
	~Ast_ProcType() {
		delete_content(params)
		delete_content(returns)
	}

	void
	Ast_ProcType::
	print(int indent) {
		Ast_Node::print(indent);
		print_content(params)
		print_content(returns)
	}

	const token::Token &
	Ast_ProcType::
	token() {
		return tok;
	}


	Ast_FunctionType::
	Ast_FunctionType(Token t, AstList<Ast_FieldSpec *> p,
	                 AstList<Ast_ProcReturn *> r, FunctionFlags f) : Ast_ProcType(t, p, r, f, AstFunctionType) {
	}


	Ast_MethodType::
	Ast_MethodType(Token t, Ast_Type *c, bool consts, AstList<Ast_FieldSpec *> p,
	               AstList<Ast_ProcReturn *> r, FunctionFlags f) : Ast_ProcType(t, p, r, f, AstMethodType) {
		classOf = c;
		this->consts = consts;
	}

	Ast_MethodType::
	~Ast_MethodType() {
		delete classOf;
	}

	void
	Ast_MethodType::
	print(int indent) {
		Ast_ProcType::print(indent + 1);
		classOf->print(indent + 1);
	}

	const token::Token &
	Ast_MethodType::
	token() {
		return Ast_ProcType::token();
	}


	Ast_PointerType::
	Ast_PointerType(Token t, Ast_Type *ty) : Ast_Type(AstPointerType) {
		tok = t;
		type = ty;
	}

	Ast_PointerType::
	~Ast_PointerType() {
		delete type;
	}

	void
	Ast_PointerType::
	print(int indent) {
		Ast_Node::print(indent);
		type->print(indent + 1);
	}

	const token::Token &
	Ast_PointerType::
	token() {
		return tok;
	}

	Ast_ArrayType::
	Ast_ArrayType(Token t, Ast_Type *ty, Ast_Expr *s) : Ast_Type(AstArrayType) {
		tok = t;
		type = ty;
		size = s;
	}

	Ast_ArrayType::
	~Ast_ArrayType() {
		delete size;
		delete type;
	}

	void
	Ast_ArrayType::
	print(int indent) {
		Ast_Node::print(indent);
		size->print(indent + 1);
		type->print(indent + 1);
	}

	const token::Token &
	Ast_ArrayType::
	token() {
		return tok;
	}

	Ast_DynamicArrayType::
	Ast_DynamicArrayType(Token t, Ast_Type *ty) : Ast_Type(AstDynamicArrayType) {
		tok = t;
		type = ty;
	}

	Ast_DynamicArrayType::
	~Ast_DynamicArrayType() {
		delete type;
	}

	void
	Ast_DynamicArrayType::
	print(int indent) {
		Ast_Node::print(indent);
		type->print(indent + 1);
	}

	const token::Token &
	Ast_DynamicArrayType::
	token() {
		return tok;
	}

	Ast_MapType::
	Ast_MapType(Token t, Ast_Type *k, Ast_Type *v) : Ast_Type(AstMapType) {
		tok = t;
		key = k;
		value = v;
	}

	Ast_MapType::
	~Ast_MapType() {
		delete key;
		delete value;
	}

	void
	Ast_MapType::
	print(int indent) {
		Ast_Node::print(indent);
		key->print(indent + 1);
		value->print(indent + 1);
	}

	const token::Token &
	Ast_MapType::
	token() {
		return tok;
	}

	Ast_EnumType::
	Ast_EnumType(Token t, const AstList<Ast_Decl *> &m) : Ast_Type(AstEnumType) {
		tok = t;
		members = m;
	}

	Ast_EnumType::
	~Ast_EnumType() {
		delete_content(members)
	}

	void
	Ast_EnumType::
	print(int indent) {
		Ast_Node::print(indent);
		print_content(members)
	}

	const token::Token &
	Ast_EnumType::
	token() {
		return tok;
	}

	Ast_UnionType::
	Ast_UnionType(Token t, const AstList<Ast_Decl *> &m) : Ast_Type(AstUnionType) {
		tok = t;
		members = m;
	}

	Ast_UnionType::
	~Ast_UnionType() {
		delete_content(members)
	}

	void
	Ast_UnionType::
	print(int indent) {
		Ast_Node::print(indent);
		print_content(members)
	}

	const token::Token &
	Ast_UnionType::
	token() {
		return tok;
	}

	Ast_ClassType::

	Ast_ClassType(token::Token t, const AstList<Ast_Decl *> &d, const AstList<Ast_Type *> &e,
	              const AstList<Ast_PolymorphicType*>& pt)
			: Ast_Type(AstClassType) {
		tok = t;
		decls = d;
		extends = e;
		poly_params = pt;
	}

	Ast_ClassType::
	~Ast_ClassType() {
		delete_content(extends)
		delete_content(decls);
		delete_content(poly_params);
	}

	void
	Ast_ClassType::
	print(int indent) {
		Ast_Node::print(indent);
		print_content(extends)
		print_content(decls)
		print_content(poly_params)
	}

	const token::Token &
	Ast_ClassType::
	token() {
		return tok;
	}

	Ast_StructType::
	Ast_StructType(Token t, const AstList<Ast_Decl *> &d, const AstList<Ast_PolymorphicType*>& pt) : Ast_Type(AstStructType) {
		tok = t;
		decls = d;
		poly_params = pt;
	}

	Ast_StructType::
	~Ast_StructType() {
		delete_content(decls);
	}

	void
	Ast_StructType::
	print(int indent) {
		Ast_Node::print(indent);
		print_content(decls);
		print_content(poly_params)
	}

	const token::Token &
	Ast_StructType::
	token() {
		return tok;
	}

	Ast_RangeExpr::Ast_RangeExpr(Ast_Expr *start, Ast_Expr *end, bool inclusive) : Ast_Expr(AstRangeExpr) {
		this->inclusive = inclusive;
		this->start = start;
		this->end = end;
	}

	void Ast_RangeExpr::print(int indent) {
		Ast_Expr::print(indent);
		start->print(indent + 1);
		end->print(indent + 1);
	}

	const token::Token &Ast_RangeExpr::token() {
		return start->token();
	}

	Ast_InExpr::Ast_InExpr(Ast_Expr *value, Ast_Expr *expr) : Ast_Expr(AstInExpr) {
		this->value = value;
		this->expr = expr;
	}

	void Ast_InExpr::print(int indent) {
		Ast_Expr::print(indent);
		value->print(indent + 1);
		expr->print(indent + 1);
	}

	const token::Token &Ast_InExpr::token() {
		return value->token();
	}

	Ast_SelectedTypedExpr::Ast_SelectedTypedExpr(const token::Token &tok, Ast_SelectedTypedExpr *parent, Ast_Type *elem) :
			Ast_Expr(AstSelectedTypeExpr) {
		this->tok = tok;
		this->parent = parent;
		this->elem = elem;
	}

	void Ast_SelectedTypedExpr::print(int indent) {
		Ast_Expr::print(indent);
		if (parent) parent->print(indent + 1);
		elem->print(indent + 1);
	}

	const token::Token &Ast_SelectedTypedExpr::token() {
		return Ast_Expr::token();
	}

	Ast_TypeExpr::Ast_TypeExpr(Ast_Type *type) : Ast_Expr(AstTypeExpr){
		this->type = type;
	}

	void Ast_TypeExpr::print(int indent) {
		Ast_Expr::print(indent);
		type->print(indent + 1);
	}

	const token::Token &Ast_TypeExpr::token() {
		return type->token();
	}

	Ast_PolymorphicType::Ast_PolymorphicType(token::Token tok, Ast_Identifier *name, const AstList<Ast_Type *> &spec) :
		Ast_Type(AstPolyType) {
		this->tok = tok;
		this->name = name;
		this->specializations = spec;
	}

	Ast_PolymorphicType::~Ast_PolymorphicType() {
		delete name;
		delete_content(specializations);
	}

	void Ast_PolymorphicType::print(int indent) {
		Ast_Node::print(indent);
		name->print(indent + 1);
		print_content(specializations);
	}

	const token::Token &Ast_PolymorphicType::token() {
		return tok;
	}
} // ast
