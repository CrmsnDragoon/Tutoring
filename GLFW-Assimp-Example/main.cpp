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

int main(int argc, char** argv)
{
#ifdef _WIN32
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
#endif
	
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
	
	Model* teapot = new Model(R"(G:\Projects\Tutoring\GLFW-Assimp-Example\assets\UtahTeapot.fbx)");

	if (!teapot->IsLoaded()) {
#ifdef _WIN32
		OutputDebugString("Failed to load Model\n");
#endif
		std::cout << "Failed to Load model" << std::endl;
		delete teapot;
        glfwTerminate();
        return -1;
    }
	
	char* imagePath = new char[71]{R"(G:\Projects\Tutoring\GLFW-Assimp-Example\assets\red-black-gradient.png)"};
	Texture* gradient = new Texture(imagePath);
	delete [] imagePath;
	imagePath = 0;
	
	glEnable(GL_TEXTURE);
	
	glBindTexture(GL_TEXTURE_2D,gradient->image);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glGenerateMipmap(GL_TEXTURE_2D);
	
	float angle = 0;
	double startTime = glfwGetTime();
	double lastFrameTime = startTime;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
    	const double currentTime = glfwGetTime()-startTime;
    	const double deltaTime = currentTime-lastFrameTime;
		angle += (float)deltaTime*90;
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
		
        GLint windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);

        // Draw stuff
        glClearColor(0.0, 0.8, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION_MATRIX);
        glLoadIdentity();
        gluPerspective( 60, (double)windowWidth / (double)windowHeight, 0.1, 100000 );

        glMatrixMode(GL_MODELVIEW_MATRIX);
        glTranslatef(0,0,-600);
        glRotatef(-90,1,0,0);
        glRotatef(angle,0,0,1);
    	
		teapot->DrawGL_1_0();
    	
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    	lastFrameTime = currentTime;
    }

	delete teapot;
	
    glfwTerminate();
    return 0;
}