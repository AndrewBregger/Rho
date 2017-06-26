#include "Ast.h"
#include <iostream>

namespace ast {


//--------------------------------- Helper Functions --------------------------
bool ast_is_decl(AstNode* node) {
  return Ast__BeginDecl < node->kind && node->kind < Ast__EndDecl;
}

bool ast_is_expr(AstNode* node) {
  return Ast__BeginExpr < node->kind && node->kind < Ast__EndExpr;
}

bool ast_is_stmt(AstNode* node) {
  return Ast__BeginStmt < node->kind && node->kind < Ast__EndStmt;
}

bool ast_is_type(AstNode* node) {
  return Ast__BeginType < node->kind && node->kind < Ast__EndType;
}
//----------------------------------- Ast Creation -------------------------------
AstFile* ast_file(sys::File* _file) {
  static size_t ast_file_index = 0;
  AstFile* file = new AstFile;
  file->m_id = ast_file_index++;
  file->m_file = _file;
  return file;
}

const char* node_strings[Ast_NumNodes] = {
  #define Ast_Node_Kind(_kind, str, ...) str,
  Ast_Node_Kinds
  #undef Ast_Node_Kind
};

const char* ast_string(AstNodeKind _kind) {
  if(_kind >= Ast_Invalid)
    return nullptr;
  return node_strings[_kind];
}

AstNode* ast_node(AstNodeKind _kind) {
	AstNode* node = new AstNode; // not efficient, maybe a pool allocator
	node->kind = _kind;
	return node;
}

AstNode* ast_ident(Token token, Atom* atom) {
	AstNode* node = ast_node(Ast_Ident);
	node->Ident.token = token;
  node->Ident.atom = atom;
	return node;	
}

AstNode* ast_keyword(Token token) {
	AstNode* node = ast_node(Ast_Keyword);
	node->Keyword = token;
	return node;
}

AstNode* ast_basic_lit(Token token) {
	AstNode* node = ast_node(Ast_BasicLit);
	node->BasicLit = token;
	return node;
}

AstNode* ast_basic_directive(Token token, const std::string& name) {
	AstNode* node = ast_node(Ast_BasicDirective);
	node->BasicDirective.token = token;
	node->BasicDirective.name = name;
	return node;
}

AstNode* ast_compound_literal(Token begin, Token end, AstNode* type, const AstNodeList& literals) {
	AstNode* node = ast_node(Ast_CompoundLiteral);
	node->CompoundLiteral.begin = begin;
	node->CompoundLiteral.end = end;
	node->CompoundLiteral.type = type;
	return node;
}

AstNode* ast_bad_expr(Token begin, Token end) {
	AstNode* node = ast_node(Ast_BadExpr);
	node->BadExpr.begin = begin;
	node->BadExpr.end = end;
	return node;
}
AstNode* ast_func_call(Token begin, Token end, AstNode* name, AstNode* type, const AstNodeList& actuals) {
	AstNode* node = ast_node(Ast_FuncCall);
	node->FuncCall.begin = begin;
	node->FuncCall.end = end;
	node->FuncCall.name = name;
	node->FuncCall.type = type;
	node->FuncCall.actuals = actuals;
	return node;
}
AstNode* ast_method_call(Token begin, Token end, AstNode* name, AstNode* type, const AstNodeList& actuals) {
	AstNode* node = ast_node(Ast_MethodCall);
	node->MethodCall.begin = begin;
	node->MethodCall.end = end;
	node->MethodCall.name = name;
	node->MethodCall.type = type;
	node->MethodCall.actuals = actuals;
	return node;
}
AstNode* ast_unary_expr(Token op, AstNode* expr) {
	AstNode* node = ast_node(Ast_UnaryExpr);
	node->UnaryExpr.op = op;
	node->UnaryExpr.expr = expr;
	return node;
}
AstNode* ast_binary_expr(Token op, AstNode* lhs, AstNode* rhs) {
	AstNode* node = ast_node(Ast_BinaryExpr);
	node->BinaryExpr.op = op;
	node->BinaryExpr.lhs = lhs;
	node->BinaryExpr.rhs = rhs;
	return node;
}
AstNode* ast_paren_expr(Token begin, Token end, AstNode* expr) {
	AstNode* node = ast_node(Ast_ParenExpr);
	node->ParenExpr.begin = begin;
	node->ParenExpr.end = end;
	node->ParenExpr.expr = expr;
	return node;
}
AstNode* ast_incdec_expr(Token op, AstNode* expr) {
	AstNode* node = ast_node(Ast_IncDecExpr);
	node->IncDecExpr.op = op;
	node->IncDecExpr.expr = expr;
	return node;
}

AstNode* ast_index_expr(Token begin, Token end, AstNode* index) {
  AstNode* node = ast_node(Ast_IndexExpr);
  node->IndexExpr.begin = begin;
  node->IndexExpr.end = end;
  node->IndexExpr.index = index;
  return node;
}

AstNode* ast_slice_expr(Token begin, Token end, Token inclusion, AstNode* startExp, AstNode* endExp) {
  AstNode* node = ast_node(Ast_SliceExpr);
  node->SliceExpr.begin = begin;
  node->SliceExpr.end = end;
  node->SliceExpr.inclusion = inclusion;
  node->SliceExpr.startExp = startExp;
  node->SliceExpr.endExp = endExp;
  return node;
}

AstNode* ast_deref_expr(Token token, AstNode* expr) {
  AstNode* node = ast_node(Ast_DerefExpr);
  node->DerefExpr.token = token;
  node->DerefExpr.expr = expr;
  return node;
}

AstNode* ast_selector_expr(Token token, AstNode* elems) {
  AstNode* node = ast_node(Ast_SelectorExpr);
  node->SelectorExpr.token = token;
  node->SelectorExpr.elems = elems;
  return node;
}

AstNode* ast_cast_expr(Token token, AstNode* type, AstNode* expr) {
  AstNode* node = ast_node(Ast_CastExpr);
  node->CastExpr.token = token;
  node->CastExpr.type = type;
  node->CastExpr.expr = expr;
  return node;
}

AstNode* ast_bad_stmt() {
	AstNode* node = ast_node(Ast_BadExpr);
	return node;
}
AstNode* ast_expr_stmt(AstNode* expr) {
	AstNode* node = ast_node(Ast_ExprStmt);
	node->ExprStmt.expr = expr;
	return node;
}
AstNode* ast_empty_stmt(Token token) {
	AstNode* node = ast_node(Ast_EmptyStmt);
	node->EmptyStmt = token;
	return node;
}
AstNode* ast_assign_stmt(Token op, const AstNodeList& names, const AstNodeList& expr) {
	AstNode* node = ast_node(Ast_AssignStmt);
	node->AssignStmt.op = op;
	node->AssignStmt.names = names;
	node->AssignStmt.expr = expr;
	return node;
}
AstNode* ast_block_stmt(Token begin, Token end, const AstNodeList& stmts) {
  AstNode* node = ast_node(Ast_BlockStmt);
  node->BlockStmt.begin = begin;
  node->BlockStmt.end = end;
  node->BlockStmt.stmts = stmts;
  return node;
}
AstNode* ast_if_stmt(Token token, AstNode* cond, AstNode* body, AstNode* if_else) {
  AstNode* node = ast_node(Ast_IfStmt);
  node->IfStmt.token = token;
  node->IfStmt.cond = cond;
  node->IfStmt.body = body; 
  node->IfStmt.if_else_stmt = if_else;
  return node;
}
AstNode* ast_for_stmt(Token token, AstNode* init, AstNode* cond, AstNode* step, AstNode* body) {
  AstNode* node = ast_node(Ast_ForStmt);
  node->ForStmt.token = token;
  node->ForStmt.init = init; 
  node->ForStmt.cond = cond;
  node->ForStmt.step = step;
  node->ForStmt.body = body;
  return node;
}
AstNode* ast_while_stmt(Token token, AstNode* cond, AstNode* body) {
  AstNode* node = ast_node(Ast_WhileStmt);
  node->WhileStmt.token = token;
  node->WhileStmt.cond = cond;
  node->WhileStmt.body = body;
  return node;
}
AstNode* ast_return_stmt(Token token, AstNodeList expr) {
  AstNode* node = ast_node(Ast_ReturnStmt);
  node->ReturnStmt.token = token;
  node->ReturnStmt.expr = expr;
  return node;
}
AstNode* ast_defer_stmt(Token token, AstNode* name) {
  AstNode* node = ast_node(Ast_DeferStmt);
  node->DeferStmt.token = token;
  node->DeferStmt.name = name;
  return node;
}
AstNode* ast_bad_decl() {
  std::cout <<  "Bad Decl" << std::endl;
  AstNode* node = ast_node(Ast_BadDecl); 
  return node;
}
AstNode* ast_variable_spec(const AstNodeList& names, const AstNodeList& values, AstNode* type) {
  AstNode* node = ast_node(Ast_VariableSpec);
  node->VariableSpec.names = names;
  node->VariableSpec.values = values;
  node->VariableSpec.type = type;
  return node;
}
AstNode* ast_type_spec(AstNode* type) {
  AstNode* node = ast_node(Ast_TypeSpec);
  node->TypeSpec.type = type;
  return node;
}
AstNode* ast_funct_method_decl(Token token, AstNode* name, AstNode* type, AstNode* body) {
  AstNode* node = ast_node(Ast_FunctMethodDecl);
  node->FunctMethodDecl.token = token;
  node->FunctMethodDecl.name = name;
  node->FunctMethodDecl.type = type;
  node->FunctMethodDecl.body = body;
  return node;
}
AstNode* ast_import_spec(Token relPath, std::string fullPath, AstNode* name, const AstNodeList& importNames) {
  AstNode* node = ast_node(Ast_ImportSpec);
  node->ImportSpec.relPath = relPath;
  node->ImportSpec.fullPath = fullPath;
  node->ImportSpec.name = name;
  node->ImportSpec.importNames = importNames;
  return node;
}

AstNode* ast_field_spec(Token token, AstNode* name, AstNode* type) {
  AstNode* node = ast_node(Ast_FieldSpec);
  node->FieldSpec.token = token;
  node->FieldSpec.name = name;
  node->FieldSpec.name = type;
  return node;
}

AstNode* ast_bad_type() {
  AstNode* node = ast_node(Ast_BadType);
  return node;
}

AstNode* ast_helper_type(AstNode* type) {
  AstNode* node = ast_node(Ast_HelperType);
  node->HelperType.type = type;
  return node;
}

AstNode* ast_primative_type(Token token) {
  AstNode* node = ast_node(Ast_HelperType);
  node->PrimativeType = token;
  return node;
}

AstNode* ast_method_type(Token token, AstNode* name, const AstNodeList& params, const AstNodeList& returns) {
  AstNode* node = ast_node(Ast_MethodType);
  node->MethodType.token = token;
  node->MethodType.name = name;
  node->MethodType.params = params;
  node->MethodType.returns = returns;
  return node;
}
AstNode* ast_pointer_type(Token token, AstNode* type) {
  AstNode* node = ast_node(Ast_PointerType);
  node->PointerType.token = token;
  node->PointerType.type = type;
  return node;
}
AstNode* ast_array_type(Token token, AstNode* size, AstNode* type) {
  AstNode* node = ast_node(Ast_ArrayType);
  node->ArrayType.token = token;
  node->ArrayType.size = size;
  node->ArrayType.type = type;
  return node;
}
AstNode* ast_dynamic_array_type(Token token, AstNode* type) {
  AstNode* node = ast_node(Ast_DynamicArrayType);
  node->DynamicArrayType.token = token;
  node->DynamicArrayType.type = type;
  return node;
}
AstNode* ast_enum_type(Token token, AstNode* name, const AstNodeList& members) {
  AstNode* node = ast_node(Ast_StructType);
  node->EnumType.token = token;
  node->EnumType.name = name;
  node->EnumType.members = members;
  return node;
}
AstNode* ast_union_type(Token token, AstNode* name, const AstNodeList& members) {
  AstNode* node = ast_node(Ast_StructType);
  node->UnionType.token = token;
  node->UnionType.name = name;
  node->UnionType.members = members;
  return node;
}
AstNode* ast_class_type(Token token, AstNode* name, const AstNodeList& extends, const AstNodeList& members, const AstNodeList& methods) {
  AstNode* node = ast_node(Ast_ClassType);
  node->ClassType.token = token;
  node->ClassType.extends = extends;
  node->ClassType.name = name;
  node->ClassType.members = members;
  node->ClassType.methods = methods;
  return node;
}
AstNode* ast_struct_type(Token token, AstNode* name, const AstNodeList& members) { 
  AstNode* node = ast_node(Ast_StructType);
  node->StructType.token = token;
  node->StructType.name = name;
  node->StructType.members = members;
  return node;
}

} // ast