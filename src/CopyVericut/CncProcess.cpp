#include "CncProcess.h"
#include <QFileDialog>
#include <RenderWindow.h>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <regex>
#include <opencascade/gp_Pnt.hxx>
#include <opencascade/gp_Vec.hxx>
#include <opencascade/BRepBuilderAPI_MakeEdge.hxx>
#include <opencascade/BRep_Tool.hxx>
#include <opencascade/GProp_GProps.hxx>
#include <opencascade/BRepGProp.hxx>
#include <opencascade/TopExp.hxx>
#include <opencascade/TopoDS_Edge.hxx>
#include <opencascade/TopoDS_Vertex.hxx>
#include <opencascade/gp_Ax2.hxx>
#include <opencascade/gp_Circ.hxx>
#include <opencascade/GC_MakeArcOfCircle.hxx>
#include <opencascade/GeomAdaptor_Curve.hxx>
#include <opencascade/GCPnts_UniformAbscissa.hxx>
#include <opencascade/Geom_CartesianPoint.hxx>
CncProcess::CncProcess()
{}
void CncProcess::ReadCncFile(QString filePath)
{
	/*创建 QFile 对象*/
	QFile file(filePath); 
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {}
	QTextStream in(&file);
	while (!in.atEnd())
	{
		/*读取一行*/
		QString line = in.readLine();  
		cncContentList.push_back(line);
	}
	file.close();  // 关闭文件

}
bool CncProcess::parseCNC()//解析CNC文件
{
	std::regex patternX(R"([X]([\d]+(?:\.\d+)?))");  // 匹配大写字母X后面的数字
	std::regex patternY(R"([Y]([\d]+(?:\.\d+)?))");  // 匹配大写字母Y后面的数字
	std::regex patternZ(R"([Z]([\d]+(?:\.\d+)?))");  // 匹配大写字母Z后面的数字
	std::regex patternI(R"([I]([\d]+(?:\.\d+)?))");  // 匹配大写字母I后面的数字
	std::regex patternJ(R"([J]([\d]+(?:\.\d+)?))");  // 匹配大写字母J后面的数字
	std::smatch match;
	std::string text;
	CncPathData cncPathData;
	string Gstatus{""};
	double currentPointX{ 0.0 }, currentPointY{ 0.0 }, currentPointZ{ 100.0 }, currentI{ 0.0 }, currentJ{0.0};
	for (auto i : cncContentList)
	{
		/*确定Gstatus状态*/
		if (i.contains("G0") or i.contains("G01") or i.contains("G1")) { Gstatus = "G01/G0"; }
		else if (i.contains("G2") or i.contains("G02")) { Gstatus = "G02"; }
		else if (i.contains("G3") or i.contains("G03")) { Gstatus = "G03"; }
		/*判断Gstatus状态*/
		if (Gstatus=="G01/G0")
		{
			/*设置轨迹几何形状*/
			cncPathData.pathType = Line;
			/*设置起点坐标*/
			cncPathData.startPointX = currentPointX;
			cncPathData.startPointY = currentPointY;
			cncPathData.startPointZ = currentPointZ;
			/*设置终点坐标*/
			text = i.toStdString();
			if (std::regex_search(text, match, patternX))
			{
				cncPathData.endPointX = std::stod(match[1]);
				currentPointX = cncPathData.endPointX;
			}
			else
			{
				cncPathData.endPointX = currentPointX;
			}
			if (std::regex_search(text, match, patternY))
			{
				cncPathData.endPointY = std::stod(match[1]);
				currentPointY = cncPathData.endPointY;
			}
			else
			{
				cncPathData.endPointY = currentPointY;
			}
			if (std::regex_search(text, match, patternZ))
			{
				cncPathData.endPointZ = std::stod(match[1]);
				currentPointZ = cncPathData.endPointZ;
			}
			else
			{
				cncPathData.endPointZ = currentPointZ;
			}
			cncPathDataList.push_back(cncPathData);
		}
		else if (Gstatus=="G02" or Gstatus=="G03")
		{
			/*设置轨迹几何形状*/
			cncPathData.pathType = Arc;
			/*设置起点坐标*/
			cncPathData.startPointX = currentPointX;
			cncPathData.startPointY = currentPointY;
			cncPathData.startPointZ = currentPointZ;
			cncPathData.I = 0.0;
			cncPathData.J = 0.0;
			/*设置终点坐标*/
			text = i.toStdString();
			/*终点X坐标值*/
			if (std::regex_search(text, match, patternX))
			{
				cncPathData.endPointX = std::stod(match[1]);
				currentPointX = cncPathData.endPointX;
			}
			else
			{
				cncPathData.endPointX = currentPointX;
			}
			/*终点Y坐标值*/
			if (std::regex_search(text, match, patternY))
			{
				cncPathData.endPointY = std::stod(match[1]);
				currentPointY = cncPathData.endPointY;
			}
			else
			{
				cncPathData.endPointY = currentPointY;
			}
			/*终点Z坐标值*/
			if (std::regex_search(text, match, patternZ))
			{
				cncPathData.endPointZ = std::stod(match[1]);
				currentPointZ = cncPathData.endPointZ;
			}
			else
			{
				cncPathData.endPointZ = currentPointZ;
			}
			cncPathDataList.push_back(cncPathData);
			/*I值*/
			if (std::regex_search(text, match, patternI))
			{
				cncPathData.I = std::stod(match[1]);
			}
			/*J值*/
			if (std::regex_search(text, match, patternJ))
			{
				cncPathData.J = std::stod(match[1]);
			}
			

			cncPathDataList.push_back(cncPathData);

		}
	
	}
	qDebug() << "解析完成";
	return true;
}

void CncProcess::GetLinearInterpolationPoints(CncPathData cncPathData, double step)
{
	InterpolationPointsList.clear();
	gp_Pnt aPnt1 = gp_Pnt(cncPathData.startPointX, cncPathData.startPointY, cncPathData.startPointZ);//起点
	gp_Pnt aPnt2 = gp_Pnt(cncPathData.endPointX, cncPathData.endPointY, cncPathData.endPointZ);//终点
	TopoDS_Edge aEdge1;
	if (!aPnt1.IsEqual(aPnt2,0.01))
	{
		 aEdge1 = BRepBuilderAPI_MakeEdge(aPnt1, aPnt2).Edge();
	}
	
	// 计算线段长度
	GProp_GProps system;
	BRepGProp::LinearProperties(aEdge1, system);
	double edge_length = system.Mass();  // 线段长度

	// 计算离散点数，保证是整数
	int num_points = std::max(1, static_cast<int>(std::round(edge_length / step)));

	// 获取起点和终点坐标
	if (!aEdge1.IsNull())
	{
		TopoDS_Vertex mVer1 = TopExp::FirstVertex(aEdge1, true);
		TopoDS_Vertex mVer2 = TopExp::LastVertex(aEdge1, true);
		gp_Pnt P1 = BRep_Tool::Pnt(mVer1);
		gp_Pnt P2 = BRep_Tool::Pnt(mVer2);
		// 计算方向向量
		gp_Vec mVec(P1, P2);
		if (mVec.Magnitude() < Precision::Confusion())
		{
			std::cerr << "Error: Points are too close or identical!" << std::endl;
		}
		mVec.Normalize();

		// 计算离散点
		for (int i = 0; i < num_points; ++i)
		{
			gp_Pnt newPoint = P1.Translated(mVec.Scaled(step * i));
			InterpolationPointsList.push_back(newPoint);
		}
	}
	

	
}

void CncProcess::GetArcInterpolationPoints(CncPathData cncPathData,double step)
{
	InterpolationPointsList.clear();
	gp_Pnt aPnt1 = gp_Pnt(cncPathData.startPointX, cncPathData.startPointY, cncPathData.startPointZ);//起点
	gp_Pnt aPnt2 = gp_Pnt(cncPathData.endPointX, cncPathData.endPointY, cncPathData.endPointZ);//终点
	// 计算圆心
	gp_Pnt circle_center(aPnt1.X() + cncPathData.I, aPnt1.Y() +cncPathData.J, aPnt1.Z());
	// 计算半径
	double radius = std::sqrt((circle_center.X() - aPnt1.X()) * (circle_center.X() - aPnt1.X()) +(circle_center.Y() - aPnt1.Y()) * (circle_center.Y() - aPnt1.Y()));
	// 确定旋转方向 (G02 顺时针, G03 逆时针)
	gp_Dir Axis(0, 0, 0);
	if (aPnt1.X()>aPnt2.X() or aPnt1.Y()>aPnt2.Y())
	{
		Axis.SetZ(1);
	}
	else if (aPnt1.X()< aPnt2.X() or aPnt1.Y() < aPnt2.Y())
	{
		Axis.SetZ(-1);
	}
	// 确定旋转方向 (G02 顺时针, G03 逆时针)
	gp_Ax2 CircleAxis(circle_center, Axis);
	gp_Circ Circle(CircleAxis, radius);

	// 创建圆弧
	GC_MakeArcOfCircle ArcofCircle(Circle, aPnt1, aPnt2, true);
	if (!ArcofCircle.IsDone()) 
	{
		std::cerr << "Error: Failed to create arc!" << std::endl;
		
	}

	// 创建边
	TopoDS_Edge arcEdge = BRepBuilderAPI_MakeEdge(ArcofCircle.Value()).Edge();

	// 获取曲线
	Handle(Geom_Curve) aCurve;
	Standard_Real ufirst, ulast;
	aCurve = BRep_Tool::Curve(arcEdge, ufirst, ulast);

	// 适配几何曲线
	GeomAdaptor_Curve gac(aCurve, ufirst, ulast);

	// 计算等间距参数
	GCPnts_UniformAbscissa ua(gac, step);
	if (!ua.IsDone()) 
	{
		std::cerr << "Error: Failed to compute arc discretization!" << std::endl;
	}

	// 获取离散点
	int num_points = ua.NbPoints();
	for (int i = 1; i <= num_points; ++i) {
		gp_Pnt p;
		aCurve->D0(ua.Parameter(i), p);
		InterpolationPointsList.push_back(p);
	}

	
}

void CncProcess::PathSimulation(DisplayCore* displayCore)
{
	for (auto i:cncPathDataList)
	{
		if (i.pathType==Line)
		{
			GetLinearInterpolationPoints(i);
		}
		else if (i.pathType==Arc)
		{
			//GetArcInterpolationPoints(i);
		}
		for (auto j: InterpolationPointsList)
		{

			Handle(Geom_Point) p = new Geom_CartesianPoint(j);
			Handle(AIS_Point) ais_point= new AIS_Point(p);
			auto drawer = ais_point->Attributes();
			auto acolor = Quantity_Color(255.0 / 255.0, 200.0 / 255.0, 135.0 / 255.0, Quantity_TOC_RGB);
			Handle(Prs3d_PointAspect) asp = new Prs3d_PointAspect(Aspect_TOM_POINT, acolor, 6);
			drawer->SetPointAspect(asp);
			ais_point->SetAttributes(drawer);
			displayCore->Context->Display(ais_point, true);
		}
	}
}

// Compare this snippet from CncProcess.cpp: