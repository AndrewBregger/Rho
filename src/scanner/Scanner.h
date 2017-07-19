#ifndef SCANNER_H_
#define SCANNER_H_

#include <cstdarg>
#include <vector>

#include "token/Token.h"
#include "token/Location.h"
#include "utils/File.h"
#include "utils/string.h"

using token::Token;
using token::Location;

namespace scanner {

	class Scanner {
		public:
			Scanner(sys::File* _file, bool _cache = false);
			Token scan();
			Token peak();

			const std::vector<Token*>& get_comments();

		private:
			char m_ch{-1};
			Location m_loc{1,1};
			Location curr;
			size_t m_index{0};
			size_t m_end{0};
			bool m_caching{false};
			Token m_next;
			std::vector<Token*> m_comments;
			sys::File* m_file{nullptr};
			std::string m_source;

		private:
			char peak(size_t _index);
			void next();

			Token scan_string();
			double scan_double(size_t _start);
			int scan_hex(size_t _start);
			int scan_binary(size_t _start);
			double scan_scientific(size_t _start);
			Token scan_number();

			void scan_comment();
			void scan_whitespace();
			Token scan_char();
			std::string scan_id();
			void cache(Token _token);

			void report_error(const Location& _loc, const char* _fmt, ...);
	};

} // scanner

#endif // SCANNER_H_
