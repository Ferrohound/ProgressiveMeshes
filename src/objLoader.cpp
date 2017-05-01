
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>

#include "Mesh.h"


/*
	for loading the obj files
	getting their edges, faces & verts
*/

bool loadOBJ( const char* path,
	std::vector < glm::vec3 > & verts,
	std::vector < glm::vec2 > & uvs,
	std::vector < glm::vec3 > & normals)
	{	
		FILE* file = fopen(path, "r");
		
		if( file == NULL )
		{
			std::cout<<"File is null"<<std::endl;
			return false;
		}
		
		//in an attempt to avoid dubs
		std::set<double> verts;
		
		std::vector<double> vertices;
		std::vector<double> tex;
		std::vector<double> normal;
		//use .find(thing)
		
		/*
			v is vertex
			vt is the texture coordinate
			vn is the normal
			f is the face => indices start at 1, not 0, 
			formatted vertex/texture coord/normal
		*/
		String line;
		double x, y, z;
		double U, V;
		
		//initializing stuff
		vert = glm::vec3(x, y, z);
		normal = glm::vec3(x, y, z);
		UV = glm::vec3(U, V);
		
		
		while ( std::getline(file, line) )
		{
			//dealing with a vertex
			if( line.substr(0,2) == "v ")
			{
				std::istringstream v(line.substr(2));
				v>>x;
				v>>y;
				v>>z;
				
				
			}
			//texture coord
			else if( line.substr(0,2) == "vt" ) 
			{
				std::istringstream v(line.substr(3));
				
			}
			//normal
			else if( line.substr(0,2) == "vn" ) 
			{
				std::istringstream v(line.substr(3));
				
			}
			//face
			else if( line.substr(0,2) == "f ") 
			{
				std::istringstream v(line.substr(2));
				
			}
		}
		
		
		return false;
	}