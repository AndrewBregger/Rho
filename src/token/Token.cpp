#include "Token.h"
#include <map>

namespace token {

	const std::map<std::string, Token_Type> tokens = {
		{"+",                                TKN_ADD       } ,
		{"-",                                TKN_SUB       } ,
		{"*",                                TKN_MULT      } ,
		{"/",                                TKN_DIV       } ,
		{"%",                                TKN_REM       } ,
		{"**",                               TKN_POW       } ,
		{"@",                                TKN_AT        } ,
		{"&",                                TKN_AND       } ,
		{"|",                                TKN_OR        } ,
		{"^",                                TKN_XOR       } ,
		{"!",                                TKN_NOT       } ,
		{"<<",                               TKN_SHTL      } ,
		{">>",                               TKN_SHTR      } ,
		{"+=",                               TKN_ADDASN    } ,
		{"-=",                               TKN_SUBASN    } ,
		{"*=",                               TKN_MULTASN   } ,
		{"/=",                               TKN_DIVASN    } ,
		{"%=",                               TKN_REMASN    } ,
		{"&=",                               TKN_ANDASN    } ,
		{"|=",                               TKN_ORASN     } ,
		{"^=",                               TKN_XORASN    } ,
		{"<<=",                              TKN_SHTLASN   } ,
		{">>=",                              TKN_SHTRASN   } ,
		{"&&",                               TKN_LITAND    } ,
		{"||",                               TKN_LITOR     } ,
		{"->",                               TKN_RARR      } ,
		{"<-",                               TKN_LARR      } ,
		{"++",                               TKN_INC       } ,
		{"--",                               TKN_DEC       } ,
		{"=",                                TKN_ASN       } ,
		{":=",                               TKN_DECL      } ,
		{"::",                               TKN_CDECL      } ,
		{"<",                                TKN_LESS      } ,
		{">",                                TKN_GRET      } ,
		{"<=",                               TKN_LEQ       } ,
		{">=",                               TKN_GEQ       } ,
		{"==",                               TKN_EQ        } ,
		{"!=",                               TKN_NEQ       } ,
		{"...",                              TKN_ELS       } ,
		{"{",                                TKN_LBRACK    } ,
		{"}",                                TKN_RBRACK    } ,
		{"[",                                TKN_LBRACE    } ,
		{"]",                                TKN_RBRACE    } ,
		{"(",                                TKN_LPAREN    } ,
		{")",                                TKN_RPAREN    } ,
		{".",                                TKN_PER       } ,
		{";",                                TKN_SEM       } ,
		{",",                                TKN_COM       } ,
		{":",                                TKN_COL       } ,
		{"#",                                TKN_DIRECTIVE     } ,
		{"$",                                TKN_DOLLAR    } ,
		{"?",                                TKN_QEST      } ,
		{"if",                               TKN_IF        } ,
		{"for",                              TKN_FOR       } ,
		{"else",                             TKN_ELSE      } ,
		{"else if",                          TKN_ELIF      } ,
		{"loop",                             TKN_LOOP      } ,
		{"class",                            TKN_CLASS     } ,
		{"struct",                           TKN_STRUCT    } ,
		{"import",                           TKN_IMPORT    } ,
		{"module",                           TKN_MODULE    } ,
		{"using",                            TKN_USING     } ,
		{"namespace",                        TKN_NAMESPACE } ,
		{"return",                           TKN_RETURN    } ,
		{"switch",                           TKN_SWTICH    } ,
		{"defer",                            TKN_DEFER     } ,
		{"workspace",                        TKN_WORKSPACE } ,
		{"new",                              TKN_NEW       } ,
		{"union",                            TKN_UNION     } ,
		{"enum",                             TKN_ENUM      } ,
		{"mut",                              TKN_MUT       } ,
		{"int",                              TKN_I32       } ,
		{"i8",                               TKN_I8      } ,
		{"i16",                              TKN_I16     } ,
		{"int",                              TKN_I32     } ,
		{"i64",                              TKN_I64     } ,
		{"uint",                             TKN_U32      } ,
		{"u8",                               TKN_U8     } ,
		{"u16",                              TKN_U16    } ,
		{"u32",                              TKN_U32    } ,
		{"u64",		                           TKN_U64    } ,
		{"float",                            TKN_FLOAT     } ,
		{"f32",                              TKN_FLOAT32   } ,
		{"f64",                              TKN_FLOAT64   } ,
		{"ufloat",                           TKN_UFLOAT    } ,
		{"uf32",                             TKN_UFLOAT32  } ,
		{"uf64",                             TKN_UFLOAT64  } ,
		{"string",                           TKN_STRING    } ,
		{"rstring",                          TKN_RSTRING   } ,
		{"wstring",                          TKN_WSTRING   } ,
		{"char",                             TKN_CHAR      } ,
		{"wchar",                            TKN_WCHAR     } ,
		{"true",                             TKN_TRUE      } ,
		{"false",                            TKN_FALSE     } ,
		{"bool",                             TKN_BOOL      } ,
		{"nil",                              TKN_NIL       } ,
		{"byte",                             TKN_BYTE      } ,
		{"asm",                              TKN_ASM       }
	};

	const std::map<Token_Type, const char*> token_str{
	  {TKN_LDOUBLE     ,      "TKN_LDOUBLE"},
	  {TKN_LINT        ,      "TKN_Li32"},
	  {TKN_IDENTIFIER  ,      "TKN_IDENTIFIER"},
	  {TKN_LCHAR       ,      "TKN_LCHAR"},
	  {TKN_LSTRING     ,      "TKN_LSTRING"},
	  {TKN_LBOOL       ,      "TKN_LBOOL"},
	  {TKN_ADD         ,      "TKN_ADD"},
	  {TKN_SUB         ,      "TKN_SUB"},
	  {TKN_MULT        ,      "TKN_MULT"},
	  {TKN_DIV         ,      "TKN_DIV"},
	  {TKN_REM         ,      "TKN_REM"},
	  {TKN_POW         ,      "TKN_POW"},
	  {TKN_AT          ,      "TKN_AT"},
	  {TKN_AND         ,      "TKN_AND"},
	  {TKN_OR          ,      "TKN_OR"},
	  {TKN_XOR         ,      "TKN_XOR"},
	  {TKN_NOT         ,      "TKN_NOT"},
	  {TKN_SHTL        ,      "TKN_SHTL"},
	  {TKN_SHTR        ,      "TKN_SHTR"},
	  {TKN_ADDASN      ,      "TKN_ADDASN"},
	  {TKN_SUBASN      ,      "TKN_SUBASN"},
	  {TKN_MULTASN     ,      "TKN_MULTASN"},
	  {TKN_DIVASN      ,      "TKN_DIVASN"},
	  {TKN_REMASN      ,      "TKN_REMASN"},
	  {TKN_ANDASN      ,      "TKN_ANDASN"},
	  {TKN_ORASN       ,      "TKN_ORASN"},
	  {TKN_XORASN      ,      "TKN_XORASN"},
	  {TKN_SHTLASN     ,      "TKN_SHTLASN"},
	  {TKN_SHTRASN     ,      "TKN_SHTRASN"},
	  {TKN_LITAND      ,      "TKN_LITAND"},
	  {TKN_LITOR       ,      "TKN_LITOR"},
	  {TKN_RARR        ,      "TKN_RARR"},
	  {TKN_LARR        ,      "TKN_LARR"},
	  {TKN_INC         ,      "TKN_INC"},
	  {TKN_DEC         ,      "TKN_DEC"},
	  {TKN_ASN         ,      "TKN_ASN"},
	  {TKN_DECL        ,      "TKN_DECL"},
	  {TKN_LESS        ,      "TKN_LESS"},
	  {TKN_GRET        ,      "TKN_GRET"},
	  {TKN_LEQ         ,      "TKN_LEQ"},
	  {TKN_GEQ         ,      "TKN_GEQ"},
	  {TKN_EQ          ,      "TKN_EQ"},
	  {TKN_NEQ         ,      "TKN_NEQ"},
	  {TKN_ELS         ,      "TKN_ELS"},
	  {TKN_LBRACK      ,      "TKN_LBRACK"},
	  {TKN_RBRACK      ,      "TKN_RBRACK"},
	  {TKN_LBRACE      ,      "TKN_LBRACE"},
	  {TKN_RBRACE      ,      "TKN_RBRACE"},
	  {TKN_LPAREN      ,      "TKN_LPAREN"},
	  {TKN_RPAREN      ,      "TKN_RPAREN"},
	  {TKN_PER         ,      "TKN_PER"},
	  {TKN_SEM         ,      "TKN_SEM"},
	  {TKN_COM         ,      "TKN_COM"},
	  {TKN_COL         ,      "TKN_COL"},
	  {TKN_CDECL        ,      "TKN_CDECL"},
	  {TKN_DIRECTIVE   ,      "TKN_DIRECTIVE"},
	  {TKN_DOLLAR      ,      "TKN_DOLLAR"},
	  {TKN_QEST        ,      "TKN_QEST"},
	  {TKN_IF          ,      "TKN_IF"},
	  {TKN_FOR         ,      "TKN_FOR"},
	  {TKN_ELSE        ,      "TKN_ELSE"},
	  {TKN_ELIF        ,      "TKN_ELIF"},
	  {TKN_LOOP        ,      "TKN_LOOP"},
	  {TKN_CLASS       ,      "TKN_CLASS"},
	  {TKN_STRUCT      ,      "TKN_STRUCT"},
	  {TKN_IMPORT      ,      "TKN_IMPORT"},
	  {TKN_MODULE      ,      "TKN_MODULE"},
	  {TKN_USING       ,      "TKN_USING"},
	  {TKN_NAMESPACE   ,      "TKN_NAMESPACE"},
	  {TKN_RETURN      ,      "TKN_RETURN"},
	  {TKN_SWTICH      ,      "TKN_SWTICH"},
	  {TKN_DEFER       ,      "TKN_DEFER"},
	  {TKN_WORKSPACE   ,      "TKN_WORKSPACE"},
	  {TKN_NEW         ,      "TKN_NEW"},
	  {TKN_UNION       ,      "TKN_UNION"},
	  {TKN_ENUM        ,      "TKN_ENUM"},
	  {TKN_INT         ,      "TKN_INT"},
	  {TKN_I8        ,      "TKN_I8"},
	  {TKN_I16       ,      "TKN_I16"},
	  {TKN_I32       ,      "TKN_I32"},
	  {TKN_I64       ,      "TKN_I64"},
	  {TKN_UINT        ,      "TKN_UINT"},
	  {TKN_U8       ,      "TKN_U8"},
	  {TKN_U16      ,      "TKN_U16"},
	  {TKN_U32      ,      "TKN_U32"},
	  {TKN_U64      ,      "TKN_U64"},
	  {TKN_FLOAT       ,      "TKN_FLOAT"},
	  {TKN_FLOAT32     ,      "TKN_FLOAT32"},
	  {TKN_FLOAT64     ,      "TKN_FLOAT64"},
	  {TKN_UFLOAT      ,      "TKN_UFLOAT"},
	  {TKN_UFLOAT32    ,      "TKN_UFLOAT32"},
	  {TKN_UFLOAT64    ,      "TKN_UFLOAT64"},
	  {TKN_STRING      ,      "TKN_STRING"},
	  {TKN_RSTRING     ,      "TKN_RSTRING"},
	  {TKN_WSTRING     ,      "TKN_WSTRING"},
	  {TKN_CHAR        ,      "TKN_CHAR"},
	  {TKN_WCHAR       ,      "TKN_WCHAR"},
	  {TKN_TRUE        ,      "TKN_TRUE"},
	  {TKN_FALSE       ,      "TKN_FALSE"},
	  {TKN_BOOL        ,      "TKN_BOOL"},
	  {TKN_NIL         ,      "TKN_NIL"},
	  {TKN_BYTE        ,      "TKN_BYTE"},
	  {TKN_ASM         ,      "TKN_ASM"}
	};

	bool is_type(Token_Type _token) {
		return begin_types < _token && _token < end_types;
	}
	
	bool is_keyword(Token_Type _keyword) {
		return begin_keyword < _keyword && _keyword < end_keyword;
	}
	
	bool is_operator(Token_Type _op) {
		return begin_operator < _op && _op < end_operator;
	}

		
		Token::
		Token(Token_Type _token, Type _type, const Location& _loc) {
			m_tokenType = _token;
			m_type = _type;
			m_values.dec = 0;
			m_location = _loc;
		}
		
		Token::
		Token(Token_Type _token, Type _type, const Location& _loc, const str::string& _str) {
			m_tokenType = _token;
			m_type = _type;
			m_values.str = _str;
			m_location = _loc;
		}
		
		Token::
		Token(Token_Type _token, Type _type, const Location& _loc, int _d) {
			m_tokenType = _token;
			m_type = _type;
			m_values.dec = _d;
			m_location = _loc;
		}
		
		Token::
		Token(Token_Type _token, Type _type, const Location& _loc, float _f) {
			m_tokenType = _token;
			m_type = _type;
			m_values.floating = _f;
			m_location = _loc;
		}
		
		Token::
		Token(Token_Type _token, Type _type, const Location& _loc, char _c) {
			m_tokenType = _token;
			m_type = _type;
			m_values.ch = _c;
			m_location = _loc;
		}
		
		Token::
		Token(Token_Type _token, Type _type, const Location& _loc, bool _b) {
			m_tokenType = _token;
			m_type = _type;
			m_values.boolean = _b;
			m_location = _loc;
		}

		Token_Type
		Token::
		token() {
			return m_tokenType;
		}
		
		Type
		Token::
		type() {
			return m_type;
		}

		const Location&
		Token::
		location() {
			return m_location;
		}
	
		const str::string&
		Token::
		get_string() {
			return m_values.str;
		}
		
		int
		Token::
		get_int() {
			return m_values.dec;
		}
		
		float
		Token::
		get_float() {
			return m_values.floating;
		}
		
		char
		Token::
		get_char() {
			return m_values.ch;
		}
		
		bool
		Token::
		get_boolean() {
			return m_values.boolean;
		}


		Token_Type
		Token::
		get_enum(const std::string& _str) {
			auto iter = tokens.find(_str);
			if(iter == tokens.end())
				return TKN_ERROR; // not really an error.
			return iter->second;
		}
		const char*
		Token::
		get_str(Token_Type _token) {
			auto iter = token_str.find(_token);
			if(iter == token_str.end())
				return nullptr; // not really an error.
			return iter->second;

		}
}