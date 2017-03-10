#include "File.h"
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

	void
	File::
	SetFile(File* _file) {
		m_current = _file;
	}
}