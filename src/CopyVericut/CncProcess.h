#pragma once
#include <QString>
#include <vector>
#include <opencascade/gp_Pnt.hxx>
using namespace std;

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
	double R;//圆弧半径
};
class CncProcess
{
public:
	CncProcess();
	~CncProcess()=default;
public:
	void ReadCncFile(QString filePath);
	bool parseCNC();
	void GetLinearInterpolationPoints(CncPathData cncPathData,double step=0.01);
	void GetArcInterpolationPoints(CncPathData cncPathData,string Direction,double step=0.01);
	void PathSimulation();
public:
	vector<QString> cncContentList;
	vector<CncPathData> cncPathDataList;
	vector<gp_Pnt> InterpolationPointsList;//插补点
};

