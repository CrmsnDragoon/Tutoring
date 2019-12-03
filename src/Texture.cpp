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

void Texture::CheckError() {
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
}

Texture::Texture(char* imagePath) {
	//Dragoon: Doing this here because ilut includes the Windows OpenGL headers (on windows) to do it's thing, so we can't do this in main.
	//This isn't a library or linker conflict because OpenGL uses function pointers, and they compile out to have different names
	if (!ilIsInit) {
		ilutInit();
		ilutRenderer(ILUT_OPENGL); 
		//ilutEnable(ILUT_OPENGL_CONV);
		ilIsInit = true;
	}
	
	image = ilutGLLoadImage(imagePath);
	if (image == 0) {
		CheckError();
		throw std::runtime_error("Failed to load image");
	}

	ILboolean success;
	ILuint imageID;

	ilGenImages(1,&imageID);
	ilBindImage(imageID);
	
	if (!ilLoadImage(imagePath)) {
		CheckError();
	}
	
	success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	
	if (!success) {
		CheckError();
		throw std::runtime_error("Failed to load image");
	}
	
	glGenTextures(1,&_textureID);
	glBindTexture(GL_TEXTURE_2D,_textureID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,ilGetInteger(IL_IMAGE_BPP),ilGetInteger(IL_IMAGE_WIDTH),
	        ilGetInteger(IL_IMAGE_HEIGHT),0,ilGetInteger(IL_IMAGE_FORMAT),GL_UNSIGNED_BYTE,
	        ilGetData());


	glBindTexture(GL_TEXTURE_2D,0);
	ilDeleteImages(1,&imageID);
	if (_textureID == 0) {
		CheckError();
		throw std::runtime_error("Failed to load image");
	}
}

Texture::~Texture() {
	glDeleteTextures(1,&_textureID);
	glDeleteTextures(1,&image);
}

void Texture::Bind() {
	glBindTexture(GL_TEXTURE_2D, image);
}
