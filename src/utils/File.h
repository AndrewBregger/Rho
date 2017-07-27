#ifndef FILE_H_
#define FILE_H_

#include "utils/string.h"
#include <vector>

namespace sys {
	/// @brief IO operatio of the file
	/// @TODO: This will be expanded later.
	enum FileMode {
		Read,
		Write,
		ReadWrite
	};

	enum FileError {
		File_None,
		File_Empty,
		File_Permissions,
		File_WrongExtension,
		File_InvalidFile
	};

	class File {
		public:
			/// @brief content of a File
			struct Content {
				char* m_content;
				size_t m_size;
			};

			/// @brief File constuctor
			/// @param _path path to file
			/// @param _mode IO operator of the file
			/// @param _content Content of the file.
			File(const std::string& _path, FileMode _mode, const Content& _content, FileError _error);

			/// @brief File Destcuctor
			~File();


			/// @brief Gets the current content of the file.
			const Content& content();
			char* buffer() {
				return m_content.m_content;
			}

			static void SetFile(File* _file);

			const std::string& GetPath() {
				return m_path;
			}

			const std::string& GetFullPath() {
				return m_fullPath;
			}

			/// @brief Reads an entire file
			/// @param _path path of file being read.
			static File* read_file(const std::string& _path);

			/// @brief Gets a line from the file.
			/// @param _line Line of the file being returned. Starts at 1.
			std::string GetLine(size_t _line);


			FileError GetError() {
				return m_error;
			}

			std::string GetExtension();

			static std::string FullPath(const std::string& _filename);
		private:
			/// @brief Divides the file into lines.
			void SplitIntoLines();


			Content m_content; ///< Current content of the file and size
			std::string m_path; ///< Path to the file on disk.
			std::string m_fullPath; ///< absolute path.
			FileMode m_mode; ///< IO mode of the file.
			std::vector<std::string> m_lineCache; ///< lines of the file.

			// this is problamatic if I want to process multiple files
			// at the same time.
			static File* m_current; ///< Current file being parsed
			FileError m_error; ///< Error when reading the file.
	};
}

#endif // FILE_H_
