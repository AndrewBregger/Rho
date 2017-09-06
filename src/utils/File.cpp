#include "File.h"
#include <string>
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

namespace sys {
	File* File::m_current = nullptr;

	File::
	File(const std::string& _path, FileMode _mode, const Content& _content, FileError _error) {
		m_content = _content;
		m_path = _path;
		m_mode = _mode;
		if(_error != File_InvalidFile)
			m_fullPath = File::FullPath(_path);
		m_error = _error;
		SplitIntoLines();
	}

	File::
	~File() {
		delete[] m_content.m_content;
	}

	const File::Content&
	File::
	content() {
		return m_content;
	}

	File*
	File::
	read_file(const std::string& _path) {
		FILE* file = fopen(_path.c_str(), "r");
		FileError err = File_None;
		File* f;
		if(file) {
			fseek(file, 0, SEEK_END);
			size_t size = ftell(file);
			rewind(file);
			char* buffer = new char[size];
			size = fread(buffer, sizeof(char), size, file);
			size_t index = _path.find_last_of(".");
			std::string ext = _path.substr(index + 1);
			if(size == 0)
				err = File_Empty;
			if(ext != "rho")
				err = File_WrongExtension;
			f = new File(_path, Read, Content{buffer, size}, err);

		}
		else {

			// check errors
			switch(errno) {
				case EACCES:
					err = File_Permissions;
					break;
				case ENOENT:
					err = File_InvalidFile;
					break;
			}
			f = new File(_path, Read, Content{0,0}, err);
		}
		if(file)
			fclose(file);
		return f;
	}

	const std::string &
	File::
	GetLine(size_t _size) {
		assert(_size >= 1);
		return m_lineInfo[_size - 1].line;
	}

	const File::LineInfo& File::GetLineInfo(size_t _line) {
		assert(_line >= 1);
		return m_lineInfo[_line - 1];
	}

	void
	File::
	SplitIntoLines() {
		size_t start = 0;
		for(size_t i = 0; i < m_content.m_size; ++i) {
			if(m_content.m_content[i] == '\n') {
				std::string temp;
				// I do not want the new line.
				size_t ltrim = 0, rtrim = 0;
				for (size_t j = start; j <= i - 1; ++j)
					temp += m_content.m_content[j];

				decltype(temp.begin()) biter;
				decltype(temp.rbegin()) iter;
				for (biter = temp.begin(); biter != temp.end(); ++biter) {
					if (isspace(*biter))
						ltrim++;
					else
						break;
				}
				for (iter = temp.rbegin(); iter != temp.rend(); ++iter) {
					if (isspace(*iter))
						rtrim++;
					else
						break;
				}
				long startIndex = biter - temp.begin();
				long endIndex = iter - temp.rbegin();
				temp = temp.substr((size_t) startIndex, (size_t) endIndex - startIndex);
				std::cout << temp << std::endl;
				m_lineInfo.push_back(LineInfo{temp, rtrim, ltrim});
				start = i + 1;
			}
		}
	}

	void
	File::
	SetFile(File* _file) {
		m_current = _file;
	}

	std::string
	File::
	GetExtension() {
		size_t index = m_path.find_last_of(".");
		return m_path.substr(index + 1);
	}

	std::string
	File::
	FullPath(const std::string& _filename) {
	#ifdef WIN32
		#error Windows not implemented
	#else
		char str[1024];
		if(realpath(_filename.c_str(), str))
			return std::string(str);
		else
			return std::string();
	#endif
	}
}