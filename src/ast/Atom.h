#ifndef ATOM_H_
#define ATOM_H_

#include "utils/string.h"

namespace ast {

	class Atom {
		public:
			Atom(const std::string& _str) {
				m_string = _str;
				m_references = 0;
			}
			~Atom() {}
			const std::string& name() { return m_string; }
			size_t ref_count() { return m_references; }

		private:

			void increment_ref() { ++m_references; }

			std::string m_string;
			size_t m_references;

			friend class AtomTable;
	};
} // ast


#endif // ATOM_H_
