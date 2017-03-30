///////////////////////////////////////////////////////////////////////////////
/// @group Parser
/// @ingroup Parser
///
/// @note A parser for the Rho programming language implemented as
///			  as a recursive desent parser.
///////////////////////////////////////////////////////////////////////////////

#ifndef PARSER_H_
#define PARSER_H_
#include <stack>
#include "ast/AstFile.h"
#include "ast/AtomTable.h"
#include "token/Location.h"
#include "scanner/Scanner.h"
#include "Scope.h"
#include "token/Token.h"



namespace parse {
	class Parser {
		public:
			Parser(sys::File* _file);

			// scope operation
			void begin_scope(ScopeKind _kind);
			void end_scope();
			Scope* get_scope();
			ast::AstFile* parse_file();
			size_t depth();
		private:
			token::Token* consume_token();
			ast::decl::AstDecl* parse_decl();
			ast::decl::AstLoad* parse_load();

		private:
			std::stack<Token*> m_tokenStack;
			token::Token* m_nextToken{nullptr};
			token::Token* m_currentToken{nullptr};
			token::Token* m_previousToken{nullptr};
			Scope* m_scope{nullptr};
			scanner::Scanner* m_scanner{nullptr};
			size_t m_depth;
			sys::File* m_file;
			ast::AtomTable* m_atomTable;
	};
}

#endif // PARSER_H_
