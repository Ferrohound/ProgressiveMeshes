#ifndef PMESH_H
#define PMESH_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using std::vector;


//pseudo Vertex/edge collapse class
//Tells us where the edge is being sent to when it's collapsed
//====================================================================pEdge Class
class pVert
{
	Vertex* from;
	Vertex* to;
};

//=====================================================================pMESH CLASS
class pMesh
{
	public:
		pMesh(Mesh* m);
		~pMesh();
		
		//==========================================================Member functions
		//initialize the vert vector, col
		void Initialize();
		//update the number of verts in the progressive mesh
		void Update( int n );
		void Update2 ( int n );
		void Reset();
		Vertex* CheapestEdge() { return progressive->Cheapest(); }
		
		//draw the progressive mesh
		void Draw(GLuint programID, const glm::mat4 &MVP) { progressive->Draw( programID, MVP); }
	
	private:
		//keep a reference to the original mesh, display the progressive
		Mesh* original;
		Mesh* progressive;
		
		//vector of the edge collapses
		//edges are ordered in a way that the highest level of detail are the higher
		//indices, follow the "to" member variable until you're in bounds
		std::vector<pVert> col;
	
};
#endif
