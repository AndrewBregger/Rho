#include "AstDecl.h"
#include "AstIdent.h"

namespace ast {
namespace decl {

	AstDecl::
	AstDecl(AstIdent* _name, ast::AstNode* _parent):
	ast::AstNode(_name->location(), _parent) {
		m_name = _name;
	}

	AstVariable::
	AstVariable(ast::AstIdent* _name, type::Type* _type, bool _const = false, ast::AstNode* _parent):
	AstDecl(_name, _parent) {
		m_constant = _const;
		m_type = _type;
	}
	/*
	AstFunction::	
	AstFunction(ast::AstIdent* _name, const std::vector<AstVariable*>& _sign,
		const std::vector<AstVariable*>& _return, ast::AstNode* _parent):
		AstDecl(_name, _parent) {
			m_signiture = _sign;
			m_returns = _return;
	}
	*/
	AstFunction::
	AstFunction(ast::AstIdent* _name,
				const std::vector<ast::decl::AstVariable*>& _sign, 
				const std::vector<ast::decl::AstReturnVariable*>& _return, 
				ast::AstNode* _parent): ast::decl::AstDecl(_name, _parent) {
			m_signiture = _sign;
			m_returns = _return;
	}
	
	AstMethod::
	AstMethod(ast::AstIdent* _name,
				const std::vector<ast::decl::AstVariable*>& _sign, 
				const std::vector<ast::decl::AstReturnVariable*>& _return, 
				ast::AstNode* _parent):
		AstFunction(_name, _sign, _return, _parent) {}

	AstClass::
	AstClass(ast::AstIdent* _name, ast::AstNode* _parent): AstDecl(_name, _parent) {}

	void
	AstClass::
	AddStatic(ast::decl::AstFunction* _static) {
		m_staticFunction.push_back(_static);
	}

	void
	AstClass::
	AddMethod(ast::decl::AstMethod* _method) {
		m_methods.push_back(_method);
	}

	void
	AstClass::
	AddInherited(ast::decl::AstClass* _parent) {
		m_parents.push_back(_parent);
	}

	AstLoad::
	AstLoad() : AstDecl(nullptr, nullptr) {
	}

}
} // ast