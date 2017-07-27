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

#include <queue>
#include <unordered_map>
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
			Parser(ast::AtomTable* table = nullptr);

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

     	typedef int FieldFlags;
      #define	FieldNoDefault 0
      #define	FieldAllowDefault 1 << 0 // allows default initialization of the field
      #define	FieldAllowView 1 << 1 // allows the field to have a view, pub keyword
      #define	FieldNoView 1 << 2
      #define	FieldDefault FieldAllowDefault


			ast::AstFile* parse_file();

			bool setup_decls();

			void parse_imports_files();

			ast::AstList<ast::Ast_Stmt*> parse_stmt_list();

			ast::Ast_Stmt* parse_stmt(StmtFlags flags = StmtDefault);

      ast::Ast_Stmt* parse_simple_stmt(StmtFlags flags);

      ast::Ast_WhileStmt* parse_while_stmt();

      ast::Ast_ForStmt* parse_for_stmt();

			// this parameter is to handle else if cases
      ast::Ast_IfStmt* parse_if_stmt(token::Token_Type expected);

      ast::Ast_Stmt* parse_match_stmt();

      ast::Ast_DeferStmt* parse_defer_stmt();

			ast::Ast_ImportSpec* parse_import_stmt();

			ast::Ast_ReturnStmt* parse_return_stmt();

			ast::Ast_Decl* parse_decl();

			ast::Ast_VariableSpec* parse_variable_decl();

			ast::Ast_Type* parse_type_decl(ast::Ast_Identifier* id);

			ast::Ast_TypeSpec* parse_enum_union_struct();

			ast::Ast_ProcSpec* parse_function_decl(ast::Ast_Identifier* id);

			ast::Ast_ProcType* parse_function_type(ast::Ast_Identifier* id,
          ast::Ast_Type* reciever, bool isConst);

      ast::Ast_Type* parse_function_reciever(bool& isConst);

      ast::AstList<ast::Ast_ProcReturn*> parse_function_return();

			ast::Ast_Type* parse_type_or_ident();

			ast::Ast_FieldSpec* parse_field(FieldFlags flags = FieldDefault);

      ast::AstList<ast::Ast_FieldSpec*> parse_field_list(FieldFlags flags = FieldDefault);

      ast::Ast_BlockStmt* parse_body();

      ast::Ast_BlockStmt* parse_block_stmt();

			ast::Ast_Expr* parse_operand(bool _lhs);

			ast::Ast_Expr* parse_expr(bool _lhs);

			ast::Ast_Expr* parse_lhs_expr();

			ast::Ast_Expr* parse_rhs_expr();

			ast::AstList<ast::Ast_Expr*> parse_lhs_expr_list();

			ast::AstList<ast::Ast_Expr*> parse_rhs_expr_list();

			ast::Ast_Expr* parse_primary_expr(bool _lhs);

			ast::Ast_Expr* parse_binary_expr(bool _lhs, int _prec_in);

			ast::Ast_Expr* parse_unary_expr(bool _lhs);

			ast::Ast_IndexExpr* parse_index_expr(ast::Ast_Expr* operand);

			ast::Ast_FuncCall* parse_call_expr(ast::Ast_Expr* operand);

			ast::Ast_SelectorExpr* parse_selector_expr(bool _lhs);

			ast::Ast_Stmt* parse_hash_directive();

			ast::Ast_Identifier* parse_identifier();

			ast::AstList<ast::Ast_Identifier*> parse_identifier_list();

			ast::Ast_Type* parse_type();

      // ast::Ast_FieldSpec* parse_field_tags(ast::Ast_FieldSpec* field);

      ast::Ast_ProcType* parse_function_tags(ast::Ast_ProcType* funct);

      ast::Ast_FieldSpec* parse_variable_tags(ast::Ast_FieldSpec* var, FieldFlags flags);

		private:

			ast::AstFile* m_ast;
			std::vector<ast::Ast_Decl*> m_specs; // all specs defined in the parsed file.
		 	size_t m_errorCount{0}, m_warnCount{0};
			sys::File* m_file{nullptr};
			ast::AtomTable* m_atomTable{nullptr}; // @note(Andrew): Maybe this should
																						// be for the entire program no per file.
			std::vector<ast::AstFile*> m_asts; // this gets returned

			// used as intermediates when parsing mutltiple files.
			std::queue<sys::File*> parseQueue;
			std::unordered_map<size_t, ast::AstFile*> files;
	};
}

#endif // PARSER_H_
