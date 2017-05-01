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
#define SDL_MAIN_HANDLED
#include <SDL.h>

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

void Movement()
{
	// Move forward
	if (glfwGetKey( GLFW_KEY_UP ) == GLFW_PRESS)
	{
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( GLFW_KEY_DOWN ) == GLFW_PRESS)
	{
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( GLFW_KEY_RIGHT ) == GLFW_PRESS)
	{
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( GLFW_KEY_LEFT ) == GLFW_PRESS || glfwGetKey( GLFW_KEY_A) == GLFW_PRESS)
	{
		position -= right * deltaTime * speed;
	}
}


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
		
		int xpos, ypos;
		glfwGetMousePos(&xpos, &ypos);
		
		// Reset mouse position for next frame
		glfwSetMousePos(1024/2, 768/2);
		
		// Compute new orientation
		horizontalAngle += mouseSpeed * deltaTime * float(1024/2 - xpos );
		verticalAngle   += mouseSpeed * deltaTime * float( 768/2 - ypos );
		
		glm::vec3 direction(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
		);
		
		// Right vector
		glm::vec3 right = glm::vec3(
			sin(horizontalAngle - 3.14f/2.0f),
			0,
			cos(horizontalAngle - 3.14f/2.0f)
		);
		
		// Up vector : perpendicular to both direction and right
		glm::vec3 up = glm::cross( right, direction );
		
		Movement();
		
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