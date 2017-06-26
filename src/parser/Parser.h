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
#include "ast/Ast.h"
#include "token/Location.h"
#include "Scope.h"
#include "token/Token.h"
#include "ast/AtomTable.h"
#include "utils/File.h"

namespace parse {

	enum ParseFileError {
		ParseFile_None,

		ParseFile_Empty,
		ParseFile_Permissions,
		ParseFile_WrongExtension,
		ParseFile_InvalidFile,
		ParseFile_InvalidToken
	};

	class Parser {
		public:
			/// @brief Parser Constructor
			/// @param _file the root file to be parsed
			/// 			 (should contain main, if compiling executable)
			Parser(sys::File* _file);

			ast::AstFile* parse_files();
		private:
			ParseFileError init();
	
			/// @name Parsing utilities
			/// @{

			void next_token();
			Token peak_token(int _num = 1);
			token::Token expect_token(token::Token_Type _type);


			bool check_token(token::Token_Type _type);
			bool allow_token(token::Token_Type _type);

			/// @}

			ast::AstFile* parse_file();

			ast::AstNodeList parse_stmt_list();

			ast::AstNode* parse_stmt();

			ast::AstNode* parse_import_stmt();

			ast::AstNode* parse_decl_expr();

			ast::AstNode* parse_variable_decl();

			ast::AstNode* parse_type_decl();

			ast::AstNode* parser_function_spec();

			ast::AstNode* parse_type_or_ident();

			ast::AstNode* parse_field();

			ast::AstNode* parse_expr();

			ast::AstNode* parse_primary_expr();

			ast::AstNode* parser_selector_expr();

			ast::AstNode* parse_hash_directive();

			ast::AstNode* parse_identifier();

			ast::AstNodeList parse_identifier_list();

			ast::AstNode* parse_type();


		private:

			ast::AstFile* m_ast;
			std::vector<ast::AstNode*> m_specs; // all specs defined in the parsed file.
			std::vector<ast::AstNode*> m_imports;
		 	size_t m_errorCount{0}, m_warnCount{0};
			sys::File* m_file{nullptr};
			ast::AtomTable* m_atomTable{nullptr}; // @note(Andrew): Maybe this should
																						// be for the entire program no per file.	
	};
}

#endif // PARSER_H_
