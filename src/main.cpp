#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//SDL? probably not
//#define SDL_MAIN_HANDLED
//#include <SDL.h>

#include "controls.hpp"
#include "pMesh.h"
#include "shaderLoader.cpp"

using std::cout;

/*
	Progressive meshes
	Read:
		http://hhoppe.com/proj/pm/
		https://www.scribd.com/document/3124116/A-Simple-Fast-Effective-Polygon-Reduction-Algorithm
		
	Neil Clarke
*/


int main(int argc, char* argv[])
{
	set_root_path(argv[0]);
	
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}
	
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

	// Open a window and create its OpenGL context
	GLFWwindow* window; // (In the accompanying source code, this variable is global)
	window = glfwCreateWindow( 1024, 768, "Progressive Meshes", NULL, NULL);
	
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window); // Initialize GLEW
	glewExperimental=true; // Needed in core profile
	
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
	
	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "../data/shaders/ga_constant_color_vert.glsl",
		"../data/shaders/ga_constant_color_frag.glsl" );

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	
	//Create meshes
	Mesh* mesh = new Mesh("../data/models/bunny_1k.obj");
	pMesh* Progressive = new pMesh(mesh);
	int max = mesh->NumVerts();
	int current = max;
	
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	
	
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 30.0f);
	
	//main loop
	do
	{
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		
		//decrease the vertex count with spacebar
		if (glfwGetKey( GLFW_KEY_SPACE ) == GLFW_PRESS && current>0)
		{
			current--;
			Progressive->Update(current);
		}
		
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);
		
		float FoV = initialFoV - 5 * glfwGetMouseWheel();
		
		// Projection matrix : 45&deg; Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
		// Camera matrix
		ViewMatrix       = glm::lookAt(
			position,           // Camera is here
			position+direction, // and looks here : at the same position, plus "direction"
			up                  // Head is up (set to 0,-1,0 to look upside-down)
		);
		
		glm::mat4 ModelMatrix = glm::mat4(1.0f);
		
		glm::mat4 mvp = ProjectionMatrix * ViewMatrix * ModelMatrix;

		glUseProgram(programID);
		//Progressive->Draw();
		mesh->Draw(mvp);
		
	}while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		!glfwWindowShouldClose(window))
	
	return 0;
}
