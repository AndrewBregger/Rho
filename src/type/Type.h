#ifndef TYPE_H_
#define TYPE_H_
#include <string>



namespace type {
  enum TypeKind : int {
    TypeBase = -1,
    TypePrimative,
    TypeFunction,
    /*
    TypeClass,
    TypeStruct,
    TypeEnum,
    TypeUnion,
    TypeMethod,

    // Syntax hasn't been determined 
    TypeGenericType,
    TypeGenericFunction,
    TypeGenericStruct,
    TypeGenericClass,
    */
  };

  class Type {
    public:
      Type(size_t _size = 0, TypeKind _kind = TypeBase) {
        m_size = _size;
        m_kind = _kind;
      }

      virtual ~Type() = default;
      size_t size() { return m_size; }
    private:
      size_t m_size;  // size in bytes
      TypeKind m_kind;
  };
}

#endif