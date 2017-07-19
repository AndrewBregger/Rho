///////////////////////////////////////////////////////////////////////////////
/// @group Parser
/// @ingroup Parser
///
/// @note A parser for the Rho programming language implemented as
///			  as a recursive desent parser.
///////////////////////////////////////////////////////////////////////////////

#ifndef PARSER_H_
#define PARSER_H_


#include <thread>
#include <mutex>

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
			Parser();

			/// @brief will parse all files needed by root file
			/// @param _file the root file to be parsed
			/// 			 (should contain main, if compiling executable)
			std::vector<ast::AstFile*> parse_files(sys::File* _file);
		private:
			ParseFileError init(sys::File* _file);

			/// @name Parsing utilities
			/// @{

			void next_token();
			Token peak_token(int _num = 1);
			token::Token expect_token(token::Token_Type _type);


			bool __check_token(const char* where, int line, token::Token_Type _type);
			bool __allow_token(const char* where, int line, token::Token_Type _type);

			bool expect_operator();

      bool expect_semicolon(const char* where);

			/// @}

			/// @name error correction
			/// @{
      void synchronize();
      /// @}

      enum StmtFlags : unsigned char {
      	StmtNoSemi = 0,
      	StmtExpectSemi = 1 << 0,
  			StmtDefault = StmtExpectSemi
      };

      enum FieldFlags : unsigned char {
      	FieldNoDefault = 0,
      	FieldAllowDefault = 1 << 0, // allows default initialization of the field
      	FieldAllowView = 1 << 2, // allows the field to have a view, pub keyword
      	FieldDefault = FieldAllowDefault
      };

			ast::AstFile* parse_file();

			bool setup_decls();

			void parse_imports_files();

			ast::AstNodeList parse_stmt_list();

			ast::AstNode* parse_stmt(StmtFlags flags = StmtDefault);

      ast::AstNode* parse_simple_stmt(StmtFlags flags);

      ast::AstNode* parse_while_stmt();

      ast::AstNode* parse_for_stmt();

			// this parameter is to handle else if cases
      ast::AstNode* parse_if_stmt(token::Token_Type expected);

      ast::AstNode* parse_match_stmt();

      ast::AstNode* parse_defer_stmt();

			ast::AstNode* parse_import_stmt();

			ast::AstNode* parse_return_stmt();

			ast::AstNode* parse_decl();

			ast::AstNode* parse_variable_decl();

			ast::AstNode* parse_type_decl(ast::AstNode* id);

			ast::AstNode* parse_function_decl(ast::AstNode* id);

			ast::AstNode* parse_function_type(ast::AstNode* id,
          ast::AstNode* reciever, bool isConst);

      ast::AstNode* parse_function_reciever(bool& isConst);

      ast::AstNodeList parse_function_return();

			ast::AstNode* parse_type_or_ident();

			ast::AstNode* parse_field(FieldFlags flags = FieldDefault);

      ast::AstNodeList parse_field_list(FieldFlags flags = FieldDefault);

      ast::AstNode* parse_body();

      ast::AstNode* parse_block_stmt();

			ast::AstNode* parse_operand(bool _lhs);

			ast::AstNode* parse_expr(bool _lhs);

			ast::AstNode* parse_lhs_expr();

			ast::AstNode* parse_rhs_expr();

			ast::AstNodeList parse_lhs_expr_list();

			ast::AstNodeList parse_rhs_expr_list();

			ast::AstNode* parse_primary_expr(bool _lhs);

			ast::AstNode* parse_binary_expr(bool _lhs, int _prec_in);

			ast::AstNode* parse_unary_expr(bool _lhs);

			ast::AstNode* parse_index_expr(ast::AstNode* operand);

			ast::AstNode* parse_call_expr(ast::AstNode* operand);

			ast::AstNode* parse_selector_expr(bool _lhs);

			ast::AstNode* parse_hash_directive();

			ast::AstNode* parse_identifier();

			ast::AstNodeList parse_identifier_list();

			ast::AstNode* parse_type();

      ast::AstNode* parse_field_tags(ast::AstNode* field);

      ast::AstNode* parse_function_tags(ast::AstNode* funct);

      ast::AstNode* parse_variable_tags(ast::AstNode* var, FieldFlags flags);

		private:

			ast::AstFile* m_ast;
			std::vector<ast::AstNode*> m_specs; // all specs defined in the parsed file.
		 	size_t m_errorCount{0}, m_warnCount{0};
			sys::File* m_file{nullptr};
			ast::AtomTable* m_atomTable{nullptr}; // @note(Andrew): Maybe this should
																						// be for the entire program no per file.
			std::vector<ast::AstFile*> m_asts;
	};
}

#endif // PARSER_H_
