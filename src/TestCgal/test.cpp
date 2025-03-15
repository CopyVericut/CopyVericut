#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/IO/STL.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Surface_mesh<Kernel::Point_3> Mesh;

int test(int argc, char* argv[]) {
	// 创建一个简单的网格
	Mesh mesh;
	auto v0 = mesh.add_vertex(Kernel::Point_3(0, 0, 0));
	auto v1 = mesh.add_vertex(Kernel::Point_3(1, 0, 0));
	auto v2 = mesh.add_vertex(Kernel::Point_3(0, 1, 0));
	mesh.add_face(v0, v1, v2);

	// 导出为 STL 文件
	const std::string output_filename = "output.stl";
	if (!CGAL::IO::write_STL(output_filename, mesh)) {
		std::cerr << "Failed to write STL file: " << output_filename << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "STL file written successfully: " << output_filename << std::endl;
	return EXIT_SUCCESS;
}