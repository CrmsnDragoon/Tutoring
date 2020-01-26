#include "Util.hpp"
#include <cstdint>
#include <assimp/postprocess.h>
#include <cstdarg>
#ifdef _WIN32
//Stop error caused by Assimp using std::min
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace Utils {
	uint32_t AssimpImportFlags(const bool flipUVs, const bool flipWindingOrder) {
		uint32_t flags = aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_GenNormals | 
			aiProcess_CalcTangentSpace;
		if (flipUVs) { flags |= aiProcess_FlipUVs; }
		if (flipWindingOrder) { flags |= aiProcess_FlipWindingOrder; }
		return flags;
	}
	void PlatformTextOut(const char* formatString, ...)  {
		va_list args;
		va_start(args, formatString);
		char sz_buffer[1024]; // get rid of this hard-coded buffer
		const auto num_chars_output = _vsnprintf_s(sz_buffer, _TRUNCATE, formatString, args);
		if (num_chars_output > -1) {
#ifdef _WIN32
			OutputDebugStringA(sz_buffer);
#endif

			printf("%s",sz_buffer);
		}
		else {
#ifdef _WIN32
			OutputDebugStringW(L"Failed to print for some reason\n");
#endif
		}
		va_end(args);
	}
}
