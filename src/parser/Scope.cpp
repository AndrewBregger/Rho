#include "Scope.h"
#include "Parser.h"

namespace parse {
	SymbolTable::
	SymbolTable(ast::AstNode* _node) {
		m_node = _node;
	}

	void
	SymbolTable::
	Add(ast::AstIdent* _name, ast::decl::AstDecl* _decl) {
		m_table.emplace(_name, _decl);
	}

	ast::decl::AstDecl*
	SymbolTable::
	Find(ast::AstIdent* _name) {
		auto iter = m_table.find(_name);
		if(iter == m_table.end())
			return nullptr;
		else
			return iter->second;
	}

	Scope::
	Scope(parse::Parser* _parser, ScopeKind _kind, ast::AstNode* _node) {
		m_parent = _parser->get_scope();
		m_depth = m_parent->GetDepth() + 1;	
		m_node = _node;
		m_kind = _kind;
		m_table = new SymbolTable(_node);
	}

	Scope::
	Scope(ScopeKind _kind, ast::AstNode* _node) {
		m_parent = nullptr;
		m_depth = 0;
		m_node = _node;
		m_kind = _kind;
		m_table = new SymbolTable(_node);
	}

	void
	Scope::
	AddDecl(ast::decl::AstDecl* _decl) {
		m_table->Add(_decl->identifier(), _decl);
	}

	ast::decl::AstDecl*
	Scope::
	FindDecl(ast::AstIdent* _name) {
		return m_table->Find(_name);
	}
} // parse
