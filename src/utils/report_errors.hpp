//
// Created by Andrew Bregger on 8/26/17.
//

#ifndef RHO_REPORT_ERRORS_HPP
#define RHO_REPORT_ERRORS_HPP

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include "token/Token.h"
#include "File.h"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"

struct ErrorStats {
	std::size_t errors{0};
	std::size_t warnings{0};
};

extern ErrorStats error_data;

inline void print_header(const std::string& header, const token::Token& token, sys::File* file) {
	using std::cout;
	if(file)
		cout << file->GetPath() << "|" << token.m_location.m_line << ":" << token.m_location.m_column <<
	     " " << header << ": ";
	else
		cout << "\t" << header << ": ";
}

inline void underline_token(const token::Token& token, sys::File* file) {
	std::string buffer = "\t";
	size_t size = token.m_location.m_column;
	const auto& info = file->GetLineInfo(token.m_location.m_line);
	size -= info.lTrim;

	while((bool) --size)
		buffer.push_back(' ');
	buffer.push_back('^');
	size = token.m_location.m_ecolumn - token.m_location.m_column;
	while((bool) --size)
		buffer.push_back('-');
	std::cout << buffer << std::endl;
}

inline void print_formated_code(const token::Token& token, sys::File* file) {
	std::cout << std::endl; // seperate the line of code from the error message
	std::size_t l = token.m_location.m_line;
	const auto& info = file->GetLineInfo(l);
	std::cout << "\t" << info.line << std::endl;
}


template <class... T>
void syntax_error(const token::Token& token, sys::File* file, std::string msg, T... args) {
	print_header("Error", token, file);
	printf(msg.c_str(), args...);
	putchar('\n');
	print_formated_code(token, file);
	underline_token(token, file);
}

template <class... T>
void syntax_note(const token::Token& token, std::string msg, T... args) {
	print_header("Note", token, nullptr);
	printf(msg.c_str(), args...);
}

template <class... T>
void file_error(std::string msg, T... args) {
	printf("File Error: ");
	printf(msg.c_str(), args...);
}
#pragma GCC diagnostic pop

#endif //RHO_REPORT_ERRORS_HPP
