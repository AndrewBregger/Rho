#ifndef TOKEN_H_
#define TOKEN_H_
#include "utils/string.h"
#include "Location.h"

namespace token {

#define Token_Types \
		Token_Type(TKN_COMMENT, "comment") \
		Token_Type(TKN_WHITESPACE, "") \
		Token_Type(TKN_ERROR, "error") \
		Token_Type(TKN_EOF, "eof") \
		Token_Type(TKN_EMPTY, "empty") \
		Token_Type(TKN_IDENTIFIER, "identifier") \
		Token_Type(TKN_KEYWORD, "keyword") \
		Token_Type(begin_literal, "") \
		Token_Type(TKN_LSTRING, "string literal") \
		Token_Type(TKN_LCHAR, "character literal") \
		Token_Type(TKN_LINT, "integer literal") \
		Token_Type(TKN_LDOUBLE, "floating point literal") \
		Token_Type(TKN_LBOOL, "bool literal") \
		Token_Type(end_literal, "") \
		Token_Type(begin_operator, "") \
		Token_Type(TKN_ADD, "+") \
		Token_Type(TKN_SUB, "-") \
		Token_Type(TKN_MULT, "*") \
		Token_Type(TKN_DIV, "/") \
		Token_Type(TKN_REM, "%") \
		Token_Type(TKN_POW, "**") \
		Token_Type(TKN_AT, "@") \
		Token_Type(TKN_AND, "&") \
		Token_Type(TKN_OR, "|") \
		Token_Type(TKN_XOR, "^") \
		Token_Type(TKN_NOT, "!") \
		Token_Type(TKN_SHTL, "<<") \
		Token_Type(TKN_SHTR, ">>") \
		Token_Type(TKN_ADDASN, "+=") \
		Token_Type(TKN_SUBASN, "-=") \
		Token_Type(TKN_MULTASN, "*=") \
		Token_Type(TKN_DIVASN, "/=") \
		Token_Type(TKN_REMASN, "%=") \
		Token_Type(TKN_XORASN, "^=") \
		Token_Type(TKN_ANDASN, "&=") \
		Token_Type(TKN_ORASN, "|=") \
		Token_Type(TKN_SHTLASN, "<<=") \
		Token_Type(TKN_SHTRASN, ">>=") \
		Token_Type(TKN_LITAND, "&&") \
		Token_Type(TKN_LITOR, "||") \
		Token_Type(TKN_RARR, "->") \
		Token_Type(TKN_LARR, "<-") \
		Token_Type(TKN_INC, "++") \
		Token_Type(TKN_DEC, "--") \
		Token_Type(TKN_ASN, "=") \
		Token_Type(TKN_DECL, ":=") \
		Token_Type(TKN_DCOL, "::") \
		Token_Type(TKN_LESS, "<") \
		Token_Type(TKN_GRET, ">") \
		Token_Type(TKN_LEQ, "<=") \
		Token_Type(TKN_GEQ, ">=") \
		Token_Type(TKN_EQ, "==") \
		Token_Type(TKN_NEQ, "!=") \
		Token_Type(TKN_ELS, "...") \
		Token_Type(TKN_LBRACK, "{") \
		Token_Type(TKN_RBRACK, "}") \
		Token_Type(TKN_LBRACE, "[") \
		Token_Type(TKN_RBRACE, "]") \
		Token_Type(TKN_LPAREN, "(") \
		Token_Type(TKN_RPAREN, ")") \
		Token_Type(TKN_PER, ".") \
		Token_Type(TKN_SEM, ";") \
		Token_Type(TKN_COM, ",") \
		Token_Type(TKN_COL, ":") \
		Token_Type(TKN_HASH, "#") \
		Token_Type(TKN_QEST, "?") \
		Token_Type(TKN_DOLLAR, "$") \
		Token_Type(TKN_DPER, "..") \
		Token_Type(end_operator, "") \
		Token_Type(begin_keyword, "") \
		Token_Type(TKN_CONST, "const") \
		Token_Type(TKN_MUT, "mut") \
		Token_Type(TKN_IF, "if") \
		Token_Type(TKN_FOR, "for") \
		Token_Type(TKN_ELSE, "else") \
		Token_Type(TKN_ELIF, "else if") \
		Token_Type(TKN_LOOP, "loop") \
		Token_Type(TKN_CLASS, "class") \
		Token_Type(TKN_STRUCT, "struct") \
		Token_Type(TKN_USING, "using") \
		Token_Type(TKN_NAMESPACE, "namespace") \
		Token_Type(TKN_RETURN, "return") \
		Token_Type(TKN_SWTICH, "switch") \
		Token_Type(TKN_DEFER, "defer") \
		Token_Type(TKN_WORKSPACE, "workspace") \
		Token_Type(TKN_NEW, "new") \
		Token_Type(TKN_UNION, "union") \
		Token_Type(TKN_ENUM, "enum") \
		Token_Type(TKN_ASM, "asm") \
		Token_Type(TKN_LET, "let") \
		Token_Type(TKN_VAR, "var") \
		Token_Type(end_keyword, "") \
		Token_Type(begin_types, "") \
		Token_Type(TKN_INT, "int") \
		Token_Type(TKN_UINT, "uint") \
		Token_Type(TKN_I8, "i8") \
		Token_Type(TKN_I16, "i16") \
		Token_Type(TKN_I32, "i32") \
		Token_Type(TKN_I64, "i64") \
		Token_Type(TKN_U8, "u8") \
		Token_Type(TKN_U16, "u16") \
		Token_Type(TKN_U32, "u32") \
		Token_Type(TKN_U64, "u64") \
		Token_Type(TKN_FLOAT, "float") \
		Token_Type(TKN_FLOAT32, "f32") \
		Token_Type(TKN_FLOAT64, "f64") \
		Token_Type(TKN_UFLOAT, "ufloat") \
		Token_Type(TKN_UFLOAT32, "uf32") \
		Token_Type(TKN_UFLOAT64, "uf64") \
		Token_Type(TKN_STRING, "string") \
		Token_Type(TKN_RSTRING, "rstring") \
		Token_Type(TKN_WSTRING, "wstring") \
		Token_Type(TKN_CHAR, "char") \
		Token_Type(TKN_WCHAR, "wchar") \
		Token_Type(TKN_TRUE, "true") \
		Token_Type(TKN_FALSE, "false") \
		Token_Type(TKN_BOOL, "bool") \
		Token_Type(TKN_NULL, "null") \
		Token_Type(TKN_BYTE, "byte") \
		Token_Type(TKN_VOID, "void") \
		Token_Type(end_types, "")

	enum Token_Type {
	#define Token_Type(_token, ...) _token,
		Token_Types
	#undef Token_Type
		TKN_INVALID,
		TKN_NUM
	};

	bool is_type(Token_Type _token);
	bool is_keyword(Token_Type _keyword);
	bool is_operator(Token_Type _op);
	bool is_literal(Token_Type _lit);
	

	/// @brief Type of token. More of a helper for the 
	///  			 parser.
	enum Type {
		Keyword,
		Operator,
		PrimativeType,
		Constant,
		Identifier,
		Special,
		Directive
	};

	/// @brief Constant Values.
	union Values {
		char* str;
		int dec;
		float floating;
		char ch;
		bool boolean;
	};

	struct Token;
	Token_Type token_enum(const char* _str);
	const std::string& token_string(Token_Type _type);

	struct Token {
			Token() = default;
			
			/// @brief operator and keyword constructor
			/// @param _token the the underlying token
			/// @param _type the type of the token
			/// @param _loc Location of the token
			Token(Token_Type _token, Type _type, const Location& _loc);

			/// @brief string and id constructor
			/// @param _token the the underlying token
			/// @param _type the type of the token
			/// @param _str the string constant or id name
			/// @param _loc Location of the token
			Token(Token_Type _token, Type _type, const Location& _loc, const char* _str);

			/// @brief int constant constructor
			/// @param _token the the underlying token
			/// @param _type the type of the token
			/// @param _d integer constant
			/// @param _loc Location of the token
			Token(Token_Type _token, Type _type, const Location& _loc, int _d);

			/// @brief floating point constant constructor
			/// @param _token the the underlying token
			/// @param _type the type of the token
			/// @param _f floating point constant
			/// @param _loc Location of the token
			Token(Token_Type _token, Type _type, const Location& _loc, float _f);

			/// @brief character constant constructor
			/// @param _token the the underlying token
			/// @param _type the type of the token
			/// @param _c character constant
			/// @param _loc Location of the token
			Token(Token_Type _token, Type _type, const Location& _loc, char _c);

			/// @brief boolean constant constructor
			/// @param _token the the underlying token
			/// @param _type the type of the token
			/// @param _b boolean constant
			/// @param _loc Location of the token
			Token(Token_Type _token, Type _type, const Location& _loc, bool _b);

			Token_Type token() const;
			Type type() const;
			
			const char* get_string() const;
			const Location& location() const;
			int get_int() const;
			float get_float() const;
			char get_char() const;
			bool get_boolean() const;

			void print();

			Token_Type m_tokenType;
			Type m_type;
			Values m_values;
			Location m_location;
	};

	std::string get_spaces(int _indent);
}

#endif