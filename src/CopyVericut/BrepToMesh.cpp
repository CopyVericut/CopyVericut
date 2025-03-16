#include "BrepToMesh.h"
#include<opencascade/TopExp_Explorer.hxx>
#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/TopoDS_Face.hxx>
#include <opencascade/TopoDS.hxx>
#include <opencascade/StlAPI_Writer.hxx>
#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/BRep_Tool.hxx>
#include <opencascade/BRepMesh_IncrementalMesh.hxx>
#include <opencascade/Poly_Triangulation.hxx>
#include <opencascade/TopExp_Explorer.hxx>


using namespace std;

BrepToMesh::BrepToMesh(TopoDS_Shape ashape)
{
	shape = ashape;
}

void BrepToMesh::SetBrepShape(TopoDS_Shape ashape)
{
	shape = ashape;
}

void BrepToMesh::ToMesh()
{
		
	BRepMesh_IncrementalMesh mesh(shape, 1);
}

void BrepToMesh::ExportToSTL(string filename)
{
	ToMesh();
	StlAPI_Writer writer;
	writer.Write(shape, filename.c_str());
}


