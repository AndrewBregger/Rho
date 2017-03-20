#ifndef AST_IDENT_H_
#define AST_IDENT_H_

#include "AstNode.h"
#include "Atom.h"

namespace ast {

	class AstIdent : public ast::AstNode {
		public:
			AstIdent(const std::string& _name, const Location& _loc, ast::AstNode* _parent);
			AstIdent(ast::Atom* _atom, const Location& _loc, ast::AstNode* _parent);
			ast::Atom* name() { return m_atom; }
		private:
			ast::Atom* m_atom;
	};

} // ast

#endif // AST_IDENT_H_
