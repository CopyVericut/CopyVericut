#include "BrepToMesh.h"
#include<opencascade/TopExp_Explorer.hxx>
#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/TopoDS_Face.hxx>
#include <opencascade/TopoDS.hxx>
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
	TopExp_Explorer explorer;
	TopLoc_Location location;
	for (explorer.Init(shape, TopAbs_FACE); explorer.More(); explorer.Next()) {
		TopoDS_Face face = TopoDS::Face(explorer.Current());
		Handle(Poly_Triangulation) triMesh = BRep_Tool::Triangulation(face, location);

		if (!triMesh.IsNull()) {
			for (int i = 1; i <= triMesh->NbNodes(); ++i) {
				const gp_Pnt& p = triMesh->Node(i);
				points.emplace_back(p);
			}

			for (int i = 1; i <= triMesh->NbTriangles(); ++i) {
				Poly_Triangle tri = triMesh->Triangle(i);
				Standard_Integer n1, n2, n3;
				tri.Get(n1, n2, n3);
				triangles.push_back({ n1 - 1, n2 - 1, n3 - 1 });
			}
		}
	}

}
