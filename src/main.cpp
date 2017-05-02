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

//#include "controls.hpp"
#include "Mesh.h"
#include "pMesh.h"
#include "shaderLoader.cpp"

using std::cout;
using std::vector;

/*
	Progressive meshes
	Read:
		http://hhoppe.com/proj/pm/
		https://www.scribd.com/document/3124116/A-Simple-Fast-Effective-Polygon-Reduction-Algorithm
		
	Neil Clarke
*/

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
} 


// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 5 ); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;

void computeMatricesFromInputs(GLFWwindow* window){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024/2 - xpos );
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
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
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}



int main(int argc, char* argv[])
{
	//set_root_path(argv[0]);
	
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
	
	// Create and compile our GLSL program from the shaders, from openGL tutorial
	GLuint programID = LoadShaders( "../data/shaders/ga_constant_color_vert.glsl",
		"../data/shaders/ga_constant_color_frag.glsl" );

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	
	//Create meshes
	Mesh* mesh = new Mesh("../data/models/suzanne.obj");
	//pMesh* Progressive = new pMesh(mesh);
	int max = /*mesh->NumVerts()*/100;
	int current = max;
	
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	
	
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 2000.0f);
	
	//main loop
	do
	{
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs(window);
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		
		//decrease the vertex count with spacebar
		if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS && current>0)
		{
			current--;
			Vertex* tmp = mesh->Cheapest();
			std::cout<<"looking for vertex with index "<<tmp->id<<std::endl;
			//Progressive->Update(current);
			mesh->eCol((Vertex*)tmp, (Vertex*)tmp->destiny/*, NULL*/);
			cout<<current<<std::endl;
		}

		if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && current < 100)
		{

			current++;
			//Progressive->Update(current);
			cout<<current<<std::endl;
		}

		//glUseProgram(programID);
		//Progressive->Draw();
		//Progressive->Draw(programID, MVP);
		mesh->Draw(programID, MVP);

		// Swap buffers, apparently this is important
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		!glfwWindowShouldClose(window));

	glfwTerminate();
	
	return 0;
}
