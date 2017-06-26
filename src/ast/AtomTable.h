#ifndef ATOM_TABLE_H_
#define ATOM_TABLE_H_

#include "Atom.h"
#include "utils/string.h"
#include <functional>
#include <unordered_map>

using ast::Atom;

namespace ast {

	class AtomTable {
		public:
			AtomTable() = default;
			
			Atom* insert(const std::string& _str);
			Atom* find(const std::string& _str);
			size_t size();	

		private:
			std::unordered_map<std::string, Atom*> m_table;
	};

} // ast

#endif // ATOM_TABLE_H_
