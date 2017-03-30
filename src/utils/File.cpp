#include "File.h"
#include <string>
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

namespace sys {
	File* File::m_current = nullptr;
	
	File::
	File(const str::string& _path, FileMode _mode, const Content& _content) {
		m_content = _content;	
		m_path = _path;
		m_mode = _mode;
		SplitIntoLines();
	}

	const File::Content&
	File::
	content() {
		return m_content;
	}

	File*
	File::
	read_file(const str::string& _path) {
		const char* path = to_str(_path);
		std::ifstream f(path);
		if(f) {
			std::string c;
			std::stringstream ss;
			ss << f.rdbuf();
			c = ss.str();	
			return new File(_path, Read, Content{strdup(c.c_str()), c.size()});
		}
		else
			return nullptr;
	}

	str::string
	File::
	GetLine(size_t _size) {
		assert(_size >= 1);
		return m_lineCache[_size - 1];
	}

	void
	File::
	SplitIntoLines() {
		size_t start = 0;
		for(size_t i = 0; i < m_content.m_size; ++i) {
			if(m_content.m_content[i] == '\n') {
				std::string temp;
				for(size_t j = start; j <= i; ++j)
					temp += m_content.m_content[j];

				m_lineCache.push_back(str::new_string(temp.c_str()));
				start = i + 1;
			}
		}	
	}

	void
	File::
	SetFile(File* _file) {
		m_current = _file;
	}
}