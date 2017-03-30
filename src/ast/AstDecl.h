#ifndef AST_DECL_H_
#define AST_DECL_H_
#include <vector>

#include "AstNode.h"
#include "Atom.h"
#include "AstIdent.h"
#include "type/Type.h"
#include "type/Types.h"
#include "utils/string.h"

namespace ast {
namespace decl {

	class AstDecl : public ast::AstNode {
		public: 
			AstDecl(ast::AstIdent* _name, ast::AstNode* _parent = nullptr);

			bool isFunction() { return false; }
			bool isMethod() 	{ return false; }
			bool isClass() 		{ return false; }
			bool isStruct() 	{ return false; }
			bool isLambda() 	{ return false; }
			bool isVariable() { return false; }
			AstIdent* identifier() { return m_name; }
			ast::Atom* name() { return m_name->name(); }

		private:
			AstIdent* m_name;
	};

	// this needs a type variable.
	class AstVariable : public AstDecl {
		public:
			AstVariable(ast::AstIdent* _name, type::Type* _type, bool _const, ast::AstNode* _parent = nullptr);
			bool isVariable() { return true; }
		private:
			bool m_constant;
			type::Type* m_type;
	};

	// this is special becuase it can have
	// a variable binded to it or not.
	// Example:
	//	-> i8
	//	-> a: i8
	class AstReturnVariable : public AstDecl {
		public:
			AstReturnVariable(ast::decl::AstVariable* _var);
			// this will be just the type constructor
			// AstReturnVariable(ast::decl::AstVariable* _var);
		private:
	};

	class AstFunction : public AstDecl {
		public:
			AstFunction(ast::AstIdent* _name,
				const std::vector<ast::decl::AstVariable*>& _sign, 
				const std::vector<ast::decl::AstReturnVariable*>& _return, 
				ast::AstNode* _parent = nullptr);
			virtual bool isFunction() { return true; }
		private:
			std::vector<AstVariable*> m_signiture;
			std::vector<AstReturnVariable*> m_returns;
	};

	class AstMethod : public AstFunction {
		public:
			AstMethod(ast::AstIdent* _name,
				const std::vector<ast::decl::AstVariable*>& _sign, 
				const std::vector<ast::decl::AstReturnVariable*>& _return, 
				ast::AstNode* _parent);
			bool isMethod() { return true; }
	};

	class AstClass : public AstDecl {
		public:
			AstClass(ast::AstIdent* _name, ast::AstNode* _parent);
			void AddStatic(ast::decl::AstFunction* _static);
			void AddMethod(ast::decl::AstMethod* _method);
			void AddInherited(ast::decl::AstClass* _parent);
			bool validate();
			bool isClass() { return true; }
		private:
      std::vector<ast::decl::AstClass*> m_parents;
      std::vector<ast::decl::AstMethod*> m_methods;
      std::vector<ast::decl::AstFunction*> m_staticFunction;
	};

	class AstLambda : public AstDecl {
		public:
			AstLambda() = default;
		private:
			std::vector<AstVariable*> m_signiture;
	};


	class AstLoad : public AstDecl {
		public:
			struct AstImportData {
				str::string m_relPath;
				str::string m_absPath;
				bool m_forign;
			};

			AstLoad();
		private:
			token::Token* m_string;
	};
	
} // decl
} // ast


#endif // AST_DECL_H_
