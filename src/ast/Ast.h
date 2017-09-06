#ifndef AST_H_
#define AST_H_

#include <vector>

#include "utils/File.h"
#include "Atom.h"
#include "token/Token.h"
#include <typeinfo>

using token::Token;

class Scope;

namespace ast {


// needed for AstFile
	struct Ast_Node;
	struct Ast_ImportSpec;
	struct Ast_Expr;
	struct Ast_FieldSpec;
	struct Ast_Decl;
	struct Ast_Type;
	struct Ast_Stmt;
	struct Ast_ProcType;
	struct Ast_Identifier;
	struct Ast_PolymorphicType;

// struct AstNode;
// typedef std::vector<AstNode*> AstNodeList;


	typedef unsigned int FunctionFlags;
#define FunctConst 1 << 0
#define FunctMember 1 << 1
#define FunctInline 1 << 2
#define FunctNotInline 1 << 3
#define FunctForiegn 1 << 4
#define FunctStatic 1 << 10
#define FunctPublic 1 << 11
#define FunctPrivate 1 << 12
#define FunctGeneric 1 << 13

	typedef unsigned int VariableFlags;
#define VarConst 1 << 0
#define VarHasIgnore 1 << 1
#define VarInferType 1 << 4
#define VarPublic 1 << 10
#define VarPrivate 1 << 11
#define VarStatic 1 << 12
#define VarGenaric 1 << 13

	typedef unsigned int TypeFlags;
#define TypeConst 1 << 0
#define TypeRef 1 << 1
#define TypeGeneric 1 << 4
#define TypeStruct 1 << 5
#define TypeClass 1 << 6
#define TypeArray 1 << 6
#define TypeDynArray 1 << 7
#define TypeEnum 1 << 8
#define TypeUnion 1 << 9


//typedef int FieldFlags;

	struct AstFile {
		// file info
		size_t m_id;
		sys::File *m_file;

		std::vector<ast::Ast_Stmt *> m_stmts;
		std::vector<ast::Ast_Decl *> m_decls;
		std::vector<ast::Ast_ImportSpec *> m_imports;
		Scope *scope{nullptr};

		std::string baseDir;
		// error counting
		size_t m_errorCount{0}, m_warningCount{0};
	};

	AstFile *ast_file(sys::File *_file);

// method and function call expr can problably be consolidated into one.
// What will distinguish it from a function will be the type pointer.
#define Ast_Node_Kinds \
  Ast_Node_Kind(NamedReturn, "named returns") \
  Ast_Node_Kind(TypeReturn, "type returns") \
Ast_Node_Kind(_BeginExpr, "")\
  Ast_Node_Kind(Ident, "identifier") \
  Ast_Node_Kind(BasicLit, "basic literal") \
  Ast_Node_Kind(NullLit, "null literal") \
  Ast_Node_Kind(BasicDirective, "basic directive")\
  Ast_Node_Kind(CompoundLiteral, "compound literal")\
  Ast_Node_Kind(BadExpr, "bad expression")\
  Ast_Node_Kind(FuncCall, "function call")\
  Ast_Node_Kind(UnaryExpr, "unary expression")\
  Ast_Node_Kind(BinaryExpr, "binary expression")\
  Ast_Node_Kind(ParenExpr, "parenthesis expression")\
  Ast_Node_Kind(IndexExpr, "index expression")\
  Ast_Node_Kind(SliceExpr, "slice expression")\
  Ast_Node_Kind(DerefExpr, "dereference expression")\
  Ast_Node_Kind(SelectorExpr, "selector expression")\
  Ast_Node_Kind(CastExpr, "cast expression")\
  Ast_Node_Kind(AddressExpr, "address expression")\
  Ast_Node_Kind(IncDecExpr, "increment decrement expression")\
  Ast_Node_Kind(NewExpr, "new expression")\
  Ast_Node_Kind(DeleteExpr, "delete expression")\
  Ast_Node_Kind(SelectedTypeExpr, "selected type expression")\
  Ast_Node_Kind(TypeExpr, "type expression") \
  Ast_Node_Kind(RangeExpr, "range expression") \
  Ast_Node_Kind(InExpr, "in expression") \
	Ast_Node_Kind(NamedLiteral, "named literal") \
Ast_Node_Kind(_EndExpr, "")\
Ast_Node_Kind(_BeginStmt, "")\
  Ast_Node_Kind(BadStmt, "bad statement")\
  Ast_Node_Kind(SpecStmt, "specificaiton statement") \
  Ast_Node_Kind(ExprStmt, "expression statement")\
  Ast_Node_Kind(EmptyStmt, "empty statement")\
  Ast_Node_Kind(BreakStmt, "break statement")\
  Ast_Node_Kind(ContinueStmt, "continue statement")\
  Ast_Node_Kind(AssignStmt, "assignment statement")\
  Ast_Node_Kind(_BeginComplexStmt, "")\
    Ast_Node_Kind(BlockStmt, "block statement")\
    Ast_Node_Kind(IfStmt, "if statement")\
    Ast_Node_Kind(ForStmt, "for statement")\
    Ast_Node_Kind(WhileStmt, "while statement")\
    Ast_Node_Kind(ReturnStmt, "return statement")\
    Ast_Node_Kind(DeferStmt, "defer statement")\
  Ast_Node_Kind(_EndComplexStmt, "")\
Ast_Node_Kind(_EndStmt, "")\
Ast_Node_Kind(_BeginDecl, "")\
  Ast_Node_Kind(BadDecl, "bad declaration")\
  Ast_Node_Kind(TypeAliasSpec, "type alias specificaiton")\
  Ast_Node_Kind(VariableSpec, "variable specificaiton")\
  Ast_Node_Kind(TypeSpec, "type specificaiton")\
  Ast_Node_Kind(ProcSpec, "procedure specificaiton")\
  Ast_Node_Kind(ImportSpec, "import specificaiton")\
  Ast_Node_Kind(MethodDeclBlock, "method declaration block")\
  Ast_Node_Kind(FieldSpec, "field spec")\
Ast_Node_Kind(_EndDecl, "")\
Ast_Node_Kind(_BeginType, "")\
  Ast_Node_Kind(BadType, "")\
  Ast_Node_Kind(NamedType, "named type")\
  Ast_Node_Kind(PrimativeType, "primative type")\
  Ast_Node_Kind(MethodType, "method type")\
  Ast_Node_Kind(FunctionType, "function type")\
  Ast_Node_Kind(PointerType, "pointer type")\
  Ast_Node_Kind(ArrayType, "array type")\
  Ast_Node_Kind(DynamicArrayType, "dynamic array type")\
  Ast_Node_Kind(MapType, "map type")\
  Ast_Node_Kind(EnumType, "enum type")\
  Ast_Node_Kind(UnionType, "union type")\
  Ast_Node_Kind(ClassType, "class type")\
  Ast_Node_Kind(StructType, "struct type") \
	Ast_Node_Kind(PolyType, "polymorphic type") \
	Ast_Node_Kind(TypeClass, "type class") \
Ast_Node_Kind(_EndType, "")


	enum AstNodeKind {
#define Ast_Node_Kind(_kind_name, ...) Ast##_kind_name,
		Ast_Node_Kinds
#undef Ast_Node_Kind
		Ast_Invalid,
		Ast_NumNodes
	};

	template<typename T>
	class AstList : public std::vector<T> {
		public:
		AstList() = default;

		bool all_of_type(AstNodeKind type) {
			for (auto i : *this)
				if (i->kind != type)
					return false;
			return true;
		}

		bool all_of_type(AstNodeKind type, int& index) {
			for(auto iter = this->begin(); iter < this->end(); ++iter) {
				if((*iter)->kind != type) {
					index = iter - this->begin();
					return false;
				}
			}
			return true;
		}

		template<typename ToType>
		AstList<ToType> convert_to() {
			AstList<ToType> list;
			for (auto i : *this) {
				auto n = dynamic_cast<ToType>(i);
				if (i)
					list.push_back(n);
				else {
					fprintf(stderr, "Compiler Error: Trying to convert a list of '%s' "
							"to a list of '%s'\n", typeid(T).name(), typeid(ToType).name());
					exit(1);
				}
			}
			return list;
		}
	};

	template<class T>
	inline void add_node(ast::AstList<T> &_list, const T &_node) {
		if(_node)
			_list.push_back(_node);
	}

	struct Ast_Node {
		AstNodeKind kind;

		Ast_Node(AstNodeKind kind);

		virtual ~Ast_Node();

		virtual void print(int indent = 0);

		const std::string &string();

		virtual const token::Token &token() = 0;

		static Token emptyReturn;
	};

	struct Ast_BadNode : public Ast_Node {
		Ast_BadNode(AstNodeKind k);

		virtual ~Ast_BadNode() {}

		virtual const token::Token &token() override;
	};


	struct Ast_Decl : public Ast_Node {
		// Ast_Identifier* name;

		Ast_Decl(AstNodeKind k);

		virtual ~Ast_Decl();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_TypeAlias : public Ast_Decl {
		token::Token tok;
		Ast_Identifier *alias;
		Ast_Type *type;

		Ast_TypeAlias(Ast_Identifier *a, Ast_Type *t);

		virtual ~Ast_TypeAlias();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_VariableSpec : public Ast_Decl {
		token::Token tok;
		AstList<Ast_Identifier *> names;
		AstList<Ast_Expr *> values;
		Ast_Type *type;
		VariableFlags flags;

		Ast_VariableSpec(token::Token t, AstList<Ast_Identifier *> n,
		                 AstList<Ast_Expr *> v, Ast_Type *ty, VariableFlags f = 0);

		virtual ~Ast_VariableSpec();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_TypeSpec : public Ast_Decl {
		Ast_Type *type;
		Ast_Identifier *name;

		Ast_TypeSpec(Ast_Identifier *n, Ast_Type *t);

		~Ast_TypeSpec();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_ProcSpec : public Ast_Decl {
		token::Token tok;
		Ast_ProcType *type;
		// maybe this should be Ast_BlockStmt
		// that is the only stmt that is allowed
		// as the body of a function.
		Ast_Stmt *body;
		Ast_Identifier *name;

		Ast_ProcSpec(token::Token t, Ast_Identifier *n, Ast_ProcType *ty, Ast_Stmt *b);

		virtual ~Ast_ProcSpec();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_ImportSpec : public Ast_Decl {
		token::Token relPath;
		std::string fullPath;
		Ast_Identifier *name;
		AstList<Ast_Identifier *> imports;
		size_t ast{(size_t) -1};
		bool valid{false};

		Ast_ImportSpec(token::Token p, const std::string &fp, Ast_Identifier *n,
		               const AstList<Ast_Identifier *> &i);

		virtual ~Ast_ImportSpec();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_FieldSpec : public Ast_Decl {
		token::Token tok;
		AstList<Ast_Identifier *> names;
		AstList<Ast_Expr *> defaults;
		Ast_Type *type;
		VariableFlags flags;

		Ast_FieldSpec(token::Token t, const AstList<Ast_Identifier *> &n, Ast_Type *ty, VariableFlags f = 0);

		virtual ~Ast_FieldSpec();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_Expr : public Ast_Node {
		Ast_Expr(AstNodeKind k);

		virtual void print(int indent = 0) override {}

		virtual const token::Token &token() override { return emptyReturn; }
	};

	struct Ast_Identifier : public Ast_Expr {
		token::Token tok;
		ast::Atom *name{nullptr};
		bool ignoreId{false};

		Ast_Identifier(token::Token token, ast::Atom *a);

		virtual ~Ast_Identifier();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_RangeExpr : public Ast_Expr {
		bool inclusive{false};
		Ast_Expr *start;
		Ast_Expr *end;

		Ast_RangeExpr(Ast_Expr *start, Ast_Expr *end, bool inclusive);

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_InExpr : public Ast_Expr {
		Ast_Expr *value;
		Ast_Expr *expr;

		Ast_InExpr(Ast_Expr *value, Ast_Expr *expr);

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_BasicLiteral : public Ast_Expr {
		token::Token tok;

		Ast_BasicLiteral(Token t);

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_NullLiteral : public Ast_Expr {
		token::Token tok;

		Ast_NullLiteral(Token t);

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_CompoundLiteral : public Ast_Expr {
		token::Token tok;
		AstList<Ast_Expr *> literals;
		Ast_Type *type;

		Ast_CompoundLiteral(Token t, const AstList<Ast_Expr *> &lits, Ast_Type *type = nullptr);

		~Ast_CompoundLiteral();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_NamedLiteral : public Ast_Expr {
		Ast_Identifier* name;
		Ast_Expr* value;

		Ast_NamedLiteral(Ast_Identifier* name, Ast_Expr* value);

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

// to represent an identifier being reference in an expression
// this is begin done because i do not want to have Ast_Identifier inheriate from Ast_Expr.
	struct Ast_Operand : public Ast_Expr {
		Ast_Identifier *name;

		Ast_Operand(Ast_Identifier *id);

		virtual ~Ast_Operand();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_FuncCall : public Ast_Expr {
		Ast_Expr *name;
		Ast_Type *type;
		AstList<Ast_Expr *> actuals;

		Ast_FuncCall(Ast_Expr *n, Ast_Type *t, const AstList<Ast_Expr *> &a);

		virtual ~Ast_FuncCall();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_UnaryExpr : public Ast_Expr {
		token::Token op;
		Ast_Expr *expr;

		Ast_UnaryExpr(token::Token t, Ast_Expr *e);

		virtual ~Ast_UnaryExpr();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_BinaryExpr : public Ast_Expr {
		token::Token op;
		Ast_Expr *lhs, *rhs;

		Ast_BinaryExpr(token::Token t, Ast_Expr *l, Ast_Expr *r);

		virtual ~Ast_BinaryExpr();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_ParenExpr : public Ast_Expr {
		token::Token begin, end;
		Ast_Expr *expr;

		Ast_ParenExpr(token::Token b, token::Token e, Ast_Expr *ex);

		virtual ~Ast_ParenExpr();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

// this is for array and map indexing
	struct Ast_IndexExpr : public Ast_Expr {
		token::Token begin, end;
		Ast_Expr *operand;
		Ast_Expr *index;

		Ast_IndexExpr(token::Token b, token::Token e, Ast_Expr *op, Ast_Expr *i);

		virtual ~Ast_IndexExpr();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_SliceExpr : public Ast_Expr {
		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_DerefExpr : public Ast_Expr {
		token::Token tok;
		Ast_Expr *expr;

		Ast_DerefExpr(token::Token t, Ast_Expr *e);

		virtual ~Ast_DerefExpr();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_SelectorExpr : public Ast_Expr {
		Ast_Expr *expr; // this would be Ast_Expr; however,
		// I need to include Ast_Identifier.
		Ast_Expr *elem;

		Ast_SelectorExpr(Ast_Expr *ex, Ast_Expr *el);

		virtual ~Ast_SelectorExpr();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_CastExpr : public Ast_Expr {
		Ast_Type *type;
		Ast_Expr *expr;

		Ast_CastExpr(Ast_Type *t, Ast_Expr *e);

		virtual ~Ast_CastExpr();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_AddressExpr : public Ast_Expr {
		token::Token op;
		Ast_Expr *expr; // same reason as Ast_SelectorExpr

		Ast_AddressExpr(token::Token o, Ast_Expr *e);

		virtual ~Ast_AddressExpr();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_IncDecExpr : public Ast_Expr {
		token::Token op;
		Ast_Expr *expr;

		Ast_IncDecExpr(token::Token o, Ast_Expr *e);

		virtual ~Ast_IncDecExpr();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_NewExpr : public Ast_Expr {
		token::Token tok;
		Ast_Type *type;
		AstList<Ast_Expr *> initExpr;

		Ast_NewExpr(token::Token t, Ast_Type *ty, const AstList<Ast_Expr *> &i);

		virtual ~Ast_NewExpr();


		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_DeleteExpr : public Ast_Expr {
		token::Token tok;
		Ast_Expr *expr;

		Ast_DeleteExpr(token::Token t, Ast_Expr *e);

		virtual ~Ast_DeleteExpr();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_SelectedTypedExpr : public Ast_Expr {
		token::Token tok;

		// the parent must be a selected type expression.
		// where the type element is a named type
		// and parent is null.
		Ast_SelectedTypedExpr *parent;

		Ast_Type *elem;

		Ast_SelectedTypedExpr(const token::Token &tok, Ast_SelectedTypedExpr *parent, Ast_Type *elem);

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_TypeExpr : public Ast_Expr {
		Ast_Type* type;

		Ast_TypeExpr(Ast_Type* type);

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_Stmt : public Ast_Expr {
		Ast_Stmt(AstNodeKind k);

		virtual void print(int indent = 0) override {}

		virtual const token::Token &token() override { return emptyReturn; }
	};

	struct Ast_SpecStmt : public Ast_Stmt {
		Ast_Decl *decl;

		Ast_SpecStmt(Ast_Decl *d);

		~Ast_SpecStmt();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_ExprStmt : public Ast_Stmt {
		Ast_Expr *expr;

		Ast_ExprStmt(Ast_Expr *e);

		virtual ~Ast_ExprStmt();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_EmptyStmt : public Ast_Stmt {
		Token tok;

		Ast_EmptyStmt(token::Token t);

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_BreakStmt : public Ast_Stmt {
		Token tok;

		Ast_BreakStmt(token::Token t);

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_ContinueStmt : public Ast_Stmt {
		token::Token tok;

		Ast_ContinueStmt(Token t);

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_AssignmentStmt : public Ast_Stmt {
		token::Token op;
		AstList<Ast_Expr *> names;
		AstList<Ast_Expr *> exprs;

		Ast_AssignmentStmt(token::Token o, const AstList<Ast_Expr *> &n,
		                   const AstList<Ast_Expr *> &e);

		virtual ~Ast_AssignmentStmt();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_BlockStmt : public Ast_Stmt {
		AstList<Ast_Stmt *> stmts;

		Ast_BlockStmt(const AstList<Ast_Stmt *> &s);

		virtual ~Ast_BlockStmt();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_IfStmt : public Ast_Stmt {
		token::Token tok;
		Ast_Expr *cond;
		Ast_Stmt *body;
		Ast_Stmt *else_if;

		Ast_IfStmt(token::Token t, Ast_Expr *c, Ast_Stmt *b, Ast_Stmt *ef);

		virtual ~Ast_IfStmt();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_ForStmt : public Ast_Stmt {
		token::Token tok;
		Ast_Expr *cond;
		Ast_Stmt *body;

		Ast_ForStmt(token::Token t, Ast_Expr *c, Ast_Stmt *b);

		virtual ~Ast_ForStmt();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_WhileStmt : public Ast_Stmt {
		token::Token tok;
		Ast_Expr *cond;
		Ast_Stmt *body;

		Ast_WhileStmt(token::Token t, Ast_Expr *c, Ast_Stmt *b);

		virtual ~Ast_WhileStmt();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_ReturnStmt : public Ast_Stmt {
		token::Token tok;
		AstList<Ast_Expr *> values;

		Ast_ReturnStmt(token::Token t, const AstList<Ast_Expr *> v);

		virtual ~Ast_ReturnStmt();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_DeferStmt : public Ast_Stmt {
		token::Token tok;
		Ast_Expr *expr;

		Ast_DeferStmt(token::Token t, Ast_Expr *e);

		virtual ~Ast_DeferStmt();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_Type : public Ast_Node {
		Ast_Type(AstNodeKind k);

		TypeFlags flags;
		// virtual void print(int indent = 0) override;
		// virtual const token::Token& token() override;
	};

	struct Ast_NamedType : public Ast_Type {
		// this would be Ast_Identifier; however, this is to
		// express all named types used within a program.
		// For example, Ast_Identifier would be able to express
		// t.t1, where t is a file scope and t1 is the type within that file.
		Ast_Expr *idExpr;
		AstList<Ast_Type*> type_parameters;

		Ast_NamedType(Ast_Expr *id, const AstList<Ast_Type*>& typeP);

		virtual ~Ast_NamedType();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_PrimativeType : public Ast_Type {
		token::Token type;

		Ast_PrimativeType(token::Token t);


		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_ProcReturn : public Ast_Node {
		Ast_ProcReturn(AstNodeKind kind);

		virtual ~Ast_ProcReturn() {}

		virtual void print(int indent = 0) override {}

		virtual const token::Token &token() override { return emptyReturn; }
	};

	struct Ast_NamedReturn : public Ast_ProcReturn {
		Ast_FieldSpec *field;

		Ast_NamedReturn(Ast_FieldSpec *f);

		virtual ~Ast_NamedReturn();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_TypedReturn : public Ast_ProcReturn {
		Ast_Type *type;

		Ast_TypedReturn(Ast_Type *t);

		virtual ~Ast_TypedReturn();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

// parent type for function and methods
	struct Ast_ProcType : public Ast_Type {
		token::Token tok;
		AstList<Ast_FieldSpec *> params;
		AstList<Ast_ProcReturn *> returns;
		FunctionFlags flags;

		Ast_ProcType(token::Token t, AstList<Ast_FieldSpec *> p,
		             AstList<Ast_ProcReturn *> r, FunctionFlags f, AstNodeKind kind);

		virtual ~Ast_ProcType();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

/*
  add :: (a, b: f32) -> f32 {
    return a + b;
  }
 */
	struct Ast_FunctionType : public Ast_ProcType {
		Ast_FunctionType(token::Token t, AstList<Ast_FieldSpec *> p,
		                 AstList<Ast_ProcReturn *> r, FunctionFlags f);
	};

/*
	add_vertex :: *TriangleMesh (vec: Vector3f) {

  }
 */
	struct Ast_MethodType : public Ast_ProcType {
		Ast_Type *classOf;
		bool consts;

		Ast_MethodType(token::Token t, Ast_Type *c, bool consts, AstList<Ast_FieldSpec *> p,
		               AstList<Ast_ProcReturn *> r, FunctionFlags f);

		virtual ~Ast_MethodType();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

/*
	root: *Ast_Node;
 */
	struct Ast_PointerType : public Ast_Type {
		token::Token tok;
		Ast_Type *type;

		Ast_PointerType(token::Token t, Ast_Type *ty);

		virtual ~Ast_PointerType();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

/*

 triangle: [3]Vector3f;

*/
	struct Ast_ArrayType : public Ast_Type {
		token::Token tok;
		Ast_Type *type;
		Ast_Expr *size;

		Ast_ArrayType(token::Token t, Ast_Type *ty, Ast_Expr *s);

		virtual ~Ast_ArrayType();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

/*

 vertices: [..]Vector3f;

*/
	struct Ast_DynamicArrayType : public Ast_Type {
		token::Token tok;
		Ast_Type *type;

		Ast_DynamicArrayType(token::Token t, Ast_Type *ty);

		virtual ~Ast_DynamicArrayType();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	struct Ast_MapType : public Ast_Type {
		token::Token tok;
		Ast_Type *key, *value;

		Ast_MapType(token::Token t, Ast_Type *k, Ast_Type *v);

		virtual ~Ast_MapType();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

/*
	Bar :: enum {

  }
*/
	struct Ast_EnumType : public Ast_Type {
		token::Token tok;
		Ast_Identifier *name;
		AstList<Ast_Decl *> members;

		Ast_EnumType(token::Token t, const AstList<Ast_Decl *> &m);

		virtual ~Ast_EnumType();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

/*
	Foo :: union {

  }

*/
	struct Ast_UnionType : public Ast_Type {
		token::Token tok;
		Ast_Identifier *name;
		AstList<Ast_Decl *> members;

		Ast_UnionType(token::Token t, const AstList<Ast_Decl *> &m);

		virtual ~Ast_UnionType();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

/*
	TriangleMesh :: class {
    vertices: [..]Vector3f;
    indices: [..]u64;

    #construct :: () {

    }

    #destruct :: () {

    }
  }

 */
	struct Ast_ClassType : public Ast_Type {
		token::Token tok;
		AstList<Ast_Decl *> decls; // class body decls
		AstList<Ast_Type *> extends; //
		AstList<Ast_PolymorphicType*> poly_params;

		Ast_ClassType(token::Token t, const AstList<Ast_Decl *> &d, const AstList<Ast_Type *> &e,
		              const AstList<Ast_PolymorphicType*>& pt);

		virtual ~Ast_ClassType();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

/*
 Vector3f :: struct {
    x, y, z: f32;
 }
 */

	struct Ast_StructType : public Ast_Type {
		token::Token tok;
		Ast_Identifier *name;
		AstList<Ast_Decl *> decls;
		AstList<Ast_PolymorphicType*> poly_params;

		Ast_StructType(token::Token t, const AstList<Ast_Decl *> &d, const AstList<Ast_PolymorphicType*>& pt);

		virtual ~Ast_StructType();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	/*
	  This function requires that both x and y are of the same
	  type, or the parameters can be implicitly premoted
	  to the said type.
	  add :: (x, y: $T) -> T {
	    return x + y;
	  }

	  This function says that x and y can be different types but
	  must both be of the Numeric type class.
	  add :: (x: $T/Integral + Ord, y: $Z/Numeric) -> T|Z {

	  }
	 */
	struct Ast_PolymorphicType : public Ast_Type {
		token::Token tok; // this will be the '$'
		Ast_Identifier *name;
		AstList<Ast_Type*> specializations;

		Ast_PolymorphicType(token::Token tok, Ast_Identifier* name, const AstList<Ast_Type*>& spec);

		virtual ~Ast_PolymorphicType();

		virtual void print(int indent = 0) override;

		virtual const token::Token &token() override;
	};

	/*
	struct Ast_TypeClass {
		AstList<Ast_FunctionType*> methods;
	};
	*/

}

#endif
