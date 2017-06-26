///////////////////////////////////////////////////////////////////////////////
///
///
///
///////////////////////////////////////////////////////////////////////////////
#ifndef SCOPE_H_
#define SCOPE_H_
#include <map>

/*
namespace parse {
	class Parser;

	class SymbolTable {
		public:
			SymbolTable(ast::AstNode* _node);
			void Add(ast::AstIdent* _name, ast::decl::AstDecl* _decl);
			ast::decl::AstDecl* Find(ast::AstIdent* _name);
		private:
			ast::AstNode* m_node;
			std::map<ast::AstIdent*, ast::decl::AstDecl*> m_table;
	};
	enum ScopeKind {
		Global = 0,
		File,
		FunctionBody,
		EnumBody,
		UnionBody,
		StuctBody,
		ClassBody,
		ForParam,		
		Bracket, // covers if, while, loop, and for bodies
	};

	class Scope {
		public:
			Scope(ScopeKind _kind, ast::AstNode* _node = nullptr);
			Scope(parse::Parser* _parser, ScopeKind _kind, ast::AstNode* _node = nullptr);
			void AddDecl(ast::decl::AstDecl* _decl);
			ast::AstNode* GetNode() { return m_node; }
			ast::decl::AstDecl* FindDecl(ast::AstIdent* _name);
			size_t GetDepth() { return m_depth; }
			Scope* Parent() { return m_parent; }
		private:
			Scope* m_parent;
			size_t  m_depth;
			ast::AstNode* m_node;
			ScopeKind m_kind;
			SymbolTable* m_table;
	};

} // parse

*/
#endif // SCOPE_H_
