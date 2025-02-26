#pragma once
#include <opencascade/TopoDS_Shape.hxx>
#include <QTextBrowser>
#include <DisplayCore.h>
#include "CncProcess.h"
#include <opencascade/Standard_Handle.hxx>
#include <opencascade/AIS_Shape.hxx>
#include "MachineControl.h"
using namespace TubeNext;
class MillSimulation
{
public:
	MillSimulation();
	~MillSimulation();
public:
	void CreateBlankShape(double L,double W,double H);
	void CreateToolShape(double length,double diameter);
	void CreateToolShape();
	void SetBlankShape(TopoDS_Shape BlankShape);
	void SetWorkPieceShape(TopoDS_Shape WorkPieceShape);
	void SetToolPathShape(TopoDS_Shape ToolPathShape);
	void SetToolSweepShape(TopoDS_Shape ToolSweepShape);
	void PathSimulation();
	void CuttingSimulation();
	void PrintGCode(QString Gcoge);
	void DisPlayToolPaths();//显示全部路径
	void DisPlayToolPath(CncPathData cncPathData);//显示单段路径
	void SetDisPlayCore(DisplayCore* displayCore);
	void SetCncPathDataList(vector<CncPathData> acncPathDataList);
	void SetTextBrowser(QTextBrowser* textBrowser);
	void RefreshBlankShape();
	void DisPlayBlankShape();
	void ResetBlankShape();

public:
	TopoDS_Shape CuttingToolShape;
	TopoDS_Shape BlankShape;
	TopoDS_Shape WorkPieceShape;
	TopoDS_Shape ToolPathShape;
	TopoDS_Shape ToolSweepShape;
	Handle(AIS_Shape) CuttingToolAis_shape { nullptr };
	Handle(AIS_Shape) BlankAis_shape {nullptr};
	Handle(AIS_Shape) WorkPieceAis_shape { nullptr };
	Handle(AIS_Shape) ToolPathAis_shape { nullptr };
public:
	DisplayCore* displayCore;
	vector<CncPathData> cncPathDataList;
	vector<gp_Pnt> InterpolationPointsList;//插补点
	QTextBrowser* textBrowser;//显示G代码的文本框
	MachineControl* machineControl;//机床控制
public://切削参数
	double CuttingSpeed = 0.0;//切削速度
	double CuttingFeed = 0.0;//切削进给
	double CuttingDepth = 0.0;//切削深度
	double CuttingToolDiameter = 0.0;//刀具直径
	double CuttingToolLength=0;
};

