#pragma once
#include <cstdint>
#include <assimp/postprocess.h>
#include <cstdarg>
#ifdef _WIN32
//Stop error caused by Assimp using std::min
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <glad/glad.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef size_t u64;
typedef float f32;
typedef double f64;

namespace Utils {
	inline uint32_t AssimpImportFlags(const bool flipUVs, const bool flipWindingOrder) {
		uint32_t flags = aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_GenNormals | 
			aiProcess_CalcTangentSpace;
		if (flipUVs) { flags |= aiProcess_FlipUVs; }
		if (flipWindingOrder) { flags |= aiProcess_FlipWindingOrder; }
		return flags;
	}
	inline void PlatformTextOut(const char* formatString, ...)  {
		va_list args;
		va_start(args, formatString);
		char sz_buffer[512]; // get rid of this hard-coded buffer
		const auto num_chars_output = _vsnprintf_s(sz_buffer, _TRUNCATE, formatString, args);
		if (num_chars_output > -1) {
			OutputDebugStringA(sz_buffer);
			printf("%s",sz_buffer);
		}
		else {
			OutputDebugStringW(L"Failed to print for some reason\n");
		}
		va_end(args);
	}
}