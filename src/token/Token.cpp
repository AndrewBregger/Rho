#include "Token.h"
#include <map>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <sstream>

#ifdef WIN32
	#define strdup_ strdup_s
#else
	#define strdup_ strdup
#endif

namespace token {

	const std::map<std::string, Token_Type> tokens = {
		#define Token_Type(token, str) {str, token},
		Token_Types
		#undef Token_Type
	};

	const std::map<Token_Type, std::string> token_str{
	 	#define Token_Type(token, str) {token, str},
	 	Token_Types
		#undef Token_Type
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

	bool is_literal(Token_Type _lit) {
		return begin_literal < _lit && _lit < end_literal;
	}

		
		Token::
		Token(Token_Type _token, Type _type, const Location& _loc) {
			m_tokenType = _token;
			m_type = _type;
			m_values.dec = 0;
			m_location = _loc;
		}
		
		Token::
		Token(Token_Type _token, Type _type, const Location& _loc, const char* _str) {
			m_tokenType = _token;
			m_type = _type;
			m_values.str = strdup_(_str);
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
		token() const{
			return m_tokenType;
		}
		
		Type
		Token::
		type() const{
			return m_type;
		}

		const Location&
		Token::
		location() const{
			return m_location;
		}
	
		const char*
		Token::
		get_string() const{
			return m_values.str;
		}
		
		int
		Token::
		get_int() const{
			return m_values.dec;
		}
		
		float
		Token::
		get_float() const{
			return m_values.floating;
		}
		
		char
		Token::
		get_char() const{
			return m_values.ch;
		}
		
		bool
		Token::
		get_boolean() const{
			return m_values.boolean;
		}


		Token_Type
		token_enum(const char* _str) {
			auto iter = tokens.find(std::string(_str));
			if(iter == tokens.end())
				return TKN_ERROR; // not really an error.
			return iter->second;
		}

		const std::string&
		Token::
		string() {
			auto iter = token_str.find(m_tokenType);
			if(iter == token_str.end()) {
				std::cout << "Token not found: " << m_tokenType << std::endl;
				return iter->second;
			}
			return iter->second;
		}

		const std::string&
		Token::
		string() const {
			auto iter = token_str.find(m_tokenType);
			if(iter == token_str.end()) {
				std::cout << "Token not found: " << m_tokenType << std::endl;
				return iter->second;
			}
			return iter->second;
		}

		void
		Token::
		print(int indent) {
			std::cout << get_spaces(indent) << this->string() << " ";
			printf("%lu : %lu - %lu: ", m_location.m_line, m_location.m_column, m_location.m_ecolumn);
			switch(m_type) {
				case Keyword:
				case Operator:
				case PrimativeType:
					break;
				case Identifier:
					std::cout << get_string();
					break;
				case Constant: {
					switch(m_tokenType) {
						case TKN_LSTRING:
							std::cout << get_string();
							break;
						case TKN_LCHAR:
							std::cout << get_char();
							break;
						case TKN_LDOUBLE:
							std::cout << get_float();
							break;
						case TKN_LBOOL:
							std::cout << get_boolean();
							break;
						case TKN_LINT:
							std::cout << get_int();
							break;
						default:
							break;
					}
				} break;
				default:
					break;
			}
			std::cout << std::endl;
		}

	void Token::print(int indent) const {
		const_cast<Token*>(this)->print(indent);
	}

	std::string
		get_spaces(int _index) {
			std::string temp;
			while(_index--) {
				temp += "  ";
			}
			return temp;
		}

	std::string Token::error_string() {
		switch(m_type) {
			case Keyword:
			case Operator:
			case PrimativeType:
				return string();
			case Identifier:
				return std::string(get_string());
			case Constant: {
				std::stringstream ss;
				switch(m_tokenType) {
					case TKN_LSTRING:
						ss << get_string();
						break;
					case TKN_LCHAR:
						ss<< get_char();
						break;
					case TKN_LDOUBLE:
						ss << get_float();
						break;
					case TKN_LBOOL:
						ss << (get_boolean() ? "true" : "false");
						break;
					case TKN_LINT:
						ss << get_int();
						break;
					default:
						// will not happen.
						break;
				}
				return ss.str();
			}
			default:
				break;
		}
		return "";
	}

	std::string Token::error_string() const {
		switch(m_type) {
			case Keyword:
			case Operator:
			case PrimativeType:
				return string();
			case Identifier:
				return std::string(get_string());
			case Constant: {
				std::stringstream ss;
				switch(m_tokenType) {
					case TKN_LSTRING:
						ss << get_string();
						break;
					case TKN_LCHAR:
						ss<< get_char();
						break;
					case TKN_LDOUBLE:
						ss << get_float();
						break;
					case TKN_LBOOL:
						ss << (get_boolean() ? "true" : "false");
						break;
					case TKN_LINT:
						ss << get_int();
						break;
					default:
						// will not happen.
						break;
				}
				return ss.str();
			}
			default:
				break;
		}
		return "";
	}
}