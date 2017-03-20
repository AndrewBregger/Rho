#ifndef AST_DECL_H_
#define AST_DECL_H_
#include <vector>

#include "AstNode.h"
#include "Atom.h"

namespace ast {
	class AstIdent;
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

			ast::Atom* name() { return m_name->name(); }

		private:
			AstIdent* m_name;
	};

	// this needs a type variable.
	class AstVariable : public AstDecl {
		public:
			AstVariable(ast::AstIdent* _name, bool _const = false, ast::AstNode* _parent = nullptr);
			bool isVariable() { return true; }

		private:
			bool m_constant;
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
			AstFunction(ast::AstIdent* _name, const std::vector<AstVariable*> _sign, const std::vector<AstReturnVariable*> _return, ast::AstNode* _parent = nullptr);
		private:
			std::vector<AstVariable*> m_signiture;
			std::vector<AstReturnVariable*> m_returns;
	};
} // decl
} // ast


#endif // AST_DECL_H_
