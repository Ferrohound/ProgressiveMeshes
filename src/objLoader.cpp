
#include <stdio.h>
#include <iostream>
#include <string>
#include <ostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>

#include "Mesh.h"

using std::string;
using std::vector;
using std::ifstream;

/*
	for loading the obj files
	getting their edges, faces & verts
*/

inline bool loadOBJ( const char* path,
	vector<Vertex>& vertices, 
	vector<Triangle>& triangles, 
	/*vector<Edge>& edges,*/ 
	vector<GLuint>& indices)
	{	
		ifstream file (path);
		
		if( file == NULL )
		{
			std::cout<<"File is null"<<std::endl;
			return false;
		}

		std::vector<glm::vec2> UVs;
		std::vector<glm::vec3> normals;
		//use .find(thing)
		
		/*
			v is vertex
			vt is the texture coordinate
			vn is the normal
			f is the face => indices start at 1, not 0, 
			formatted vertex/texture coord/normal
		*/
		string line;
		double x, y, z;
		double U, V;
		
		//initializing stuff
		//glm::vec3 vert = glm::vec3(x, y, z);
		//glm::vec2 UV = glm::vec2(U, V);
		glm::vec3 norm = glm::vec3(x, y, z);
		
		std::cout<<"Reading OBJ File"<<std::endl;
		int count = 0;
		//for skipping
		char ch;
		while ( std::getline(file, line) )
		{
			//dealing with a vertex
			if( line.substr(0,2) == "v ")
			{
				//std::cout<<"Reading Vertices..."<<std::endl;
				std::istringstream v(line.substr(2));
				v>>x;
				v>>y;
				v>>z;
				Vertex vert;
				vert.Position = glm::vec3(x, y, z);
				vert.id = count;
				count++;
				vertices.push_back(vert);
				//std::cout<<"Done."<<std::endl;
				
			}
			//texture coord
			else if( line.substr(0,2) == "vt" ) 
			{
				//std::cout<<"Reading Texture coord..."<<std::endl;
				std::istringstream v(line.substr(3));
				v>>U;
				v>>V;
				glm::vec2 UV = glm::vec2(U, V);
				UVs.push_back(UV);
				//std::cout<<"Done."<<std::endl;
				
			}
			//normal
			else if( line.substr(0,2) == "vn" ) 
			{
				//std::cout<<"Reading Normal..."<<std::endl;
				std::istringstream v(line.substr(3));
				v>>x;
				v>>y;
				v>>z;
				normals.push_back(glm::vec3(x, y, z));
				//std::cout<<"Done."<<std::endl;
				
			}
			//create the faces
			else if( line.substr(0,2) == "f ") 
			{
				//std::cout<<"Reading Face..."<<std::endl;
				//std::istringstream v(line.substr(2));
				//int vert, tex, norm;
				int a, b, c;
				int n1, n2, n3;
				int t1, t2, t3;

				const char* ln=line.c_str();
				sscanf(ln, "f %i/%i/%i %i/%i/%i %i/%i/%i", &a, &t1, &n1, &b, &t2, &n2, &c, &t3, &n3);
				//std::cout<<ln<<std::endl;

				//obj files are indexed 1-n instead of 0
				a--; b--; c--;
				n1--; n2--; n3--;
				t1--; t2--; t3--;

				/*vertices[vert].Normal = normals[norm];
				vertices[vert].TexCoords = UVs[tex];*/

				//create the new triangle
				Triangle t = Triangle(&vertices[a], &vertices[b], &vertices[c]);
				triangles.push_back(t);

				vertices[a].Normal = normals[n1];
				vertices[a].TexCoords = UVs[t1];

				vertices[b].Normal = normals[n2];
				vertices[b].TexCoords = UVs[t2];

				vertices[c].Normal = normals[n3];
				vertices[c].TexCoords = UVs[t3];

				//make the edges and add them to the pot
				/*Edge ab = Edge(vertices[a], vertices[b], &t);
				Edge bc = Edge(vertices[b], vertices[c], &t);
				Edge ca = Edge(vertices[c], vertices[a], &t);
				*/
				vertices[a].edges.push_back(&vertices[b]);
				vertices[a].edges.push_back(&vertices[c]);

				vertices[b].edges.push_back(&vertices[a]);
				vertices[b].edges.push_back(&vertices[c]);

				vertices[c].edges.push_back(&vertices[a]);
				vertices[c].edges.push_back(&vertices[b]);
				/*
				edges.push_back(ab);
				edges.push_back(bc);
				edges.push_back(ca);*/

				//pushback the triangles
				vertices[a].triangles.push_back(&t);
				vertices[b].triangles.push_back(&t);
				vertices[c].triangles.push_back(&t);

				//push back the indices
				indices.push_back(a);
				indices.push_back(b);
				indices.push_back(c);
				//std::cout<<"Done."<<std::endl;
				
			}
		}
		
		std::cout<<"Mesh Loading Complete."<<std::endl;
		return true;
	}