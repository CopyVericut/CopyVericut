#pragma once
#include <opencascade/TopoDS_Shape.hxx>
#include <QTextBrowser>
#include <DisplayCore.h>
#include "CncProcess.h"
using namespace TubeNext;
class MillSimulation
{
public:
	MillSimulation();
	~MillSimulation();
public:
	void SetBlankShape(TopoDS_Shape BlankShape);
	void SetWorkPieceShape(TopoDS_Shape WorkPieceShape);
	void SetToolPathShape(TopoDS_Shape ToolPathShape);
	void SetToolSweepShape(TopoDS_Shape ToolSweepShape);
	void PathSimulation();
	void CuttingSimulation();
	void PrintGCode(QString Gcoge);
	void DisPlayToolPath();
	void SetDisPlayCore(DisplayCore* displayCore);
	void SetCncPathDataList(vector<CncPathData> acncPathDataList);
	void SetTextBrowser(QTextBrowser* textBrowser);
public:
	TopoDS_Shape BlankShape;
	TopoDS_Shape WorkPieceShape;
	TopoDS_Shape ToolPathShape;
	TopoDS_Shape ToolSweepShape;
public:
	DisplayCore* displayCore;
	vector<CncPathData> cncPathDataList;
	vector<gp_Pnt> InterpolationPointsList;//插补点
	QTextBrowser* textBrowser;//显示G代码的文本框
};

