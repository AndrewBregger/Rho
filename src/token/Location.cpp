#include "Location.h"

namespace token {
	
	Location::
	Location() {
		m_line = m_column = m_eline = m_ecolumn = 0;	
	}

	
	Location::
	Location(size_t _line, size_t _column) {
		m_line = _line;
		m_column = _column;
		m_eline = m_ecolumn = 0;
	}

	
	Location
	diff(const Location& _l, const Location& _r) {
		Location loc;
		loc.m_line = _l.m_line;
		loc.m_column = _l.m_column;
		loc.m_eline = _r.m_line;
		loc.m_ecolumn = _r.m_column;
		return loc;
	}
}
