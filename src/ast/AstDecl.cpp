#include "AstDecl.h"
#include "AstIdent.h"

namespace ast {
namespace decl {

	AstDecl::
	AstDecl(AstIdent* _name, ast::AstNode* _parent):
	ast::AstNode(_parent, _name->location()) {
		m_name = _name;
	}

	AstVariable::
	AstVariable(ast::AstIdent* _name, bool _const = false, ast::AstNode* _parent):
	ast::AstDecl(_name, _parent) {
		m_constant = _const;	
	}

	AstFunction::	
	AstFunction(ast::AstIdent* _name, const std::vector<AstVariable*> _sign, const std::vector<AstVariable*> _return, ast::AstNode* _parent = nullptr) {

	}
	
}
} // ast