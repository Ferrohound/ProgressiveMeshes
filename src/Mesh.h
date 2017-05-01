#ifndef MESH_H
#define MESH_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


/*
	half edge bastardization
*/

class Vertex;
class Triangle;

//===========================================================================VERTEX
struct Vertex
{		
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	//list of triangles which contain this vert
	std::vector<Triangle*> triangles;
	std::vector<Edge*> edges;
	int id;
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
		
		//list of triangles which contain this edge
		Triangle** triangles;
		
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
		
		Triangle( Vertex* a, Vertex* b, Vertex* c);
		Triangle& operator = ( const Triangle& t );
		
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
		
		//check if the triangle contains vert 
		void Contains(Vertex* vert);
		//replace vertex v with vertex u
		void Replace(Vertex* u, Vertex* v);
		//calculate & update the normal
		void UpdateNormal();
		
		//edge?
		Edge* edge;
		glm::vec3 normal;
		
	
};

//===========================================================================MESH
class Mesh
{		
	public:
	
		int numVerts() const { return verts.size(); }
		
		public:
        /*  Mesh Data  */
        //================================================================Mesh Functions
		Mesh();
        Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);
		Mesh(char* path);
		Mesh(const Mesh& m);
		~Mesh();
		
		void setVBO();
		void updateVBO();
		void Clean();
		
		// =====================================================edge collapse and vertex split
		void eCol(Vertex *u, Vertex *v);
		void vSplit();
		
		//function to get the mesh to the correct level of detail
		//have a number from 0-1, display the currect number of verts proportional to that
		void Update(float n);
		//return the number of vertices
		int NumVerts() { return vertices.size(); }
		Vertex* Cheapest();
		
	private:
        /*  Render data  */
        GLuint VAO, VBO;
        /*  Functions    */
        void setupMesh();
		
		//representation
		vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;
		vector<Triangle> triangles;
		vector<Edge> edges;
};


//===================================================Helper Functions================
//compute the cost of an edge collapse
/*
cost n
	u,v u v f normal n normal f Tu Tuv ( ) =−× − • { } { }
*/
float Cost(Vertex *u, Vertex *v)
{
	float length = glm::Distance(u->Position, v->Position);
	float curve = 0;
	
	std::vector<Triangle*>shared;
	//get the triangles that share edge uv
	for(int i=0; i<u->triangles.size(); i++)
	{
		if(u->triangles[i].Contains(v))
			shared.push_back(u->triangles[i]);
	}
	
	//use the face facing the most away for curvature
	for( int i=0; i<u->triangles.size(); i++)
	{
		float min = 1;
		for( int j=0; j<shared.size(); j++)
		{
			float dot = glm::dot(u->triangles[i].normal, shared[j]->normal);
			min = std::min(min, (1-dot)/2.0);
		}
		curve = std::max(curve, min);
	}
	
	return length * curve;
}

void ComputeEdgeCost(Vertex* v)
{
	
}


#endif
