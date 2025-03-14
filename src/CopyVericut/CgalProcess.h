#pragma once
#include <CGAL/Surface_mesh.h>
#include <CGAL/Simple_cartesian.h>
#include "BrepToMesh.h"
typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Surface_mesh<Kernel::Point_3> Mesh;

class CgalProcess
{
public:
	CgalProcess(BrepToMesh* aBlankShapeMess, BrepToMesh* aCuttingTollMess);
	void SetBlankMesh(BrepToMesh*aBlankShapeMess);
	void SetCuttingMesh(BrepToMesh* aCuttingTollMess);
	~CgalProcess() = default;
public:
	BrepToMesh* aBrepToMess;
	BrepToMesh* CuttingTollMess;
	BrepToMesh* BlankShapeMess;
	Mesh cgalMesh;
};

