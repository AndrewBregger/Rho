#include "Type.h"

#include <string>
#include <vector>

namespace token {
	class Token;
}
namespace ast {
  namespace decl {
    class AstClass;
    class AstMethod;
  }
  class AstIdent;
}
namespace type {
  class PrimativeType : public type::Type {
    public:
      PrimativeType(const std::string& _name, size_t _bytes): Type(_bytes, type::TypePrimative) {
      	m_name = _name;
      }
    private:
     	std::string m_name;
  };

  class FunctionType : public type::Type {
    public:
      FunctionType(const std::vector<Type*>& _params, const std::vector<Type*>& _ret): Type(0, type::TypeFunction) {
        m_params = _params;
        m_rets = _ret;
      }

      const std::vector<Type*>& parameters() { return m_params; }
      const std::vector<Type*>& returns() { return m_rets; }

    private:
      std::vector<Type*> m_params;
      std::vector<Type*> m_rets;
  };


  class ClassType : public type::Type {
    public:
      ClassType(ast::AstIdent* _name);
    private:
  };

  class StructType {

  };

  PrimativeType* get_primative(token::Token* _token);
}