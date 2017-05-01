#include <algorithm> /* min/max */

#include "Mesh.h"
#include "objLoader.cpp"


//================================ EDGE FUNCTIONS ==================================

//================================ TRIANGLE FUNCTIONS ===============================


//==============================MESH FUNCTIONS======================================

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices)
{
    this->vertices = vertices;
    this->indices = indices;
    //this->textures = textures;

    this->setupMesh();	
}

Mesh::Mesh(char* path)
{
	//loadOBJ(path, &this->vertices, &this->indices, &this->textures);
	//this->setupMesh();
}


//copy constructor
Mesh::Mesh( const Mesh& m)
{
	this->vertices = m.vertices;
	this->indices = m.indices;
	this->textures = m.textures;
	
	this->setupMesh();
}

Mesh::~Mesh()
{
	Clean();
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    //glGenBuffers(1, &this->EBO);
  
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), 
                 &this->vertices[0], GL_STATIC_DRAW);  
	
	/*
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), 
                 &this->indices[0], GL_STATIC_DRAW);
	*/

    // Vertex Positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                         (GLvoid*)0);
    // Vertex Normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                         (GLvoid*)offsetof(Vertex, Normal));
    // Vertex Texture Coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                         (GLvoid*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

//should probably remove this
/*
void Mesh::setVBO()
{
	 std::vector<glm::vec3> indexed_verts;
	
	for(int i=0; i<triangles.size();i++)
	{
		Triangle *t = triangles[i];
		
		// grab the vertex positions
		glm::vec3 a = (*t)[0]->getPos();
		glm::vec3 b = (*t)[1]->getPos();
		glm::vec3 c = (*t)[2]->getPos();
		
	}
	
	glBufferData(GL_ARRAY_BUFFER, indexed_verts.size() * sizeof(glm::vec3), &indexed_verts[0], GL_STATIC_DRAW);
	// the index data (refers to vertex data)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indeces.size() * sizeof(unsigned int), &indices[0] , GL_STATIC_DRAW);
	
}
*/

void Mesh::updateVBO()
{
	
}

//clean up and wrap up
void Mesh::Clean()
{
	glDeleteBuffers(1, &VAO);
	glDeleteBuffers(1, &VBO);
}


void Mesh::Draw( GLuint programID, const glm::mat4 &MVP)
{
	//glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	glUseProgram(programID);
	GLint loc = glGetUniformLocation(programID, "u_mvp");
	if (loc != -1)
	{
	   glUniformMatrix4fv(loc, 1, GL_FALSE, &MVP[0][0]);
	}
	
	//update indices in here ====================================================================
    for(int i=0; i<0; i++)
    {
		
    }

    // Draw mesh
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);	
}

// =====================================================edge collapse and vertex split

//collapse the edge based on the given algorithm
void Mesh::eCol(Vertex *u, Vertex *v)
{
	//pop the edge from the indices?
	if(v ==  NULL)
	{
		delete u;
		return;
	}
	
	std::vector<Edge*> tmp;
	for(int i=0 ; i<u->edges.size(); i++)
	{
		tmp.push_back(u->edges[i]);
	}
	
	//delete the shared
	for(int i=0; i<u->triangles.size();i++)
	{
		if(u->triangles[i]->Contains(v))
		{
			delete(u->triangles[i]);
			tmp.erase(tmp.begin()+i);
		}
	}
	
	for(int i=0; i<tmp.size(); i++)
	{
		tmp[i]->Replace(u, v);
	}
	
	delete u;
	
	for(int i=0; i<tmp.size(); i++)
	{
		ComputeEdgeCost(tmp[i]->begin);
	}
}

void Mesh::vSplit()
{
	
	
	
}

//get the cheapest edge
Vertex* Mesh::Cheapest()
{
	Vertex* tmp;
	float minCost = 10000;
	
	for(int i = 0; i<edges.size(); i++)
	{
		cost = Cost(edges[i].start, edges[i].end);
		if(cost<minCost)
		{
			tmp = edges[i].start;
			minCost = cost;
		}
		cost = Cost(edges[i].end, edges[i].start);
		if(cost<minCost)
		{
			tmp = edges[i].end;
			minCost = cost;
		}	
	}
	return tmp;
}