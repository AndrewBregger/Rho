#include "Scanner.h"

#include <cstdio>
#include <cctype>
#include <sstream>
#include <utility>

using namespace token;

bool is_letter(char _ch) {
  return isalpha(_ch) || _ch == '_';
}

bool is_number(char _ch) {
  return isdigit(_ch);
}

namespace scanner {
	
	Scanner::
	Scanner(sys::File* _file, bool _cache) {
		m_end = _file->content().m_size;
		m_caching = _cache;
		m_file = _file;
		m_source = str::new_string(m_file->content().m_content);
		m_ch = m_source[m_index];
	}
	
	Token*
	Scanner::
	scan() {
		if(m_next) {
			auto temp = m_next;
			m_next = nullptr;
			return temp;
		}
		// ignore whitespace
		scan_whitespace();

		curr = m_loc;
		char ch = m_ch;
		Token_Type token = token::TKN_ERROR;
		if(ch == -1)
			return new Token(token::TKN_EOF, Special, diff(curr, m_loc));
		if(is_letter(ch)) {
			str::string lit = scan_id();
			token = Token::get_enum(str::to_string(lit));
			if(token == token::TKN_ERROR)
				return new Token(TKN_IDENTIFIER, Identifier, diff(curr, m_loc), lit);
			else {
				if(is_type(token))
					return new Token(token, PrimativeType, diff(curr, m_loc));
				else {
					if(token == TKN_ELSE) {
						Token* n = peak();
						if(n->token() == TKN_IF) {
							m_next = nullptr;
							return new Token(TKN_ELIF, Keyword, diff(curr, n->location()));
						}
					}
					return new Token(token, Keyword, diff(curr, m_loc));
				}
			}
		}
		else if(is_number(ch)) {
			return scan_number();
		}
		else {
			next();
			switch(ch) {
				case '\'': {
					return scan_char();
				} break;

				case '"': {
					return scan_string();
				} break;

				case '/': {
					if(m_ch == '*' || m_ch == '/') {
            scan_comment();
						return scan(); // return the next token
					}
					else {
						if(m_ch == '=') {
							token = TKN_DIVASN;
						}
						else {
							token = TKN_DIV;
						}
					}
				} break;
				case '.': {
					if(m_ch == '.') {
						if(peak(1) == '.') {
							next();
							token = TKN_ELS;
						}
						else {
              // @todo(Andrew): error handling
							token 	= TKN_ERROR;
						}
					}
					else {
						token = TKN_PER;
					}
					} break;
					case ',': {
	          token = TKN_COM;
					} break;
					case ';': {
	          token = TKN_SEM;
					} break;
					case ':': {
						if(m_ch == ':') {
							token = TKN_CDECL;
							next();
						}
						else if(m_ch == '=') {
							token = TKN_DECL;
							next();
						}
						else {
							token = TKN_COL;
						}
					} break;
					case ']': {
	          token = TKN_RBRACE;
					} break;
					case '[': {
	          token = TKN_LBRACE;
					} break;
					case '}': {
	          token = TKN_RBRACK;
					} break;
					case '{': {
	          token = TKN_LBRACK;
					} break;
					case '|': {
						if(m_ch == '=') {
							token = TKN_ORASN;
							next();
						}
						else if(m_ch == '|') {
							token = TKN_LITOR;
							next();
						}
						else {
							token = TKN_OR;
						}
					} break;
					case '(': {
	          token = TKN_LPAREN;
					} break;
					case ')': {
	          token = TKN_RPAREN;
					} break;
					case '+': {
						if(m_ch == '+') {
							token = TKN_INC;
							next();
						}
						else if(m_ch == '=') {
							token = TKN_ADDASN;
							next();
						}
						else {
							token = TKN_ADD;
						}
					} break;
					case '-': 	{
						if(m_ch == '-') {
							token = TKN_DEC;
							next();
						}
						else if(m_ch == '=') {
							token = TKN_SUBASN;
							next();
						}
						else if(m_ch == '>') {
							token = TKN_RARR;
							next();
						}
						else {
							token = TKN_SUB;
						}
					} break;
					case '=': {
						if(m_ch == '=') {
							token = TKN_EQ;
							next();
						}
						else {
							token = TKN_ASN;
						}
					} break;
					case '!': {
						if(m_ch == '=') {
							token = TKN_NEQ;
							next();
						}
						else {
							token = TKN_NOT;
						}
					} break;
					case '<': {
						if(m_ch == '<') {
							if(peak(1) == '=') {
								next();
								token = TKN_SHTLASN;
							}
							else {
								token = TKN_SHTL;
							}
							next();
						}
						else if(m_ch == '=') {
							token = TKN_LEQ;
							next();
						}
						else if(m_ch == '-') {
							token = TKN_LARR;
							next();
						}
						else {
							token = TKN_LESS;
						}
					} break;
					case '>': {
						if(m_ch == '>') {
							if(peak(1) == '=') {
								next();
								token = TKN_SHTRASN;
							}
							else {
								token = TKN_SHTR;
							}
							next();
						}
						if(m_ch == '=') {
							token = TKN_GEQ;
							next();
						}
						else {
							token = TKN_GRET;
						}
					} break;
					case '@': {
	          token = TKN_AT;
					} break;
					case '#': {
						str::string lit = scan_id();
						if(str::str_eq(lit, str::new_string("load"))) {
							return new token::Token(TKN_LOAD, Directive, diff(curr, m_loc), lit);
						}
						return new token::Token(TKN_ERROR, Special, diff(curr, m_loc));
					} break;
					case '$': {
	          token = TKN_DOLLAR;
					} break;
					case '%': {
						if(m_ch == '=') {
							token = TKN_REMASN;
							next();
						}
						else {
							token = TKN_REM;
						}
					} break;
					case '^': {
						if(m_ch == '=') {
							token = TKN_XORASN;
							next();
						}
						else {
							token = TKN_XOR;
						}
					} break;
					case '&': {
						if(m_ch == '=') {
							token = TKN_ANDASN;
							next();
						}
						else if(m_ch == '&') {
							token = TKN_LITAND;
							next();
						}
						else {
							token = TKN_AND;
						}
					} break;
					case '*': {
						if(m_ch == '*') {
							token = TKN_POW;
							next();
						}
						else if(m_ch == '=') {
							token = TKN_MULTASN;
							next();
						}
						else {
							token = TKN_MULT;
						}
					} break;
					case '?': {
	          token = TKN_QEST;
					} break;
					default:
						report_error(curr, "Invalid character '%c'", ch);
			}
		}
		if(is_operator(token))
			return new Token(token, Operator, diff(curr, m_loc));
		else {
			printf("Token: %s\n", Token::get_str(token));
			return new Token(TKN_ERROR, Special, diff(curr, m_loc));
		}
	}

	Token*
	Scanner::
	peak() {
		if(m_next){
			return m_next;
		}
		else {
			m_next = scan();
			return m_next;
		}
	}

	
	const std::vector<Token*>&
	Scanner::
	get_comments() {
		return m_comments;
	}

	char
	Scanner::
	peak(size_t _index) {
		return m_source[m_index + _index];
	}

	void
	Scanner::
	next() {
		if(m_index < m_end - 1) {
      if(m_source[m_index] == '\n') {
        m_loc.m_column = 0;
        ++m_loc.m_line;
      }
      m_ch = m_source[++m_index];
      ++m_loc.m_column;
    }
    else
      m_ch = -1;
	}

	Token*
	Scanner::
	scan_string() {
		Token_Type token = TKN_LSTRING;
		size_t start = m_index - 1;
		while(m_ch != '"') {
      if(m_ch == -1) {
      	report_error(m_loc, "String not ended before end of file");
      	token = TKN_ERROR;
        break;
      }
      next();
    }
    next();
    str::string lit = str::substr(m_source, start, m_index);
    return new Token(token, Constant, diff(curr, m_loc), lit);
	}

	double
	Scanner::
	scan_double(size_t _start) {
	 	next();
    while(is_number(m_ch)) {
      next();
    }
    auto str = str::substr(m_source, _start, m_index);
    double d = strtod(str::to_str(str), NULL);
    str::dest_string(&str);
    return d;
	}

	int
	Scanner::
	scan_hex(size_t _start) {
   	next();
    while(isxdigit(m_ch)) {
      next();
    }
    auto str = str::substr(m_source, _start, m_index);
    std::stringstream ss;
    ss << std::hex << str::to_string(str);
    int t;
    ss >> t;
    str::dest_string(&str);
    return t;
	}

	int
	Scanner::
	scan_binary(size_t _start) {
		return _start;
	}

	double
	Scanner::
	scan_scientific(size_t _start) {
		next();
    if(peak(1) == '-' || peak(1) == '+') {
      next();
    }
    while(is_number(m_ch)) {
        next();
    }
    auto str = str::substr(m_source, _start, m_index);
    std::stringstream ss;
    ss << std::scientific << str::to_string(str);
    double t;
    ss >> t;
    str::dest_string(&str);
    return t;
	}

	Token*
	Scanner::
	scan_number() {
		size_t start = m_index;
	 	auto t = [](char _c) {
      return (is_number(_c) || _c == '.'
        || _c == 'E' || _c == 'e' || _c == 'x' || _c == 'X' ||
        _c == 'b' || _c == 'B');
    };
    float f;
    int d;
    Token_Type token = TKN_LINT;
    while(t(m_ch)) {
      if(m_ch == '.') {
        f = scan_double(start);
        token = TKN_LDOUBLE;
		  	return new Token(token, Constant, diff(curr, m_loc), f);
      }
      else if(m_ch == 'e'|| m_ch == 'E') {
        f = scan_scientific(start);
        token = TKN_LDOUBLE;
		  	return new Token(token, Constant, diff(curr, m_loc), f);
      }
      else if(m_ch == 'x' || m_ch == 'X') {
        d = scan_hex(start);
	    	return new Token(token, Constant, diff(curr, m_loc), d);
      }
      else if(m_ch == 'b' || m_ch == 'B') {
        d = scan_binary(start);
	    	return new Token(token, Constant, diff(curr, m_loc), d);
      }
      next();
    }
    auto lit = str::substr(m_source, start, m_index);

    d = atoi(to_str(lit));
  	return new Token(token, Constant, diff(curr, m_loc), d);
  }	

	void
	Scanner::
	scan_comment() {
		if(m_ch == '/') {
			while(m_ch != '\n') next();
		}
		else if(m_ch == '*') {
			next();
			while(m_ch != -1) {
				if(m_ch == '*') {
					next();
					if(m_ch == '/')
						break;
				}
				next();
			}
			next();
		}
	}

	void
	Scanner::
	scan_whitespace() {
		while(isspace(m_ch)) next();
	}

	Token*
	Scanner::
	scan_char() {
		size_t start = m_index - 1;
		next();
		while(m_ch == '\'') next();

		if(m_index - start != 3) {
			report_error(curr, "multi-character characters are not allowed");
			return new Token(TKN_ERROR, Constant, diff(curr, m_loc)) ;
		}
		str::string lit = str::substr(m_source, start, m_index);
		return new Token(TKN_LCHAR, Constant, diff(curr, m_loc), lit) ;
	}

	str::string
	Scanner::
	scan_id() {
		size_t start = m_index;
		auto t = [](char _c) {return is_letter(_c) || is_number(_c);};
		while(t(m_ch)) next();
		return str::substr(m_source, start, m_index);
	}

	void
	Scanner::
	cache(Token* _token) {
		m_comments.push_back(_token);
	}

	void
	Scanner::
	report_error(const Location& _loc, const char* _fmt, ...) {
		printf("Error: %s|%zu:%zu: ", m_file->GetPath(), _loc.m_line, _loc.m_column);
		va_list l;
		printf(_fmt, l);
	}
}
