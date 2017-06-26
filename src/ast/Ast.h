#ifndef AST_H_
#define AST_H_

#include <vector>

#include "token/Token.h"
#include "utils/File.h"
#include "Atom.h"
#include "utils/string.h"

using token::Token;

class Scope;

namespace ast {

struct AstNode;
typedef std::vector<AstNode*> AstNodeList;

template <class T>
inline void add_node(std::vector<T>& _list, const T& _node) {
	_list.push_back(_node);
}

struct AstFile {
	// file info
	size_t m_id;
	sys::File* m_file;
		
	// parser info
	Token m_currToken;
	Token m_prevToken;
	size_t m_tokenIndex{0};
	size_t m_scopeLevel{0};

	std::vector<Token> m_tokens;
	std::vector<AstNode*> m_decls;
	AstNode* currParent{nullptr}; // null if in global scope
	Scope* scope{nullptr};

	// error counting
	size_t m_errorCount{0}, m_warningCount{0};
};

#define Ast_Node_Kinds \
	Ast_Node_Kind(Ident, "identifier", struct {Token token; Atom* atom;}) \
	Ast_Node_Kind(Keyword, "keyword", Token) \
	Ast_Node_Kind(BasicLit, "basic literal", Token) \
	Ast_Node_Kind(BasicDirective, "basic directive", struct { Token token; std::string name; }) \
	Ast_Node_Kind(CompoundLiteral, "compound literal", struct { Token begin, end; AstNode* type; AstNodeList literals; }) \
Ast_Node_Kind(_BeginExpr, "", int) \
	Ast_Node_Kind(BadExpr, "bad expression", struct { Token begin, end; }) \
	Ast_Node_Kind(FuncCall, "function call", struct { AstNode* name,* type; AstNodeList actuals; Token begin, end;}) \
	Ast_Node_Kind(MethodCall, "method call", struct { AstNode* name,* type; AstNodeList actuals; Token begin, end;}) \
	Ast_Node_Kind(UnaryExpr, "unary expression", struct {Token op; AstNode* expr; }) \
	Ast_Node_Kind(BinaryExpr, "binary expression", struct {Token op; AstNode* lhs,* rhs; }) \
	Ast_Node_Kind(ParenExpr, "parenthesis expression", struct { AstNode* expr; Token begin, end; }) \
	Ast_Node_Kind(IndexExpr, "index expression", struct { Token begin, end; AstNode* index;}) \
	Ast_Node_Kind(SliceExpr, "slice expression", struct { Token begin, end, inclusion; AstNode* startExp, *endExp;}) \
	Ast_Node_Kind(DerefExpr, "dereference expression", struct {Token token; AstNode* expr; }) \
	Ast_Node_Kind(SelectorExpr, "selector expression", struct {Token token; AstNode* elems; }) \
	Ast_Node_Kind(CastExpr, "cast expression", struct {Token token; AstNode* type,* expr; }) \
	Ast_Node_Kind(IncDecExpr, "increment decrement expression", struct { Token op; AstNode* expr; }) \
Ast_Node_Kind(_EndExpr, "", int) \
Ast_Node_Kind(_BeginStmt, "", int) \
	Ast_Node_Kind(BadStmt, "bad statement", int) \
	Ast_Node_Kind(ExprStmt, "expression statement", struct { AstNode* expr; }) \
	Ast_Node_Kind(EmptyStmt, "empty statement", Token) \
	Ast_Node_Kind(AssignStmt, "assignment statement", struct { AstNodeList names, expr; Token op; }) \
	Ast_Node_Kind(_BeginComplexStmt, "", int) \
		Ast_Node_Kind(BlockStmt, "block statement", struct { AstNodeList stmts; Token begin, end; }) \
		Ast_Node_Kind(IfStmt, "if statement", struct { Token token; AstNode* cond,* body, *if_else_stmt; }) \
		Ast_Node_Kind(ForStmt, "for statement", struct { Token token; AstNode* init, *cond, *step, *body; }) \
		Ast_Node_Kind(WhileStmt, "while statement", struct { Token token; AstNode* cond, *body; }) \
		Ast_Node_Kind(ReturnStmt, "return statement", struct { Token token; AstNodeList expr; }) \
		Ast_Node_Kind(DeferStmt, "defer statement", struct { Token token; AstNode* name; }) \
	Ast_Node_Kind(_EndComplexStmt, "", int) \
Ast_Node_Kind(_EndStmt, "", int) \
Ast_Node_Kind(_BeginDecl, "", int) \
	Ast_Node_Kind(BadDecl, "bad declaration", int) \
	Ast_Node_Kind(VariableSpec, "varialble specificaiton", struct { AstNodeList names, values; AstNode* type; }) \
	Ast_Node_Kind(TypeSpec, "type specificaiton", struct { AstNode* type; }) \
	Ast_Node_Kind(FunctMethodDecl, "function method declaration", struct { Token token; AstNode* name, *type, *body; }) \
	Ast_Node_Kind(ImportSpec, "import specificaiton", struct {Token relPath; std::string fullPath; AstNode* name; AstNodeList importNames;}) \
	Ast_Node_Kind(MethodDeclBlock, "method declaration block", struct {Token token; AstNode* type; AstNodeList methods; }) /*temorary construct for parsing methods*/ \
	Ast_Node_Kind(FieldSpec, "field spec", struct { Token token; AstNode* name; AstNode* type; }) \
Ast_Node_Kind(_EndDecl, "", int) \
Ast_Node_Kind(_BeginType, "", int) \
	Ast_Node_Kind(BadType, "", int) \
	Ast_Node_Kind(HelperType, "helper type", struct { AstNode* type; }) \
	Ast_Node_Kind(PrimativeType, "primative type", Token) \
	Ast_Node_Kind(MethodType, "method type", struct { Token token; AstNode* name; AstNodeList params, returns; }) \
	Ast_Node_Kind(PointerType, "pointer type", struct { Token token; AstNode* type; }) \
	Ast_Node_Kind(ArrayType, "array type", struct { Token token; AstNode* size, *type; }) \
	Ast_Node_Kind(DynamicArrayType, "dyanmic array type", struct { Token token; AstNode* type; }) \
	Ast_Node_Kind(EnumType, "enum type", struct { Token token; AstNode* name; AstNodeList members; }) \
	Ast_Node_Kind(UnionType, "union type", struct { Token token; AstNode* name; AstNodeList members; }) \
	Ast_Node_Kind(ClassType, "class type", struct { Token token; AstNode* name; AstNodeList extends, members, methods; }) /*add alignment and packing later*/ \
	Ast_Node_Kind(StructType, "struct type", struct { Token token; AstNode* name; AstNodeList members; }) /*add alignment and packing later*/\
Ast_Node_Kind(_EndType, "", int)


enum AstNodeKind {
#define Ast_Node_Kind(_kind_name, ...) Ast_##_kind_name,
	Ast_Node_Kinds
#undef Ast_Node_Kind
	Ast_Invalid,
	Ast_NumNodes
};

#define Ast_Node_Kind(_kind, name, ...) typedef __VA_ARGS__ Ast##_kind;
Ast_Node_Kinds
#undef Ast_Node_Kind

struct AstNode {
	AstNodeKind kind;
#define Ast_Node_Kind(_kind, ...) Ast##_kind _kind;
	Ast_Node_Kinds
#undef Ast_Node_Kind

};

const char* ast_string(AstNodeKind _kind);

bool ast_is_decl(AstNode* node);
bool ast_is_expr(AstNode* node);
bool ast_is_stmt(AstNode* node);
bool ast_is_type(AstNode* node);

AstFile* ast_file(sys::File* _file);

AstNode* ast_node(AstNodeKind _kind);

AstNode* ast_ident(Token token, Atom* atom);
AstNode* ast_keyword(Token token);
AstNode* ast_basic_lit(Token token);
AstNode* ast_basic_directive(Token token, const std::string& name);
AstNode* ast_compound_literal(Token begin, Token end, AstNode* type, const AstNodeList& literals);
AstNode* ast_bad_expr(Token begin, Token end);
AstNode* ast_func_call(Token begin, Token end, AstNode* name, AstNode* type, const AstNodeList& actuals);
AstNode* ast_method_call(Token begin, Token end, AstNode* name, AstNode* type, const AstNodeList& actuals);
AstNode* ast_unary_expr(Token op, AstNode* expr);
AstNode* ast_binary_expr(Token op, AstNode* lhs, AstNode* rhs);
AstNode* ast_paren_expr(Token begin, Token end, AstNode* expr);
AstNode* ast_incdec_expr(Token op, AstNode* expr);
AstNode* ast_index_expr(Token begin, Token end, AstNode* index);
AstNode* ast_slice_expr(Token begin, Token end, Token inclusion, AstNode* startExp, AstNode* endExp);
AstNode* ast_deref_expr(Token token, AstNode* expr);
AstNode* ast_selector_expr(Token token, AstNode* elems);
AstNode* ast_cast_expr(Token token, AstNode* type, AstNode* expr); 
AstNode* ast_bad_stmt();
AstNode* ast_expr_stmt(AstNode* expr);
AstNode* ast_empty_stmt(Token token);
AstNode* ast_assign_stmt(Token op, const AstNodeList& names, const AstNodeList& expr);
AstNode* ast_block_stmt(Token begin, Token end, const AstNodeList& stmts);
AstNode* ast_if_stmt(Token token, AstNode* cond, AstNode* body, AstNode* if_else);
AstNode* ast_for_stmt(Token token, AstNode* init, AstNode* cond, AstNode* step, AstNode* body);
AstNode* ast_while_stmt(Token token, AstNode* cond, AstNode* body);
AstNode* ast_return_stmt(Token token, AstNodeList expr);
AstNode* ast_defer_stmt(Token token, AstNode* name);
AstNode* ast_bad_decl();
AstNode* ast_variable_spec(const AstNodeList& names, const AstNodeList& values, AstNode* type);
AstNode* ast_type_spec(AstNode* type);
AstNode* ast_funct_method_decl(Token token, AstNode* name, AstNode* type, AstNode* body);
AstNode* ast_import_spec(Token relPath, std::string fullPath, AstNode* name, const AstNodeList& importNames);
AstNode* ast_field_spec(Token token, AstNode* name, AstNode* type);
AstNode* ast_bad_type();
AstNode* ast_helper_type(AstNode* type);
AstNode* ast_primative_type(Token token);
AstNode* ast_method_type(Token token, AstNode* name, const AstNodeList& params, const AstNodeList& returns);
AstNode* ast_pointer_type(Token token, AstNode* type);
AstNode* ast_array_type(Token token, AstNode* size, AstNode* type);
AstNode* ast_dynamic_array_type(Token token, AstNode* type);
AstNode* ast_enum_type(Token token, AstNode* name, const AstNodeList& members);
AstNode* ast_union_type(Token token, AstNode* name, const AstNodeList& members);
AstNode* ast_class_type(Token token, AstNode* name, const AstNodeList& extends, const AstNodeList& members, const AstNodeList& methods);
AstNode* ast_struct_type(Token token, AstNode* name, const AstNodeList& members);

}

#endif
