//
// Created by Andrew Bregger on 8/23/17.
//

#ifndef RHO_PARSER_H
#define RHO_PARSER_H

#include <stack>
#include "ast/Ast.h"
#include "ast/AtomTable.h"
#include "utils/File.h"
#include "token/Token.h"
#include "utils/report_errors.hpp"

using namespace ast;
using namespace token;
using sys::File;

namespace parser {

	struct ParserState {
		Token prev;
		Token token;
		size_t index;
		Ast_Node* parent;
		ErrorStats stats;
	};

	class Parser {
		public:

		/// @brief Constructor
		/// @{
		Parser();
		/// @}

		/// @brief Parser API
		/// @{
		AstFile* parse_file(File* file);
		/// @}
		private:

		/// @brief Token operations and checking
		/// @{

		/// @brief checks the current tokens type to given types.
		template <Token_Type type, Token_Type... types>
		inline bool check_tokens() {
			return check_token<type>() || check_tokens<types...>();
		}

		template <Token_Type type>
		inline bool check_tokens() {
			return check_token<type>();
		}

		/// @brief checks the current token to given type.
		/// @note is the base case for the recursive template definition.
		template <Token_Type type>
		inline bool check_token() {
			return type == token.token();
		}

		/// @brief checks the current tokens type to given types and consumes it.
		template <Token_Type... types>
		inline bool allow_tokens() {
			if(check_tokens<types...>()) {
				consume();
				return true;
			}
			return false;
		}

//		template <Token_Type type>
//		inline bool allow_tokens() {
//			if(check_token<type>()) {
//				consume();
//				return true;
//			}
//			return false;
//		}

		/// @brief checks the current token to given type.
		/// @note is the base case for the recursive template definition.
		template <Token_Type type>
		inline bool allow_token() {
			if(check_token<type>()) {
				consume();
				return true;
			}
			return false;
		}


		template<Token_Type type, typename... T>
		inline Token expect_token(const std::string& msg, T ... params) {
			Token p = token;
			if(p.token() == type) {
				consume();
			}
			else {
				if(msg.empty()) {
					if(type == TKN_IDENTIFIER)
						syntax_error(p, file, "expecting an %s: found '%s'",
						             Token(type, Special, Location()).string().c_str(),
						             p.error_string().c_str());
					else
						syntax_error(p, file, "expecting '%s': found '%s'",
					             Token(type, Special, Location()).string().c_str(),
					             p.error_string().c_str());
				}
				else
					syntax_error(p, file, msg, params...);
			}
			return p;
		}

		void consume();

		Token peek_token(int offset = 1);


		bool expect_operator();

		/// @brief initializes the parser state with the new file.
		bool init();

		void post_proccess();

		ParserState save_state();

		void restore_state(ParserState state);

		/// @brief syncs the state of the parser if an error ocurred
		///        finds the next statement to continue parsing.
		void sync();

		/// @}

		/// @brief Statement Parsing
		/// @{
		Ast_Stmt* parse_stmt();

		Ast_Stmt* parse_simple_stmt();

		Ast_IfStmt* parse_if_stmt();

		Ast_WhileStmt* parse_while_stmt();

		Ast_ForStmt* parse_for_stmt();

		Ast_DeferStmt* parse_defer_stmt();

		Ast_ReturnStmt* parse_return_stmt();

		Ast_BlockStmt* parse_block_stmt();

		Ast_Stmt* parse_hash_stmt();

//		Ast_MatchStmt* parse_match_stmt();
//		Ast_CaseStmt* parse_case_stmt();

		/// @}

		/// @brief Expression Parsing
		/// @{
		Ast_Expr* parse_expr(bool lhs = false);

		Ast_Expr* parse_binary_expr(int prec_in);

		Ast_Expr* parse_unary_expr();

		Ast_Expr* parse_operand();

		Ast_Expr* parse_atomic_expr();

		Ast_FuncCall* parse_function_call(Ast_Expr* operand);

		Ast_IndexExpr* parse_index_expr(Ast_Expr* operand);

		Ast_InExpr* parse_in_expr(Ast_Expr* operand);

		Ast_RangeExpr* parse_range_expr(Ast_Expr* operand);

		AstList<Ast_Expr*> parse_expr_list(bool lhs);

		Ast_Identifier* parse_identifier();

		AstList<Ast_Identifier*> parse_identifier_list();

		Ast_CompoundLiteral *parse_compound_literal(Ast_Expr *operand);

		Ast_Expr *parse_compound_element(Ast_Expr *operand);

		/// @}

		/// @brief Declaration Parsing
		/// @{
		Ast_Decl* parse_decl();

		Ast_VariableSpec *parse_variable_spec(AstList<Ast_Identifier *> ids);

		Ast_ProcSpec *parse_proc_spec(Ast_Identifier *id);

		Ast_TypeSpec *parse_type_spec(Ast_Identifier *id);

		Ast_FieldSpec* parse_field_spec();

		Ast_FieldSpec *parse_class_field();

		Ast_FieldSpec* parse_param_field();

		Ast_ImportSpec* parse_import_spec();

		/// @}

		/// @brief Type Parsing
		/// @{
		Ast_Type* parse_type();

		Ast_StructType* parse_struct_type();

		Ast_ClassType* parse_class_type();

		Ast_ProcType* parse_function_type(Ast_Type* reciever = nullptr, bool constant = true);

		AstList<Ast_ProcReturn*> parse_function_return();

		AstList<Ast_PolymorphicType*> parse_polymophic_type_list();
		/// @}

		/// @brief Parsing Helpers
		/// @{
		template <typename R, typename Functor>
		R parse_parenthesis(Functor funct) {
			if(check_token<TKN_LPAREN>()) {
				consume();
				R value = (this->*funct)();
				expect_token<TKN_RPAREN>("");
				return value;
			}
			return (this->*funct)();
		}
		/// @}
		private:
		Token token;
		Token prev;
		size_t index{0};

		std::vector<Token> tokens;
		std::vector<Token> comments;
		sys::File* file;
		Ast_Node* currParent{nullptr}; // null if in global scope
		ErrorStats* stats;

		bool inClassBody{false};
		bool inFunctionBody{false};

		AtomTable* atoms;
	};
}


#endif //RHO_PARSER_H
