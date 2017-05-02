#include <vector>

#include "pMesh.h"


pMesh::pMesh(Mesh* m, float distance)
{
	//ideally make a copy of mesh m and store it inside of progressive
	original = m;
	progressive = new Mesh(*m);
	maxD = maxD;
	Initialize();
}


pMesh::~pMesh()
{
	delete original;
	delete progressive;
}

void pMesh::Initialize()
{
	//collapse the mesh to 0
	std::vector<Vertex*> tmp;
	while ( progressive->NumVerts() > 0 )
	{
		Vertex *cheapest = progressive->Cheapest();
		tmp.push_back(cheapest);
		progressive->eCol(cheapest, cheapest->destiny/*, NULL*/); //==========================================FIX THIS
		//reverse the order of pVert and ID the vertices accordingly
		for( int i = tmp.size()-1 ; i > -1 ; i -- )
		{
			tmp[i]->id = col.size();
			
			pVert V;
			V.from = tmp[i];
			V.to = tmp[i]->destiny;

			col.push_back(V);
		}
	
	}
}


//for split and collapse
//this method doesn't use the pVert array 
void pMesh::Update( int n )
{
	if( n < 0 || n > original->NumVerts() || n == progressive->NumVerts())
		return;
	
	//collapse some edges
	if ( n < progressive->NumVerts() )
	{
		while ( progressive->NumVerts() != n )
		{
			Vertex *cheapest = CheapestEdge();
			//tmp.push_back(cheapest);
			progressive->eCol(cheapest, cheapest->destiny/*, NULL*/);
		}
	}
	
	else
	{
		while ( progressive->NumVerts() != n )
		{
			Vertex *cheapest = CheapestEdge();
			//tmp.push_back(cheapest);
			progressive->vSplit();	
		}
	}
}

//this methd does used the preserved info from Initialize()
//Update the mesh proportionally to the maxD set
void pMesh::Update2 ( float distance )
{
	//percentage-wise
	if(distance > maxD)
		LOD = 0;
	else
	{
		LOD = col.size() * (distance/maxD);
	}

	//Recreate the triangles, using LOD as the filter for how far in to go



}

void pMesh::Reset()
{
	delete progressive;
	progressive = new Mesh(*original);
}


Vertex* pMesh::progress(pVert p)
{
	while(p.index > LOD)
	{
		p = p.to;
	}

	return p;
}
