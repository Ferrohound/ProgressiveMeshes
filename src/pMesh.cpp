#include <vector>

#include "pMesh.cpp"


pMesh::pMesh(Mesh* m)
{
	//ideally make a copy of mesh m and store it inside of progressive
	original = m;
	progressive = new Mesh(m);
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
	while ( progressive.NumVerts > 0 )
	{
		Vertex *cheapest = progressive.CheapestEdge();
		tmp.push_back(cheapest);
		progressive.eCol(cheapest, );
	}
	//reverse the order of pVert and ID the vertices accordingly
	for( int i = tmp.size()-1 ; i > -1 ; i -- )
	{
		tmp[i].id = pVert.size();
		pVert.push_back(tmp[i]);
	}
	
}


//for split and collapse
//this method doesn't use the pVert array 
void pMesh::Update( int n )
{
	if( n < 0 || n > original.NumVerts() || n == progressive.NumVerts())
		return;
	
	//collapse some edges
	if ( n < progressive.NumVerts() )
	{
		while ( progressive.NumVerts() != n )
		{
			Vertex *cheapest = CheapestEdge();
			//tmp.push_back(cheapest);
			Collapse(cheapest);
		}
	}
	
	else
	{
		while ( progressive.NumVerts() != n )
		{
			Vertex *cheapest = CheapestEdge();
			//tmp.push_back(cheapest);
			eSplit();	
		}
	}
}

//this methd does used the preserved info from Initialize()
void pMesh::Update2 ( int n )
{
	
}

void pMesh::Reset()
{
	delete progressive;
	progressive = new Mesh(original);
}


//draw the triangles
void pMesh::Draw()
{
	progressive->Draw();
}
