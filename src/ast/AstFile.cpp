#include "AstFile.h"

namespace ast {
	AstFile::
	AstFile(sys::File* _file) : ast::decl::AstDecl( nullptr, nullptr) {
		m_file = _file;
	}

	void
	AstFile::
	AddDecl(ast::decl::AstDecl* _decl) {
		m_decls.push_back(_decl);
	}

	void
	AstFile::
	SetScope(parse::Scope* _scope) {
		m_scope = _scope;
	}
}