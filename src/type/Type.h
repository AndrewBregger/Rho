#ifndef TYPE_H_
#define TYPE_H_
#include <string>



namespace type {
  class Type {
    public:
      Type() = default;
      virtual size_t size() = 0;
    private:
      size_t m_size;
  };

  class PrimativeType : public Type {
    public:
      PrimativeType();
    private:
  };
}

#endif