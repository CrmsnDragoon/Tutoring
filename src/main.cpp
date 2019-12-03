#ifdef _WIN32
//Stop error caused by Assimp using std::min
#define NOMINMAX
#include <windows.h>
#endif
#include <glad/glad.h>
//tell GLFW to include the glu header
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include "Model.h"
#include "Texture.h"
#include <stdexcept>
#include <ostream>
#include <iostream>
#ifndef IL_USE_PRAGMA_LIBS
#define IL_USE_PRAGMA_LIBS
#endif
#include <IL/il.h>
#include <IL/ilu.h>

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
#ifdef _WIN32
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
#endif

	for (size_t i = 0; i < argc; i++)
	{
		OutputDebugStringA(argv[i]);
	}
	
    GLFWwindow* window;

    /* Initialize the GLFW library */
    if (!glfwInit())
        return -1;
	
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Model Loader", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
	ilInit();
	

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
	
	Model* teapot = new Model(R"(assets\UtahTeapot.fbx)");

	if (!teapot->IsLoaded()) {
#ifdef _WIN32
		OutputDebugString("Failed to load Model\n");
#endif
		std::cout << "Failed to Load model" << std::endl;
		delete teapot;
        glfwTerminate();
        return -1;
    }
	
	char* imagePath = new char[71]{R"(assets\red-black-gradient.png)"};
	Texture* gradient = new Texture(imagePath);
	delete [] imagePath;
	imagePath = 0;
	
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,gradient->image);
	
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	CheckErrors(); 
	
	float angle = 0;
	double startTime = glfwGetTime();
	double lastFrameTime = startTime;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
    	const double currentTime = glfwGetTime()-startTime;
    	const double deltaTime = currentTime-lastFrameTime;
		angle += (float)deltaTime*90;
    	while (angle > 360) {
    		angle -= 360;
    	}
    	
        /* Render here */
        glClearColor(0.0, 0.8, 0.3, 1.0);
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
    	
		teapot->DrawGL_1_0();
    	
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
