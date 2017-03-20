#include "AtomTable.h"
namespace ast {

	Atom*
	AtomTable::
	insert(const std::string& _str) {
		Atom* atom = find(_str);
		if(atom) {
			atom->increment_ref();
			return atom;
		}
		else {
			atom = new Atom(_str);
			atom->increment_ref();
			m_table.emplace(_str, atom);
			return atom;
		}
	}

	Atom*
	AtomTable::
	find(const std::string& _str) {
		auto iter = m_table.find(_str);
		if(iter == m_table.end())
			return nullptr;
		else
			return iter->second;	
	}

	size_t
	AtomTable::
	size() {
		return m_table.size();
	}

} // ast
