#include "AstIdent.h"

namespace ast {

	AstIdent::
	AstIdent(const std::string& _name, const Location& _loc, ast::AstNode* _parent):
	ast::AstNode(_parent, _loc) {
		m_atom = nullptr; //_name;
	}

	AstIdent::
	AstIdent(ast::Atom* _atom, const Location& _loc, ast::AstNode* _parent):
	ast::AstNode(_parent, _loc) {
		m_atom = _atom;
	}
	
} // ast