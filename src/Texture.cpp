#include "Texture.h"

#ifndef IL_USE_PRAGMA_LIBS
#define IL_USE_PRAGMA_LIBS
#endif
#define ILUT_USE_OPENGL
#define ILUT_USE_WIN32
#include <IL/ilut.h>
#include <stdexcept>
#include <ostream>
#include <iostream>

static bool ilIsInit = false;

Texture::Texture(char* imagePath) {
	//Dragoon: Doing this here because ilut includes the Windows OpenGL headers (on windows) to do it's thing, so we can't do this in main.
	//This isn't a conflict because OpenGL uses function pointers, and they compile out to have different names
	if (!ilIsInit) {
		ilutRenderer(ILUT_OPENGL); 
		ilutEnable(ILUT_OPENGL_CONV);
		ilIsInit = true;
	}
	ilutGLBindTexImage();
	image = ilutGLLoadImage(imagePath);
	ILenum error = ilGetError();
	//Print out errors
	while (error != IL_NO_ERROR) {
#ifdef _WIN32
  		OutputDebugStringA(iluErrorString(error));
		OutputDebugStringA("\n");
#endif

		std::cout << (iluErrorString(error)) << std::endl;
		error = ilGetError();
	}
	if (image == 0) {
		throw std::runtime_error("Failed to load image");
	}
}

Texture::~Texture() {
	glDeleteTextures(1,&image);
	ilDeleteImages(1,&image);
}
