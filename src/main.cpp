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
#include "AnimatedModel.hpp"
#include <propidlbase.h>
#include "AnimationPlayer.hpp"
#include "Util.hpp"

//Dragoon: Include directory in the base of the repo contains glad, GLFW I have installed on my path through vcpkg.
//Dragoon: I recommend using a package manager, it still allows you to use CMAKE with it.

//Dragoon: Include GLFW, this can be done via CMAKE or Visual studio project, but I like being explicit.
#ifdef _WIN32
#pragma comment(lib, "glfw3dll.lib")
#endif

void CheckErrors() {
	const char* errorStr = nullptr;
	int error = glfwGetError(&errorStr);
	while (error != GLFW_NO_ERROR) {
#ifdef _WIN32
		OutputDebugStringA(errorStr);
		OutputDebugStringA("\n");
#endif
		std::cout << errorStr << std::endl;
		error = glfwGetError(&errorStr);
	}

	error = glGetError();
	while (error != GL_NO_ERROR) {
		auto error_string = gluErrorString(error);
#ifdef _WIN32
		OutputDebugStringA((char*) error_string);
		OutputDebugStringA("\n");
#endif
		std::cout << error_string << std::endl;
	}
}


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

std::string GetFileAsString(std::string filePath) {
    std::string outString;
    std::ifstream file;
    // ensure ifstream objects can throw exceptions:
    file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        // open files
        file.open(filePath);
        std::stringstream strStream;
        // read file's buffer contents into streams
        strStream << file.rdbuf();		
        // close file handlers
        file.close();
        // convert stream into string
        outString = strStream.str();		
    }
    catch(std::ifstream::failure e)
    {
		Utils::PlatformTextOut("Error: Fail to read shader %s", filePath.c_str());
    }
	return outString;
}

void CreateShader(GLuint& texProgram, const std::string vertexShaderPath, const std::string fragmentShaderPath) {
	char informationLog[512]{};

	auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
	auto fragmentShader =  glCreateShader(GL_FRAGMENT_SHADER);

	std::string shaderSrc = GetFileAsString(vertexShaderPath);
	auto shaderCode = shaderSrc.c_str();
	glShaderSource(vertexShader, 1, &shaderCode, nullptr);
	glCompileShader(vertexShader);
	int success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, nullptr, informationLog);
		Utils::PlatformTextOut("GL Error: %s\n", informationLog);
	}
	
	shaderSrc = GetFileAsString(fragmentShaderPath);
	shaderCode = shaderSrc.c_str();
	glShaderSource(fragmentShader, 1, &shaderCode, nullptr);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success ) {
		glGetShaderInfoLog(fragmentShader, 512, nullptr, informationLog);
		Utils::PlatformTextOut("GL Error: %s\n", informationLog);
	}

	texProgram = glCreateProgram();
	glAttachShader(texProgram, vertexShader);
	glAttachShader(texProgram, fragmentShader);
	glLinkProgram(texProgram);
	glGetProgramiv(texProgram, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(texProgram, 512, nullptr, informationLog);
		Utils::PlatformTextOut("GL Error: %s\n", informationLog);
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

int main(int argc, char** argv)
{

	HRESULT res = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	if (FAILED(res)) {
		return 0;
	}
	
	for (int i = 0; i < argc; i++)
	{
		OutputDebugStringA(argv[i]);
	}
	
	/* Initialize the GLFW library */
    if (!glfwInit())
        return -1;
    GLFWwindow* window = glfwCreateWindow(1280, 960, "GLModel Loader", NULL, NULL);
    if (!window)
    {
		const char* desc;
    	glfwGetError(&desc);
    	Utils::PlatformTextOut("GLFW Error: %s", desc);
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

	AnimatedModel animTeapot = AnimatedModel::ImportModel("assets/UtahTeapot.fbx", false, false);
	AnimatedModel animModel = AnimatedModel::ImportModel("assets/Cheering.fbx", false, false);
	
	animTeapot.rootNode->position.z = -1000;
    
	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(XM_2PI/360*90,0,0);
    
	XMStoreFloat4(&animTeapot.rootNode->rotation, quat);
	animTeapot.rootNode->UpdateTransform();
	
	animModel.rootNode->position.z = -1000;
	animModel.rootNode->UpdateTransform();
	
	teapot->SetupGL_3_2();
	CheckErrors(); 
	animTeapot.setupGL();
	CheckErrors(); 
	animModel.setupGL();
	CheckErrors(); 
	
	AnimationPlayer::AnimationPlayerState state[2];
	
	AnimationPlayer::SetModel(state[0], &animModel);
	AnimationPlayer::SetAnimationInterpolation(state[0], true);
	AnimationPlayer::PlayClip(state[0], true, 0);
	
	AnimationPlayer::SetModel(state[1], &animTeapot);
	AnimationPlayer::SetAnimationInterpolation(state[1], true);
	AnimationPlayer::PlayClip(state[1], true, 0);

	//Setup shader programs
	GLuint texProgram, boneProgram;
	CreateShader(texProgram, "assets/Shaders/tex.vert", "assets/Shaders/tex.frag");
	CreateShader(boneProgram, "assets/Shaders/bone.vert", "assets/Shaders/bone.frag");
	
	XMFLOAT3 upVec = {1,0,1};
	XMVECTOR up = XMLoadFloat3(&upVec);
	float angle = 0;
	double startTime = glfwGetTime();
	double lastFrameTime = startTime;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
    	glfwPollEvents();
    	
		CheckErrors(); 
    	const double currentTime = glfwGetTime()-startTime;
    	const double deltaTime = currentTime-lastFrameTime;
		angle += float(deltaTime*90);
    	while (angle > 360) {
    		angle -= 360;
    	}
    	
		quat = XMQuaternionRotationRollPitchYaw(XM_2PI/360*90,XM_2PI/360*angle,0);
    	
		XMStoreFloat4(&animTeapot.rootNode->rotation, quat);
		animTeapot.rootNode->UpdateTransform();
		animModel.rootNode->UpdateTransform();
    	
    	AnimationPlayer::Update(float(deltaTime), state, 2);
    	
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
        glTranslatef(250,300,0);
    	glScalef(0.5f,0.5f,0.5f);
        glTranslatef(0,0,-1200);
        glRotatef(-90,1,0,0);
        glRotatef(angle,0,0,1);
    	
    	glUseProgram(texProgram);
		teapot->DrawGL_3_2();
		CheckErrors(); 

        //glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
       /* glTranslatef(0,0,-600);
        glRotatef(angle,0,1,0);
        */

    	glUseProgram(boneProgram);

    	const unsigned NUM_MATS = 5;
		std::vector<float> mats = std::vector<float>(NUM_MATS*4*4,1);
    	uint32_t index = 0;
		for (uint32_t mat_index = 0; mat_index < NUM_MATS; ++mat_index) {
			for (uint32_t y = 0; y < 4; ++y) {
				for (uint32_t x = 0; x < 4; ++x) {
					//This is transposing the matricies as DirectXMath is Row Major by default (glm is also)
					//mats[index++] = state[1].boneMats[mat_index].m[x][y];
					auto transMat = XMLoadFloat4x4(&state[1].boneMats[mat_index]);
					transMat = XMMatrixTranspose(transMat);
					mats[index++] = transMat.r[x].m128_f32[y];
				}	
			}
		}
    	//Presuming that this doesn't work
    	//TODO: Replace with a buffer update
    	glUniformMatrix4fv(glGetUniformLocation(boneProgram,"boneMats"), NUM_MATS, GL_FALSE, mats.data());

    	animTeapot.Draw3_2();
    	
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
		CheckErrors(); 
    	lastFrameTime = currentTime;
    }

	glDeleteProgram(texProgram);
	glDeleteProgram(boneProgram);

	animModel.Shutdown();
	
	delete teapot;
	
    glfwTerminate();
    return 0;
}
