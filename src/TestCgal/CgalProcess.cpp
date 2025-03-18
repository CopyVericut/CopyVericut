#include "CgalProcess.h"
#include <CGAL/IO/STL.h>
#include <CGAL/draw_polyhedron.h>  
#define CGAL_USE_BASIC_VIEWER

namespace PMP = CGAL::Polygon_mesh_processing;
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
	const std::string output_filename = filename;
	if (!CGAL::IO::write_STL(output_filename, result))
	{
		std::cerr << "Failed to write STL file: " << output_filename << std::endl;
	
	}

	std::cout << "STL file written successfully: " << output_filename << std::endl;
	
}
void CgalProcess::SetToolMesh(Mesh toolMesh)
{
	surfaceToolMesh = toolMesh;
}

void CgalProcess::SetBlankMesh(Mesh blankMesh)
{
	surfaceBlankMesh = blankMesh;
}

void CgalProcess::Cut(string toolStlPath, string blankStlPath, string resultStlPath)
{
	// 读取两个 STL 文件
	Polyhedron_3  __polyToolMesh, __polyBlankMesh, __result;
	Polyhedron_3 poly_Partition;
	std::vector<CGAL::cpp11::array<double, 3> > points;
	std::vector<CGAL::cpp11::array<int, 3> > triangles;
	if (count==0)
	{
		CGAL::IO::read_STL(toolStlPath, __polyToolMesh);
		CGAL::IO::read_STL(blankStlPath, __polyBlankMesh);
		count++;
	}
	else
	{
		CGAL::IO::read_STL(toolStlPath, __polyToolMesh);
		//CGAL::IO::read_STL(resultStlPath, __polyBlankMesh);
		__polyBlankMesh = result;
	}
	

	//cout << "number of points in soup : " << points.size() << "\n";
	//cout << "number of triangles in soup : " << triangles.size() << "\n";

	//CGAL::Polygon_mesh_processing::orient_polygon_soup(points, triangles);
	//CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, triangles, __polyBlankMesh);

	//PMP::stitch_borders(__polyBlankMesh);
	//PMP::remove_isolated_vertices(__polyBlankMesh);


	

	//cout << "number of points in soup : " << points.size() << "\n";
	//cout << "number of triangles in soup : " << triangles.size() << "\n";

	//CGAL::Polygon_mesh_processing::orient_polygon_soup(points, triangles);
	//CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, triangles, __polyToolMesh);

	//PMP::stitch_borders(__polyToolMesh);
	//PMP::remove_isolated_vertices(__polyToolMesh);



	// 执行布尔运算 (差集)
	auto start = std::chrono::high_resolution_clock::now();
	if (CGAL::Polygon_mesh_processing::corefine_and_compute_difference(__polyBlankMesh, __polyToolMesh, __result))
	{
		result = __result;
		//PMP::remove_self_intersections(result);
		PMP::stitch_borders(__result);
		PMP::remove_isolated_vertices(__result);
		//if (!CGAL::IO::write_STL(resultStlPath, __result))
		{
			std::cerr << "Failed to write STL file: " << "output_filename.stl" << std::endl;

		}

		std::cout << "STL file written successfully: " << "output_filename.stl" << std::endl;
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;
	std::cout << "一次布尔运算的时间: " << duration.count() << " ms" << std::endl;
	
	
	
	
}

void CgalProcess::Cut()
{
	
	//CGAL::Polygon_mesh_processing::corefine_and_compute_difference(surfaceBlankMesh, surfaceBlankMesh,result);
	const std::string output_filename = "OUT.STL";
	if (!CGAL::IO::write_STL(output_filename, result))
	{
		std::cerr << "Failed to write STL file: " << output_filename << std::endl;

	}

	std::cout << "STL file written successfully: " << output_filename << std::endl;
}

