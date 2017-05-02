#include <algorithm> /* min/max */

#include "Mesh.h"
#include "objLoader.cpp"


//================================ EDGE FUNCTIONS ==================================

/*Edge::Edge(Vertex &s, Vertex &e, Triangle *t)
{
	start = &s;
	end = &e;
	triangles.push_back(*t);
}
*/
//================================ TRIANGLE FUNCTIONS ===============================


bool Triangle::Contains(Vertex* vert)
{
	if(vert == NULL)
		return false;
	if(verts[0] == vert || verts[1] == vert || verts[2] == vert)
		return true;
	return false;
}

void Triangle::Replace(Vertex* u, Vertex* v)
{
	for( int i = 0; i<3 ; i++)
	{
		if(verts[i] == u)
		{
			verts[i] = v;
			return;
		}
	}
}
//==============================MESH FUNCTIONS======================================

Mesh::Mesh(vector<Vertex>& vertices, vector<Triangle>& triangles, /*vector<Edge>& edges,*/ vector<GLuint>& indices)
{
    this->vertices = vertices;
    this->indices = indices;
    //this->textures = textures;

    this->setupMesh();	
}

Mesh::Mesh(char* path)
{
	loadOBJ(path, this->vertices, this->triangles, /*this->edges,*/ this->indices/*, &this->textures*/);
	this->setupMesh();
}


//copy constructor
Mesh::Mesh( const Mesh& m)
{
	this->vertices = m.vertices;
	this->triangles = m.triangles;
	//this->edges = m.edges;
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
    glGenBuffers(1, &this->EBO);
  
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), 
                 &this->vertices[0], GL_STATIC_DRAW);  
	

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), 
                 &this->indices[0], GL_STATIC_DRAW);

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
	glDeleteBuffers(1, &EBO);
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
	
	//update indices in here (probably) ====================================================================
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
void Mesh::eCol(Vertex *u, Vertex *v/*, Edge* e*/)
{
	std::cout<<"Collapsing Edge Now..."<<std::endl;
	//pop the edge from the indices?
	if(v ==  NULL)
	{
		std::cout<<"V is null!"<<std::endl;
		for(int i=0 ; i< vertices.size(); i++)
		{
			if(vertices[i].id == u->id)
			{
				vertices.erase(vertices.begin()+i);
				break;
			}
		}
		//vec.erase(std::remove(vec.begin(), vec.end(), int_to_remove), vec.end());
		std::cout<<"Lone Edge\nDone."<<std::endl;
		//delete e;
		//delete u;
		return;
	}
	
	//get the edge to remove
	//std::vector<Edge*> tmp;
	/*
	for(int i=0 ; i<u->edges.size(); i++)
	{
		tmp.push_back(&u->edges[i]);
		if(u->edges[i].end == v || u->edges[i].start == v)
			e = &u->edges[i];
	}*/
	
	//delete the shared
	for(int i=0; i<u->triangles.size();i++)
	{
		if(u->triangles[i]->Contains(v))
		{
			//delete(u->triangles[i]);
			for(int j = 0; j<triangles.size(); j++)
			{
				Triangle k = triangles[j];
				if(triangles[j] == k)
					triangles.erase(triangles.begin()+j);
			}
			u->triangles.erase(u->triangles.begin()+i);
			//tmp.erase(tmp.begin()+i);
		}
	}
	
	for(int i=0; i<u->triangles.size(); i++)
	{
		u->triangles[i]->Replace(u, v);
	}

	//remove this vertex from its edges
	for(int i=0; i<u->edges.size(); i++)
	{
		for(int j = 0; j<u->edges[i]->edges.size();j++)
		{
			if(u->edges[i]->edges[j] == u)
			{
				u->edges[i]->edges.erase(u->edges[i]->edges.begin() + j);
			}
		}
	}
	
	//need to remove edge u, v from the list as well
	//delete u;
	for(int i=0 ; i< vertices.size(); i++)
	{
		//std::cout<<vertices[i].id<<" "<<u->id<<std::endl;
		if(vertices[i].id == u->id)
		{
			vertices.erase(vertices.begin()+i);
			std::cout<<"Found you!"<<std::endl;
			break;
		}
	}

/*
	for (int i = 0; i<edges.size(); i++)
	{
		Edge test = edges[i];
		if(e->start == test.start && e->end == test.end)
		{
			edges.erase(edges.begin()+i);
		}
		if(e->start == test.end && e->end == test.start)
		{
			edges.erase(edges.begin()+i);
		}
	}*/
	
	/*for(int i=0; i<tmp.size(); i++)
	{
		ComputeEdgeCost(tmp[i]->start);
	}*/
	std::cout<<"Done."<<std::endl;
	std::cout<<"Verts remaining=> "<<NumVerts()<<std::endl<<vertices.size()<<std::endl;
}

//no longer need this
void Mesh::vSplit()
{
	
	
	
}

//get the cheapest edge
Vertex* Mesh::Cheapest()
{
	Vertex* rtn;
	Vertex* tmp;
	Vertex* tmp2;

	Vertex* a;
	Vertex* b;
	//Edge* edge;
	float minCost = 10000;
	float cost = 0;

	std::cout<<"Searching for the cheapest now"<<std::endl;
	//std::cout<<edges.size()<<" edges remaining."<<std::endl;
	std::cout<<NumVerts()<<" verts remaining"<<std::endl;

	for(int i = 0; i<vertices.size(); i++)
	{
		//std::cout<<"bump"<<std::endl;
		a = &vertices[i];
		for(int j = 0; j<a->edges.size(); j++)
		{
			//std::cout<<"bump"<<std::endl;
			b = a->edges[j];
			//cost =& Cost(edges[i].start, edges[i].end);
			cost = Cost(a, b);
			//std::cout<<"Cost is..."<<cost<<std::endl;
			if(cost<minCost)
			{
				//tmp = edges[i].start;
				tmp = a;
				tmp2 = b;
				minCost = cost;
				//edge = &edges[i];
			}
			//cost = Cost(edges[i].start, edges[i].end);
			cost = Cost(b, a);
			//std::cout<<"Cost is..."<<cost<<std::endl;
			if(cost<minCost)
			{
				//tmp = edges[i].end;
				tmp = b;
				tmp2 = a;
				minCost = cost;
				//edge = &edges[i];
			}	
		}
	}
	//set this edge's destiny vertex
	std::cout<<"Found the cheapest."<<std::endl;
	tmp->destiny = tmp2;

	rtn = tmp;

	std::cout<<"Returning the cheapest vert to remove"<<std::endl;
	return rtn;
}