#ifndef LOCATION_H_
#define LOCATION_H_
#include <cstdlib>

namespace token {
	struct Location {
			/// @brief Defalt constructor
			Location();

			/// @brief Construct with line and column values
			/// @param _line line of the file
			/// @param _column column of the file
			Location(size_t _line, size_t _column);

			/// @brief difference between the two locations
			/// @param _l Start location
			/// @param _r End location
			friend Location diff(const Location& _l, const Location& _r);

			size_t m_line;
			size_t m_column;
			size_t m_eline;
			size_t m_ecolumn;
	};
}

#endif // LOCATION_H_
