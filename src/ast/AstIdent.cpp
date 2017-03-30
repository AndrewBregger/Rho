#include "AstIdent.h"

namespace ast {

	AstIdent::
	AstIdent(const std::string& _name, const Location& _loc, ast::AstNode* _parent):
	ast::AstNode(_loc, _parent) {
		m_atom = new Atom(_name);
		m_atom = nullptr; //_name;
	}

	AstIdent::
	AstIdent(ast::Atom* _atom, const Location& _loc, ast::AstNode* _parent):
	ast::AstNode( _loc, _parent) {
		m_atom = _atom;
	}
	
} // ast