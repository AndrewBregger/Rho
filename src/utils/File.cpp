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
		fclose(file);
		return f;
	}

	std::string
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
				m_lineCache.push_back(temp);
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
		realpath(_filename.c_str(), str);
		 return std::string(str);
	#endif
	}
}