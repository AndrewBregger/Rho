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
			File(const str::string& _path, FileMode _mode, const Content& _content);

			/// @brief Gets the current content of the file.
			const Content& content();

			static void SetFile(File* _file);

			const char* GetPath() {
				return to_str(m_path);
			}

			/// @brief Reads an entire file
			/// @param _path path of file being read.
			static File* read_file(const str::string& _path);

			/// @brief Gets a line from the file.
			/// @param _line Line of the file being returned. Starts at 1.
			str::string GetLine(size_t _line);

		private:
			/// @brief Divides the file into lines.
			void SplitIntoLines();


			Content m_content; ///< Current content of the file and size
			str::string m_path; ///< Path to the file on disk.
			FileMode m_mode; ///< IO mode of the file.
			std::vector<str::string> m_lineCache; ///< lines of the file.

			// this is problamatic if I want to process multiple files 
			// at the same time.
			static File* m_current; ///< Current file being parsed
	};
}

#endif // FILE_H_
