#ifndef AST_FILE_H_
#define AST_FILE_H_

#include <vector>

#include "AstDecl.h"
#include "utils/File.h"
#include "parser/Scope.h"

namespace ast {

	class AstFile : ast::decl::AstDecl{

		public:
			AstFile(sys::File* _file);
			void AddDecl(ast::decl::AstDecl* _decl);
			void SetScope(parse::Scope* _scope);
		private:
			sys::File* m_file;
			// std::vector<ast::decl::AstImport*> m_imports;
			// std::vector<ast::decl::AstLibrary*> m_librarys;
			std::vector<ast::decl::AstDecl*> m_decls;

			parse::Scope* m_scope;
	};

} // ast


#endif // AST_FILE_H_
