#ifndef RHO_CONTEXT_H_
#define RHO_CONTEXT_H_


enum OS {
	Windows = 0,
	Linux,
	OSX
};

enum Arch {
	X86 = 0,
	X64
};

enum Endian {
	Little = 0,
	Big
};

namespace parser {

	class RhoContext {
		public:
			RhoContext() {
#ifdef _WIN32
		#error OS Not Supported
#elif __APPLE__
	m_os = OSX;
	m_arch = X64;
	m_endian = Little;
	m_version = "0.0.1";
#elif __LINUX__
		#error OS Not Supported
#endif
			}

		private:
			OS m_os;
			Arch m_arch;
			Endian m_endian;
			std::string m_version; 	// language version
			std::string m_corePath; // location of the standard library
	};

}


#endif // RHO_CONTEXT_H_
