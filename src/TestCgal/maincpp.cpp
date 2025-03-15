#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polygon_mesh_processing.h>
#include <CGAL/IO/STL.h>
#include <CGAL/draw_polyhedron.h>
#include "CgalProcess.h"
using namespace std;
typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron_3;

int main()
{
	
	CgalProcess cgalProcess;
	//cgalProcess.SetBlankMesh();
	//cgalProcess.SetToolMesh();
	auto start = std::chrono::high_resolution_clock::now();
	cgalProcess.Cut();
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;
	std::cout << "布尔运算时间: " << duration.count() << " ms" << std::endl;
	cgalProcess.ExportStl("115.stl");
	return EXIT_SUCCESS;
}
