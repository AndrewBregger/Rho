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

Token ast_token(AstNode* node) {
  Token token;
  switch(node->kind) {
    case Ast_Ident:
      token = node->Ident.token;
      break;
    case Ast_Keyword:
      token = node->Keyword;
      break;
    case Ast_BasicLit:
      token = node->BasicLit;
      break;
		case Ast_NullLit:
			token = node->NullLit;
    case Ast_BasicDirective:
      token = node->BasicDirective.token;
      break;
    case Ast_CompoundLiteral:
      token = node->CompoundLiteral.begin;
      break;
    case Ast_BadExpr:
      token = node->BadExpr.begin;
      break;
    case Ast_FuncCall:
      token = node->FuncCall.begin;
      break;
    case Ast_UnaryExpr:
      token = node->UnaryExpr.op;
      break;
    case Ast_BinaryExpr:
      token = node->BinaryExpr.op;
      break;
    case Ast_ParenExpr:
      token = node->ParenExpr.begin;
      break;
    case Ast_IndexExpr:
      token = node->IndexExpr.begin;
      break;
    case Ast_SliceExpr:
      token = node->SliceExpr.begin;
      break;
    case Ast_DerefExpr:
      token = node->DerefExpr.token;
      break;
    case Ast_SelectorExpr:
      token = node->SelectorExpr.token;
      break;
    case Ast_CastExpr:
      token = node->CastExpr.token;
      break;
    case Ast_IncDecExpr:
      token = node->IncDecExpr.op;
      break;
    // case Ast_BadStmt:
    //   token = node->BadStmt
    case Ast_ExprStmt:
      token = ast_token(node->ExprStmt.expr);
      break;
    case Ast_EmptyStmt:
      token = node->EmptyStmt;
      break;
    case Ast_AssignStmt:
      token = node->AssignStmt.op;
      break;
    case Ast_BlockStmt:
      token = node->BlockStmt.begin;
      break;
    case Ast_IfStmt:
      token = node->IfStmt.token;
      break;
    case Ast_ForStmt:
      token = node->ForStmt.token;
      break;
    case Ast_WhileStmt:
      token = node->WhileStmt.token;
      break;
    case Ast_ReturnStmt:
      token = node->ReturnStmt.token;
      break;
    case Ast_DeferStmt:
      token = node->DeferStmt.token;
      break;
    // case Ast_BadDecl:
    //   token = node->BadDecl
    case Ast_VariableSpec:
      token = node->VariableSpec.token;
      break;
    case Ast_TypeSpec:
      token = ast_token(node->TypeSpec.type);
      break;
    case Ast_FunctMethodDecl:
      token = node->FunctMethodDecl.token;
      break;
    case Ast_ImportSpec:
      token = node->ImportSpec.relPath;
      break;
    case Ast_MethodDeclBlock:
      token = node->MethodDeclBlock.token;
      break;
    case Ast_FieldSpec:
      token = node->FieldSpec.token;
      break;
    // case Ast_BadType:
    //   token = node->BadType
    case Ast_HelperType:
      token = ast_token(node->HelperType.type);
      break;
    case Ast_PrimativeType:
      token = node->PrimativeType;
      break;
    case Ast_MethodType:
      token = node->MethodType.token;
      break;
    case Ast_PointerType:
      token = node->PointerType.token;
      break;
    case Ast_ArrayType:
      token = node->ArrayType.token;
      break;
    case Ast_DynamicArrayType:
      token = node->DynamicArrayType.token;
      break;
    case Ast_EnumType:
      token = node->EnumType.token;
      break;
    case Ast_UnionType:
      token = node->UnionType.token;
      break;
    case Ast_ClassType:
      token = node->ClassType.token;
      break;
    case Ast_StructType:
      token = node->StructType.token;
      break;
    default:
      break;
  }
  return token;
}

void ast_print(AstNode* node, int indent) {
  std::cout << token::get_spaces(indent) << ast_string(node->kind) << std::endl;
  switch(node->kind) {
    case Ast_Ident: {
      auto data = node->Ident;
      std::cout << token::get_spaces(indent + 1) << data.atom->name() << std::endl;
    } break;
    case Ast_Keyword: {
      auto data = node->Keyword;
      data.print(indent + 1);
    } break;
    case Ast_BasicLit: {
      auto data = node->BasicLit;
      data.print(indent + 1);
    } break;
		case Ast_NullLit: {
      auto data = node->NullLit;
      data.print(indent + 1);
		} break;
    case Ast_BasicDirective: {
      auto data = node->BasicDirective;
      std::cout << token::get_spaces(indent + 1) << data.name << std::endl;
    } break;
    case Ast_CompoundLiteral: {
      auto data = node->CompoundLiteral;
      ast_print(data.type, indent + 1);
      for(const auto& e : data.literals)
        ast_print(e, indent + 1);
    } break;
    case Ast_BadExpr: {
    } break;
    case Ast_FuncCall: {
      auto data = node->FuncCall;
      ast_print(data.name, indent + 1);
      if(data.type)
        ast_print(data.type, indent + 1);
      for(const auto& e : data.actuals)
        ast_print(e, indent + 1);
    } break;
    case Ast_UnaryExpr: {
      auto data = node->UnaryExpr;
      data.op.print(indent + 1);
      ast_print(data.expr, indent + 1);
    } break;
    case Ast_BinaryExpr: {
      auto data = node->BinaryExpr;
      data.op.print(indent + 1);
      ast_print(data.lhs, indent + 1);
      ast_print(data.rhs, indent + 1);
    } break;
    case Ast_ParenExpr: {
      auto data = node->ParenExpr;
      ast_print(data.expr, indent + 1);
    } break;
    case Ast_IndexExpr: {
      auto data = node->IndexExpr;
      ast_print(data.operand, indent + 1);
      ast_print(data.index, indent + 1);
    } break;
    case Ast_SliceExpr: {
      auto data = node->SliceExpr;
      data.inclusion.print(indent + 1);
      ast_print(data.startExp, indent + 1);
      ast_print(data.endExp, indent + 1);
    } break;
    case Ast_DerefExpr: {
      auto data = node->DerefExpr;
      ast_print(data.expr, indent + 1);
    } break;
    case Ast_SelectorExpr: {
      auto data = node->SelectorExpr;
      std::cout << token::get_spaces(indent + 1) << "Epxr:\n";
      if(data.expr) ast_print(data.expr, indent + 1);
      std::cout << token::get_spaces(indent + 1) << "Elems:\n";
      if(data.elems) ast_print(data.elems, indent + 1);
      std::cout << token::get_spaces(indent + 1) << "Next:\n";
      if(data.next) ast_print(data.next, indent + 1);
    } break;
    case Ast_CastExpr: {
      auto data = node->CastExpr;
      ast_print(data.type, indent + 1);
      ast_print(data.expr, indent + 1);
    } break;
    case Ast_IncDecExpr: {
      auto data = node->IncDecExpr;
      data.op.print(indent + 1);
      ast_print(data.expr, indent + 1);
    } break;
    case Ast_NewExpr: {
      auto data = node->NewExpr;
      std::cout << token::get_spaces(indent + 1) << "Type:\n";
      ast_print(data.type, indent + 1);
      std::cout << token::get_spaces(indent + 1) << "Init:\n";
      for(auto e : data.initExpr)
        ast_print(e, indent + 1);
    } break;
    case Ast_DeleteExpr: {
      auto data = node->DeleteExpr;
      ast_print(data.name, indent + 1);
    } break;
    case Ast_BadStmt:
      break;
    case Ast_ExprStmt: {
      ast_print(node->ExprStmt.expr, indent + 1);
    } break;
    case Ast_EmptyStmt: {
      auto data = node->EmptyStmt;
      data.print(indent + 1);
    } break;
    case Ast_AssignStmt: {
      auto data = node->AssignStmt;
      data.op.print(indent + 1);
      for(const auto& e : data.names)
        ast_print(e, indent + 1);
      for(const auto& e : data.expr)
        ast_print(e, indent + 1);
    } break;
    case Ast_BlockStmt: {
      auto data = node->BlockStmt;
      for(const auto& e : data.stmts)
        ast_print(e, indent + 1);
    } break;
    case Ast_IfStmt: {
      auto data = node->IfStmt;
			std::cout << token::get_spaces(indent + 1) << "Condition:" << std::endl;
      if(data.cond) ast_print(data.cond, indent + 1);
			std::cout << token::get_spaces(indent + 1) << "Body:" << std::endl;
      if(data.body) ast_print(data.body, indent + 1);
			std::cout << token::get_spaces(indent + 1) << "else:" << std::endl;
      if(data.if_else_stmt) ast_print(data.if_else_stmt, indent + 1);
    } break;
    case Ast_ForStmt: {
      auto data = node->ForStmt;
      std::cout << token::get_spaces(indent + 1) << "Init:" << std::endl;
      if(data.init) ast_print(data.init, indent + 1);
      std::cout << token::get_spaces(indent + 1) << "Condition:" << std::endl;
      if(data.cond) ast_print(data.cond, indent + 1);
      std::cout << token::get_spaces(indent + 1) << "Step:" << std::endl;
      if(data.step) ast_print(data.step, indent + 1);
      std::cout << token::get_spaces(indent + 1) << "Body:" << std::endl;
      if(data.body) ast_print(data.body, indent + 1);
    } break;
    case Ast_WhileStmt: {
      auto data = node->WhileStmt;
      std::cout << token::get_spaces(indent + 1) << "Condition:" << std::endl;
      if(data.cond) ast_print(data.cond, indent + 1);
      std::cout << token::get_spaces(indent + 1) << "Body:" << std::endl;
      if(data.body) ast_print(data.body, indent + 1);
    } break;
    case Ast_ReturnStmt: {
      auto data = node->ReturnStmt;
      for(const auto& e : data.expr)
        ast_print(e, indent + 1);
    } break;
    case Ast_DeferStmt: {
      auto data = node->DeferStmt;
      ast_print(data.name, indent + 1);
    } break;
    case Ast_BadDecl:
      break;
    case Ast_VariableSpec: {
      auto data = node->VariableSpec;
      for(const auto& e : data.names)
        ast_print(e, indent + 1);
      for(const auto& e : data.values)
        ast_print(e, indent + 1);
      if(data.type)
        ast_print(data.type, indent + 1);
    } break;
    case Ast_TypeSpec: {
      auto data = node->TypeSpec;
      ast_print(data.type, indent + 1);
    } break;
    case Ast_FunctMethodDecl: {
      auto data = node->FunctMethodDecl;
      // this should have the type.
      // it is constructed when being parsed
      ast_print(data.type, indent + 1);
      ast_print(data.body, indent + 1);
    } break;
    case Ast_ImportSpec: {
      auto data = node->ImportSpec;
      data.relPath.print(indent + 1);
      std::cout << token::get_spaces(indent + 1) << data.fullPath << std::endl;
      if(data.name)
        ast_print(data.name, indent + 1);
      for(const auto& e : data.importNames)
        ast_print(e, indent + 1);
      std::cout << token::get_spaces(indent + 1) << "Ast: " << data.m_ast << std::endl;
    } break;
    case Ast_MethodDeclBlock: {
      // auto data = node->MethodDeclBlock.token;
    } break;
    case Ast_FieldSpec: {
      auto data = node->FieldSpec;
      for(const auto& e : data.name)
        ast_print(e, indent + 1);
      ast_print(data.type, indent + 1);
    } break;
    case Ast_TypeAliasSpec: {
      auto data = node->TypeAliasSpec;
      std::cout << token::get_spaces(indent + 1) << "Type:" << std::endl;
      ast_print(data.type, indent + 1);
      std::cout << token::get_spaces(indent + 1) << "Alias:" << std::endl;
      ast_print(data.alias, indent + 1);
    } break;
    case Ast_BadType:
      break;
    case Ast_HelperType: {
      auto data = node->HelperType;
      ast_print(data.type, indent + 1);
    } break;
    case Ast_PrimativeType: {
      auto data = node->PrimativeType;
      data.print(indent + 1);
    } break;
    case Ast_MethodType: {
      auto data = node->MethodType;
			if(data.name) ast_print(data.name, indent + 1);
			std::cout << token::get_spaces(indent + 1) << "Member:" << std::endl;
			if(data.classOf) ast_print(data.classOf, indent + 1);
			std::cout << token::get_spaces(indent + 1) << "Params:" << std::endl;
			for(const auto& e : data.params)
				ast_print(e, indent + 1);
			std::cout << token::get_spaces(indent + 1) << "Returns:" << std::endl;
			if(data.returns.empty())
				std::cout << token::get_spaces(indent + 2) << "void" << std::endl;
			else
				for(const auto& e : data.returns)
					ast_print(e, indent + 1);
			std::cout << token::get_spaces(indent + 1) << "Flags: " << data.flags << std::endl;
    } break;
    case Ast_FunctionType: {
      auto data = node->FunctionType;
			ast_print(data.name, indent + 1);
			std::cout << token::get_spaces(indent + 1) << "Params:" << std::endl;
			for(const auto& e : data.params)
				ast_print(e, indent + 1);
			std::cout << token::get_spaces(indent + 1) << "Return:" << std::endl;
			if(data.returns.empty())
				std::cout << token::get_spaces(indent + 2) << "void" << std::endl;
			else
				for(const auto& e : data.returns)
					ast_print(e, indent + 1);
			std::cout << token::get_spaces(indent + 1) << "Flags: " << data.flags << std::endl;
    } break;
    case Ast_PointerType: {
      auto data = node->PointerType;
			ast_print(data.type, indent + 1);
    } break;
    case Ast_ArrayType: {
      auto data = node->ArrayType;
			ast_print(data.size, indent + 1);
			ast_print(data.type, indent + 1);
    } break;
    case Ast_DynamicArrayType: {
      auto data = node->DynamicArrayType;
			ast_print(data.type, indent + 1);
    } break;
    case Ast_EnumType: {
      auto data = node->EnumType;
      std::cout << token::get_spaces(indent + 1) << "Name: " << std::endl;
     if(data.name) ast_print(data.name, indent + 1);
      else std::cout << token::get_spaces(indent + 1) << "Anonamous" << std::endl;

      std::cout << token::get_spaces(indent + 1) << "Members: " << std::endl;
      for(const auto& e : data.members)
       ast_print(e, indent + 1);
			// ast_print(data., indent + 1);
		// 	ast_print(data., indent + 1);
		//		ast_print(data., indent + 1);
    } break;
    case Ast_UnionType: {
      auto data = node->UnionType;
      std::cout << token::get_spaces(indent + 1) << "Name: " << std::endl;
      if(data.name) ast_print(data.name, indent + 1);
      else std::cout << token::get_spaces(indent + 1) << "Anonamous" << std::endl;

      std::cout << token::get_spaces(indent + 1) << "Members: " << std::endl;
      for(const auto& e : data.members)
       ast_print(e, indent + 1);
			// ast_print(data., indent + 1);
// 			ast_print(data., indent + 1);
// 			ast_print(data., indent + 1);
    } break;
    case Ast_ClassType: {
      auto data = node->ClassType;
      std::cout << token::get_spaces(indent + 1) << "Name: " << std::endl;
			if(data.name) ast_print(data.name, indent + 1);
      else std::cout << token::get_spaces(indent + 1) << "Anonamous" << std::endl;

      std::cout << token::get_spaces(indent + 1) << "Members: " << std::endl;
			for(const auto& e : data.members)
       ast_print(e, indent + 1);

      std::cout << token::get_spaces(indent + 1) << "Inherate From: " << std::endl;
			for(const auto& e : data.extends)
       ast_print(e, indent + 1);

      std::cout << token::get_spaces(indent + 1) << "Methods: " << std::endl;
      for(const auto& e : data.methods)
        ast_print(e, indent + 1);
    } break;
    case Ast_StructType: {
      auto data = node->StructType;
      std::cout << token::get_spaces(indent + 1) << "Name: " << std::endl;
      if(data.name) ast_print(data.name, indent + 1);
      else std::cout << token::get_spaces(indent + 1) << "Anonamous" << std::endl;

      std::cout << token::get_spaces(indent + 1) << "Members: " << std::endl;
      for(const auto& e : data.members)
       ast_print(e, indent + 1);
    } break;
    default:
      break;
  }
}

void ast_destroy(AstNode* node) {
	if(!node) return;
  switch(node->kind) {
    case Ast_Ident:
    case Ast_Keyword:
    case Ast_BasicLit:
    case Ast_BasicDirective:
		case Ast_NullLit:
      delete node;
    	break;
    case Ast_CompoundLiteral: {
      auto data = node->CompoundLiteral;
      ast_destroy(data.type);
      for(const auto& e : data.literals)
        ast_destroy(e);
    } break;
    case Ast_BadExpr: {
    } break;
    case Ast_FuncCall: {
      auto data = node->FuncCall;
      ast_destroy(data.type);
      for(const auto& e : data.actuals)
        ast_destroy(e);
    } break;
    case Ast_UnaryExpr: {
      auto data = node->UnaryExpr;
      ast_destroy(data.expr);
    } break;
    case Ast_BinaryExpr: {
      auto data = node->BinaryExpr;
      ast_destroy(data.lhs);
      ast_destroy(data.rhs);
    } break;
    case Ast_ParenExpr: {
      auto data = node->ParenExpr;
      ast_destroy(data.expr);
    } break;
    case Ast_IndexExpr: {
      auto data = node->IndexExpr;
      ast_destroy(data.operand);
      ast_destroy(data.index);
    } break;
    case Ast_SliceExpr: {
      auto data = node->SliceExpr;
      ast_destroy(data.startExp);
      ast_destroy(data.endExp);
    } break;
    case Ast_DerefExpr: {
      auto data = node->DerefExpr;
      ast_destroy(data.expr);
    } break;
    case Ast_SelectorExpr: {
      auto data = node->SelectorExpr;
			ast_destroy(data.expr);
			ast_destroy(data.elems);
			ast_destroy(data.next);
    } break;
    case Ast_CastExpr: {
      auto data = node->CastExpr;
      ast_destroy(data.type);
      ast_destroy(data.expr);
    } break;
    case Ast_IncDecExpr: {
      auto data = node->IncDecExpr;
      ast_destroy(data.expr);
    } break;
    case Ast_BadStmt:
      break;
    case Ast_ExprStmt: {
      ast_destroy(node->ExprStmt.expr);
    } break;
    case Ast_EmptyStmt: {
    } break;
    case Ast_AssignStmt: {
      auto data = node->AssignStmt;
      for(const auto& e : data.names)
        ast_destroy(e);
      for(const auto& e : data.expr)
        ast_destroy(e);
    } break;
    case Ast_BlockStmt: {
      auto data = node->BlockStmt;
      for(const auto& e : data.stmts)
        ast_destroy(e);
    } break;
    case Ast_IfStmt: {
      auto data = node->IfStmt;
			ast_destroy(data.cond);
			ast_destroy(data.body);
			ast_destroy(data.if_else_stmt);
    } break;
    case Ast_ForStmt: {
      auto data = node->ForStmt;
      ast_destroy(data.init);
      ast_destroy(data.cond);
      ast_destroy(data.step);
      ast_destroy(data.body);
    } break;
    case Ast_WhileStmt: {
      auto data = node->WhileStmt;
      ast_destroy(data.cond);
      ast_destroy(data.body);
    } break;
    case Ast_ReturnStmt: {
      auto data = node->ReturnStmt;
      for(const auto& e : data.expr)
        ast_destroy(e);
    } break;
    case Ast_DeferStmt: {
      auto data = node->DeferStmt;
      ast_destroy(data.name);
    } break;
    case Ast_BadDecl:
      break;
    case Ast_VariableSpec: {
      auto data = node->VariableSpec;
      for(const auto& e : data.names)
        ast_destroy(e);
      for(const auto& e : data.values)
        ast_destroy(e);
      ast_destroy(data.type);
    } break;
    case Ast_TypeSpec: {
      auto data = node->TypeSpec;
      ast_destroy(data.type);
    } break;
    case Ast_FunctMethodDecl: {
      auto data = node->FunctMethodDecl;
      // this should have the type.
      // it is constructed when being parsed
      ast_destroy(data.type);
      ast_destroy(data.body);
    } break;
    case Ast_ImportSpec: {
      auto data = node->ImportSpec;
      ast_destroy(data.name);
      for(const auto& e : data.importNames)
        ast_destroy(e);
    } break;
    case Ast_MethodDeclBlock: {
      // auto data = node->MethodDeclBlock.token;
    } break;
    case Ast_FieldSpec: {
      auto data = node->FieldSpec;
      for(const auto& e : data.name)
        ast_destroy(e);
      ast_destroy(data.type);
    } break;
    case Ast_TypeAliasSpec: {
      auto data = node->TypeAliasSpec;
      ast_destroy(data.type);
      ast_destroy(data.alias);
    }
    case Ast_BadType:
      break;
    case Ast_HelperType: {
      auto data = node->HelperType;
      ast_destroy(data.type);
    } break;
    case Ast_PrimativeType: {
    } break;
    case Ast_MethodType: {
      auto data = node->MethodType;
			ast_destroy(data.name);
			ast_destroy(data.classOf);
			for(const auto& e : data.params)
				ast_destroy(e);
			for(const auto& e : data.returns)
				ast_destroy(e);
    } break;
    case Ast_FunctionType: {
      auto data = node->FunctionType;
			ast_destroy(data.name);
			for(const auto& e : data.params)
				ast_destroy(e);
			for(const auto& e : data.returns)
				ast_destroy(e);
    } break;
    case Ast_PointerType: {
      auto data = node->PointerType;
			ast_destroy(data.type);
    } break;
    case Ast_ArrayType: {
      auto data = node->ArrayType;
			ast_destroy(data.size);
			ast_destroy(data.type);
    } break;
    case Ast_DynamicArrayType: {
      auto data = node->DynamicArrayType;
			ast_destroy(data.type);
    } break;
    case Ast_EnumType: {
      auto data = node->EnumType;
			// ast_destroy(data., indent + 1);
		// 	ast_destroy(data., indent + 1);
		//		ast_destroy(data., indent + 1);
    } break;
    case Ast_UnionType: {
      auto data = node->UnionType;
			// ast_destroy(data., indent + 1);
// 			ast_destroy(data., indent + 1);
// 			ast_destroy(data., indent + 1);
    } break;
    case Ast_ClassType: {
      auto data = node->ClassType;
			// ast_destroy(data., );
// 			ast_destroy(data., );
// 			ast_destroy(data., );
    } break;
    case Ast_StructType: {
      auto data = node->StructType;
			// ast_destroy(data., );
// 			ast_destroy(data., );
// 			ast_destroy(data., );
    } break;
    default:
      break;
  }
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

AstNode* ast_null_lit(Token token) {
	AstNode* node = ast_node(Ast_NullLit);
	node->NullLit = token;
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
AstNode* ast_func_call(Token begin, Token end, AstNode* name, const AstNodeList& actuals) {
	AstNode* node = ast_node(Ast_FuncCall);
	node->FuncCall.begin = begin;
	node->FuncCall.end = end;
	node->FuncCall.name = name;
	// node->FuncCall.type = type;
	node->FuncCall.actuals = actuals;
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

AstNode* ast_index_expr(Token begin, Token end, AstNode* operand, AstNode* index) {
  AstNode* node = ast_node(Ast_IndexExpr);
  node->IndexExpr.begin = begin;
  node->IndexExpr.end = end;
  node->IndexExpr.operand = operand;
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

AstNode* ast_selector_expr(Token token, AstNode* expr, AstNode* next, AstNode* elems) {
  AstNode* node = ast_node(Ast_SelectorExpr);
  node->SelectorExpr.token = token;
  node->SelectorExpr.expr = expr;
  node->SelectorExpr.next = next;
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

AstNode* ast_new_expr(Token token, AstNode* type, const AstNodeList& initExpr) {
  AstNode* node = ast_node(Ast_NewExpr);
  node->NewExpr.token = token;
  node->NewExpr.type = type;
  node->NewExpr.initExpr = initExpr;
  return node;
}

AstNode* ast_delete_expr(Token token, AstNode* name) {
  AstNode* node = ast_node(Ast_DeleteExpr);
  node->DeleteExpr.token = token;
  node->DeleteExpr.name = name;
  return node;
}

AstNode* ast_address_expr(Token token, AstNode* expr) {
  AstNode* node = ast_node(Ast_AddressExpr);
  node->AddressExpr.token = token;
  node->AddressExpr.expr = expr;
  return node;
}

AstNode* ast_bad_stmt() {
	AstNode* node = ast_node(Ast_BadExpr);
	return node;
}

AstNode* ast_break_stmt(Token token) {
  AstNode* node = ast_node(Ast_BreakStmt);
  node->BreakStmt = token;
  return node;
}

AstNode* ast_continue_stmt(Token token) {
  AstNode* node = ast_node(Ast_ContinueStmt);
  node->ContinueStmt = token;
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
  AstNode* node = ast_node(Ast_BadDecl);
  return node;
}
AstNode* ast_variable_spec(Token token, const AstNodeList& names, const AstNodeList& values, AstNode* type) {
  AstNode* node = ast_node(Ast_VariableSpec);
  node->VariableSpec.token = token;
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
AstNode* ast_funct_method_decl(Token token, AstNode* type, AstNode* body) {
  AstNode* node = ast_node(Ast_FunctMethodDecl);
  node->FunctMethodDecl.token = token;
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

AstNode* ast_field_spec(Token token, const AstNodeList& name, AstNode* type) {
  AstNode* node = ast_node(Ast_FieldSpec);
  node->FieldSpec.token = token;
  node->FieldSpec.name = name;
  node->FieldSpec.type= type;
  return node;
}

AstNode* ast_type_alias_spec(Token token, AstNode* type, AstNode* alias) {
  AstNode* node = ast_node(Ast_TypeAliasSpec);
  node->TypeAliasSpec.token = token;
  node->TypeAliasSpec.type = type;
  node->TypeAliasSpec.alias = alias;
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
  AstNode* node = ast_node(Ast_PrimativeType);
  node->PrimativeType = token;
  return node;
}

AstNode* ast_function_type(Token token, AstNode* name, const AstNodeList& params, const AstNodeList& returns) {
  AstNode* node = ast_node(Ast_FunctionType);
  node->FunctionType.token = token;
  node->FunctionType.name = name;
  node->FunctionType.params = params;
  node->FunctionType.returns = returns;
  return node;
}

AstNode* ast_method_type(Token token, AstNode* name, AstNode* classOf,
    const AstNodeList& params, const AstNodeList& returns) {
  AstNode* node = ast_node(Ast_MethodType);
  node->MethodType.token = token;
  node->MethodType.name = name;
  node->MethodType.classOf = classOf;
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

AstNode* ast_map_type(Token begin, Token end, AstNode* key, AstNode* value) {
  AstNode* node = ast_node(Ast_MapType);
  node->MapType.begin = begin;
  node->MapType.end = end;
  node->MapType.key = key;
  node->MapType.value = value;
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
