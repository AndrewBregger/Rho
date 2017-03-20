#ifndef ASTNODE_H_
#define ASTNODE_H_
#include "token/Location.h"

using token::Location;

namespace ast {
	// base class for the ast.
	class AstNode {
		public:
			AstNode(const Location& _loc, AstNode* _parent): m_parent(_parent), m_location(_loc) {}
			void setParent(AstNode* _parent) { m_parent = _parent; }	
			AstNode* parent() { return m_parent; }
			const Location& location() { return m_location; }
			
			bool isDecl() { return false; }
			bool isExpr() { return false; }
			bool isStmt() { return false; }

		private:
			AstNode* m_parent;
			Location m_location;
	};
}

#endif