#pragma once
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polygon_mesh_processing.h>
#include <CGAL/IO/STL.h>
#include <CGAL/draw_polyhedron.h>
#include <CGAL/Surface_mesh.h>
using namespace std;
typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron_3;
typedef std::vector<CGAL::cpp11::array<double, 3> >arryPoint;
typedef std::vector<CGAL::cpp11::array<int, 3> > arrtriangles;
typedef CGAL::Surface_mesh<Kernel::Point_3> Mesh;

class CgalProcess
{
public:
	CgalProcess();
	~CgalProcess() = default;
public:
	void ImpoerStl(string filename,arryPoint apoint, arrtriangles triangles, Mesh amesh);
	void ExportStl(string filename);
	void SetToolMesh();
	void SetBlankMesh();
	void Cut();
public:
	Mesh  polyToolMesh, polyBlankMesh, result;
	std::vector<CGAL::cpp11::array<double, 3> > toolPoints,blankPoints;
	std::vector<CGAL::cpp11::array<int, 3> > toolrtriangles, blankrtriangles;

	
};

