#ifndef TOKEN_H_
#define TOKEN_H_
#include "utils/string.h"
#include "Location.h"

namespace token {
	enum Token_Type {
		TKN_COMMENT,
		TKN_WHITESPACE,
		TKN_ERROR,
		TKN_EOF,
		TKN_EMPTY,

		begin_literal,
		TKN_LSTRING,
		TKN_IDENTIFIER,
		TKN_KEYWORD,
		TKN_LCHAR,
		TKN_LINT,
		TKN_LDOUBLE,
		TKN_LBOOL,
		end_literal,

		begin_operator,

		TKN_ADD,
		TKN_SUB,
		TKN_MULT,
		TKN_DIV,
		TKN_REM,
		TKN_POW,

		TKN_AT,
		TKN_AND,
		TKN_OR,
		TKN_XOR,
		TKN_NOT,
		TKN_SHTL,
		TKN_SHTR,

		TKN_ADDASN,
		TKN_SUBASN,
		TKN_MULTASN,
		TKN_DIVASN,
		TKN_REMASN,
		TKN_XORASN,


		TKN_ANDASN,
		TKN_ORASN,

		TKN_SHTLASN,
		TKN_SHTRASN,

		TKN_LITAND,
		TKN_LITOR,

		TKN_RARR,
		TKN_LARR,

		TKN_INC,
		TKN_DEC,

		TKN_ASN, // =
		TKN_DECL, // :=
		TKN_CDECL, // ::

		TKN_LESS,
		TKN_GRET,
		TKN_LEQ,
		TKN_GEQ,

		TKN_EQ,
		TKN_NEQ,
		TKN_ELS,

		TKN_LBRACK,
		TKN_RBRACK,
		TKN_LBRACE,
		TKN_RBRACE,
		TKN_LPAREN,
		TKN_RPAREN,

		TKN_PER,
		TKN_SEM,
		TKN_COM,
		TKN_COL,

		TKN_DIRECTIVE,
		TKN_QEST,
		TKN_DOLLAR,

		end_operator,

		begin_keyword,
		TKN_CONST,
		TKN_MUT,
		TKN_IF,
		TKN_FOR,
		TKN_ELSE,
		TKN_ELIF,
		TKN_LOOP,
		TKN_CLASS,
		TKN_STRUCT,
		TKN_IMPORT,
		TKN_MODULE,
		TKN_USING,
		TKN_NAMESPACE,
		TKN_RETURN,
		TKN_SWTICH,
		TKN_DEFER,
		TKN_WORKSPACE,
		TKN_NEW,
		TKN_UNION,
		TKN_ENUM,
		TKN_ASM,
		end_keyword,

		begin_types,
		TKN_INT,
		TKN_UINT,
		TKN_I8,
		TKN_I16,
		TKN_I32,
		TKN_I64,
		TKN_U8,
		TKN_U16,
		TKN_U32,
		TKN_U64,
		TKN_FLOAT,
		TKN_FLOAT32,
		TKN_FLOAT64,
		TKN_UFLOAT,
		TKN_UFLOAT32,
		TKN_UFLOAT64,
		TKN_STRING,
		TKN_RSTRING,
		TKN_WSTRING,
		TKN_CHAR,
		TKN_WCHAR,
		TKN_TRUE,
		TKN_FALSE,
		TKN_BOOL,
		TKN_NIL,
		TKN_BYTE,
		end_types
	};

	bool is_type(Token_Type _token);
	bool is_keyword(Token_Type _keyword);
	bool is_operator(Token_Type _op);
	

	/// @brief Type of token. More of a helper for the 
	///  			 parser.
	enum Type {
		Keyword,
		Operator,
		PrimativeType,
		Constant,
		Identifier,
		Special
	};

	/// @brief Constant Values.
	union Values {
		str::string str;
		int dec;
		float floating;
		char ch;
		bool boolean;
	};

	class Token {
		public:
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
			Token(Token_Type _token, Type _type, const Location& _loc, const str::string& _str);

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

			Token_Type token();
			Type type();
			
			const str::string& get_string();
			const Location& location();
			int get_int();
			float get_float();
			char get_char();
			bool get_boolean();


			static Token_Type get_enum(const std::string& _str);
			static const char* get_str(Token_Type _token);
		private:
			Token_Type m_tokenType;
			Type m_type;
			Values m_values;
			Location m_location;
	};
}

#endif