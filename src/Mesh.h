#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"

/*
	half edge bastardization
*/

class Vertex;
class Triangle;

//===========================================================================VERTEX
class Vertex
{
	public:
		Vertex(const glm::vec3 & pos) : position(pos) {}
		glm::vec3 position;
	
	
};

//===========================================================================EDGE
class Edge
{
	public:
		Edge(Vertex *s, Vertex *e, Triangle *t);
		~Edge();
	
	//============================================================Edge Member Variables
		Edge* next;
		Edge* opposite;
		
		Vertex* start;
		Vertex* end;
		
		Triangle* triangle;
		
		float length;
		
	//============================================================Edge Member functions
};

//===========================================================================TRIANGLE
class Triangle
{
	public:
		Triangle()
		{
			edge = NULL;
		}
		
		//===========================================================Triangle Members
		
		Vertex* operator[](int i) const
		{
			if(edge == NULL || i>2)
				return NULL;
			
			if( i == 0 )
				return edge->start;
			if( i == 1 )
				return edge->next->start;
			if( i == 2 )
				return edge->next->next->start;
		}
		Edge* edge;
		
	
};

//===========================================================================MESH
class Mesh
{		
	public:
		
		Mesh();
		int numVerts() const { return verts.size(); }
		
	// =====================================================edge collapse and vertex split
		void eCol();
		void vSplit();
		
		initializeVBO();
		setVBO();
		clearVBO();
		
		Edge* getEdge(Edge* edge);
		
		
	private:
		std::vector<Vertex*> verts;
		std::vector<Edge*> edges;
		std::vector<Triangle*> triangles;
		
		GLuint VAO;
		GLuint vertVBO;
		GLuint indices;
};
