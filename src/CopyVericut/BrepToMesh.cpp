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

void BrepToMesh::ConvertCgalMesh()
{
	//SurfaceMesh cgal_mesh;

	TopExp_Explorer exp;
	TopLoc_Location toploc_location;
	// 遍历 OpenCascade Face
	for (exp.Init(shape, TopAbs_FACE); exp.More(); exp.Next()) {
		TopoDS_Face face = TopoDS::Face(exp.Current());

		// 获取 OpenCascade 的 Poly_Triangulation
		Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, toploc_location);

		if (triangulation.IsNull()) {
			continue;
		}

		const int nodes = triangulation->NbNodes();
		const int triangles = triangulation->NbTriangles();

		// 映射顶点索引
		std::vector<SurfaceMesh::Vertex_index> vertex_map(nodes + 1);

		// OpenCascade 顶点 → CGAL 顶点
		for (int i =1; i <= nodes; ++i) {
			const gp_Pnt& p = triangulation->Node(i);
			vertex_map[i] = cgal_mesh.add_vertex(Kernel::Point_3(p.X(), p.Y(), p.Z()));
		}

		// 添加面
		for (int i = 1; i <= triangles; ++i) {
			Poly_Triangle t = triangulation->Triangle(i);
			int n1, n2, n3;
			t.Get(n1, n2, n3);
			cgal_mesh.add_face(vertex_map[n1], vertex_map[n2], vertex_map[n3]);
		}
	}

	if (!CGAL::IO::write_STL("output_filename.stl", cgal_mesh))
	{
		std::cerr << "Failed to write STL file: " << "output_filename.stl" << std::endl;

	}

	std::cout << "STL file written successfully: " << "output_filename.stl" << std::endl;
}

void BrepToMesh::ToMesh()
{
		
	BRepMesh_IncrementalMesh mesh(shape, 0.1);
}

void BrepToMesh::ExportToSTL(string filename)
{
	ToMesh();
	StlAPI_Writer writer;
	writer.Write(shape, filename.c_str());
}


