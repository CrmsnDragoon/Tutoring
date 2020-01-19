#pragma once
#include <cstdint>
#include <assimp/postprocess.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef size_t u64;
typedef float f32;
typedef double f64;

namespace Utils {
	void PlatformTextOut(const char* formatString, ...) ;
}