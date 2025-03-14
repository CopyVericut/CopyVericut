#include "CgalProcess.h"

CgalProcess::CgalProcess(BrepToMesh* aBlankShapeMess, BrepToMesh* aCuttingToolMess)
{
	BlankShapeMess = aBlankShapeMess;
	CuttingTollMess = aCuttingToolMess;
}

void CgalProcess::SetBlankMesh(BrepToMesh* aBlankShapeMess)
{
	BlankShapeMess = aBlankShapeMess;
}

void CgalProcess::SetCuttingMesh(BrepToMesh* aCuttingToolMess)
{
	CuttingTollMess = aCuttingToolMess;
}
