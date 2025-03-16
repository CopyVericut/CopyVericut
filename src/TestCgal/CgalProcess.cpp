#include "CgalProcess.h"
#include <CGAL/IO/STL.h>
#include <CGAL/draw_polyhedron.h>  // ✅ CGAL Viewer
#define CGAL_USE_BASIC_VIEWER

typedef CGAL::Polyhedron_3<Kernel> Polyhedron_3;

CgalProcess::CgalProcess()
{
	
}

void CgalProcess::ImpoerStl(string filename, arryPoint apoint, arrtriangles atriangles, Mesh amesh)
{
	// 读取两个 STL 文件
	Polyhedron_3 poly_Partition;
	std::vector<CGAL::cpp11::array<double, 3> > points;
	std::vector<CGAL::cpp11::array<int, 3> > triangles;
	CGAL::IO::read_STL(filename, apoint, atriangles);

	cout << "number of points in soup : " << apoint.size() << "\n";
	cout << "number of triangles in soup : " << atriangles.size() << "\n";

	CGAL::Polygon_mesh_processing::orient_polygon_soup(apoint, atriangles);
	if (filename.find("blank.stl"))
	{
		CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(apoint, atriangles, polyBlankMesh);
	}
	else if (filename.find("tool.stl"))
	{
		CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(apoint, atriangles, polyToolMesh);
	}
	
}

void CgalProcess::ExportStl(string filename)
{
	const std::string output_filename = "output.stl";
	if (!CGAL::IO::write_STL(output_filename, result))
	{
		std::cerr << "Failed to write STL file: " << output_filename << std::endl;
	
	}

	std::cout << "STL file written successfully: " << output_filename << std::endl;
	
}
void CgalProcess::SetToolMesh()
{
	ImpoerStl("../CopyVericut/tool.stl",toolPoints,toolrtriangles,polyToolMesh);
}

void CgalProcess::SetBlankMesh()
{
	ImpoerStl("../CopyVericut/blank.stl", blankPoints, blankrtriangles,polyBlankMesh);
}

void CgalProcess::Cut()
{
	// 读取两个 STL 文件
	Mesh  __polyToolMesh, __polyBlankMesh,__result;
	Polyhedron_3 poly_Partition;
	std::vector<CGAL::cpp11::array<double, 3> > points;
	std::vector<CGAL::cpp11::array<int, 3> > triangles;

	CGAL::IO::read_STL("../CopyVericut/blank.stl", points, points);

	cout << "number of points in soup : " << points.size() << "\n";
	cout << "number of triangles in soup : " << triangles.size() << "\n";

	CGAL::Polygon_mesh_processing::orient_polygon_soup(points, triangles);
	CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, triangles, __polyBlankMesh);

	CGAL::IO::read_STL("../CopyVericut/tool.stl", points, points);

	cout << "number of points in soup : " << points.size() << "\n";
	cout << "number of triangles in soup : " << triangles.size() << "\n";

	CGAL::Polygon_mesh_processing::orient_polygon_soup(points, triangles);
	CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, triangles, __polyToolMesh);

	// 执行布尔运算 (差集)
	CGAL::Polygon_mesh_processing::corefine_and_compute_difference(__polyBlankMesh, __polyToolMesh, __result);
	
}

