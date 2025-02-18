#pragma once
#include <QString>
#include <vector>
#include <opencascade/gp_Pnt.hxx>
#include <opencascade/TopoDS_Face.hxx>
#include <DisplayCore.h>
using namespace std;
using namespace TubeNext;
enum PathType
{
	Line,
	Arc,
	Circle
};
struct CncPathData
{
	PathType pathType;
	double startPointX;//起点X坐标
	double startPointY;//起点Y坐标
	double startPointZ;//起点Z坐标
	double endPointX;//终点X坐标
	double endPointY;//终点Y坐标
	double endPointZ;//终点Z坐标
	double I;//圆弧圆心X坐标
	double J;//圆弧圆心Y坐标
	double K;//圆弧圆心Y坐标
	double R;//圆弧半径
	string Gstatus;//状态码
	string Gcode;//G代码
};
class CncProcess
{
public:
	CncProcess();
	~CncProcess()=default;
public:
	void ReadCncFile(QString filePath);
	bool parseCNC();
	void GetLinearInterpolationPoints(CncPathData cncPathData,double step=0.1);
	void GetArcInterpolationPoints(CncPathData cncPathData,double step=0.1);
	void PathSimulation(DisplayCore *displayCore);
	void DisPlayToolPath(DisplayCore* displayCore);
	void CuttingSimulation(DisplayCore* displayCore);
	gp_Dir GetFaceDirection(const TopoDS_Face& face);

public:
	vector<QString> cncContentList;
	vector<CncPathData> cncPathDataList;
	vector<gp_Pnt> InterpolationPointsList;//插补点
};

