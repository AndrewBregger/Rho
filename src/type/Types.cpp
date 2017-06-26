#include "Types.h"
#include "token/Token.h"
#include <map>
using namespace token;

namespace type {
	std::map<Token_Type, PrimativeType*> primatives = {
		{ TKN_I8, new PrimativeType("i8", 1) },
		{ TKN_I16, new PrimativeType("i16", 2) },
		{ TKN_I32, new PrimativeType("i32", 4) },
		{ TKN_I64, new PrimativeType("i64", 8) },
		{ TKN_U8, new PrimativeType("u8", 1) },
		{ TKN_U16, new PrimativeType("u16", 2) },
		{ TKN_U32, new PrimativeType("u32", 4) },
		{ TKN_U64, new PrimativeType("u64", 8) },
		{ TKN_FLOAT32, new PrimativeType("float32", 4) },
		{ TKN_FLOAT64, new PrimativeType("float64", 8) },
		{ TKN_UFLOAT32, new PrimativeType("ufloat32", 4) },
		{ TKN_UFLOAT64, new PrimativeType("ufloat64", 8) },
		{ TKN_CHAR, new PrimativeType("char", 1) },
		{ TKN_WCHAR, new PrimativeType("wchar", 2) }
	};

	// primatives.emplace(TKN_INT, primatives[TKN_I32]);
	// primatives.emplace(TKN_UINT, primatives[TKN_U32]);
	// primatives.emplace(TKN_FLOAT, primatives[TKN_FLOAT32]);
	// primatives.emplace(TKN_UFLOAT, primatives[TKN_UFLOAT32]);

	PrimativeType* get_primative(token::Token* _token) {
		auto iter = primatives.find(_token->token());
		if(iter == primatives.end())
			return nullptr;
		else
			return iter->second;
	}

}