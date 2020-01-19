#ifdef _WIN32
//Stop error caused by Assimp using std::min
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <glad/glad.h>
//tell GLFW to include the glu header
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include "GLModel.h"
#include "Texture.h"
#include <stdexcept>
#include <ostream>
#include <iostream>
#ifndef IL_USE_PRAGMA_LIBS
#define IL_USE_PRAGMA_LIBS
#endif
#include <IL/il.h>
#include <IL/ilu.h>
#include "Model.hpp"
#include "AnimatedModel.hpp"
#include <propidlbase.h>

//Dragoon: Include directory in the base of the repo contains glad, GLFW I have installed on my path through vcpkg.
//Dragoon: I recommend using a package manager, it still allows you to use CMAKE with it.

//Dragoon: Include GLFW, this can be done via CMAKE or Visual studio project, but I like being explicit.
#ifdef _WIN32
#pragma comment(lib, "glfw3dll.lib")
#endif

void CheckErrors() {
	const char* errorStr = nullptr;
	int error = glfwGetError(&errorStr);
	if (error != GLFW_NO_ERROR) {
#ifdef _WIN32
		OutputDebugStringA(errorStr);
		OutputDebugStringA("\n");
#endif
		std::cout << errorStr << std::endl;
		error = glfwGetError(&errorStr);
	}

	error = glGetError();
	if (error != GL_NO_ERROR) {
		auto error_string = gluErrorString(error);
#ifdef _WIN32
		OutputDebugStringA((char*) error_string);
		OutputDebugStringA("\n");
#endif
		std::cout << error_string << std::endl;
	}
}

int main(int argc, char** argv)
{

	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	for (int i = 0; i < argc; i++)
	{
		OutputDebugStringA(argv[i]);
	}

	/* Initialize the GLFW library */
    if (!glfwInit())
        return -1;
	
    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(1280, 960, "GLModel Loader", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
	ilInit();
	iluInit();
	

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

	//Dragoon: This loads all the OpenGL API method function pointers using the glad library (https://github.com/Dav1dde/glad)
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	
	//Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
	//Disable culling
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
	glEnable(GL_TEXTURE_2D);
	
	GLModel* teapot = new GLModel(R"(assets/UtahTeapot.fbx)");
	
	if (!teapot->IsLoaded()) {
#ifdef _WIN32
		OutputDebugString("Failed to load GLModel\n");
#endif
		std::cout << "Failed to Load model" << std::endl;
		delete teapot;
        glfwTerminate();
        return -1;
    }
	
	char* imagePath = new char[20]{R"(assets\wall.jpg)"};
	Texture* texture = new Texture(imagePath);
	delete [] imagePath;
	imagePath = 0;

	CheckErrors(); 

	AnimatedModel animModel = AnimatedModel::ImportModel("assets/UtahTeapot.fbx", false, false);

	teapot->SetupGL_3_2();
	CheckErrors(); 
	animModel.setupGL();
	CheckErrors(); 
	
	float angle = 0;
	double startTime = glfwGetTime();
	double lastFrameTime = startTime;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
		CheckErrors(); 
    	const double currentTime = glfwGetTime()-startTime;
    	const double deltaTime = currentTime-lastFrameTime;
		angle += (float)deltaTime*90;
    	while (angle > 360) {
    		angle -= 360;
    	}

		texture->Bind();
    	
        /* Render here */
        glClearColor(0.0f, 0.8f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
        GLint windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective( 60, (double)windowWidth / (double)windowHeight, 0.1, 100000 );
		
    	
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0,0,-600);
        glRotatef(-90,1,0,0);
        glRotatef(angle,0,0,1);
    	
		teapot->DrawGL_3_2();

    	
        //glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0,0,-600);
        glRotatef(-90,1,0,0);
    	animModel.Draw3_2();
    	
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
		CheckErrors(); 
    	lastFrameTime = currentTime;
    }

	delete teapot;
	
    glfwTerminate();
    return 0;
}
