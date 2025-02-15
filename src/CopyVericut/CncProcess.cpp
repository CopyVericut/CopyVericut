#include "CncProcess.h"
#include <QFileDialog>
#include <RenderWindow.h>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <regex>
#include <QApplication>
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
#include <opencascade/GC_MakeSegment.hxx>
#include <opencascade/BRepBuilderAPI_MakeWire.hxx>
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
	std::regex patternX(R"(X(-?\d*\.\d+|-?\d+))");  // 匹配大写字母X后面的数字
	std::regex patternY(R"(Y(-?\d*\.\d+|-?\d+))");  // 匹配大写字母Y后面的数字
	std::regex patternZ(R"(Z(-?\d*\.\d+|-?\d+))");  // 匹配大写字母Z后面的数字
	std::regex patternI(R"(I(-?\d*\.\d+|-?\d+))");  // 匹配大写字母I后面的数字
	std::regex patternJ(R"(J(-?\d*\.\d+|-?\d+))");  // 匹配大写字母J后面的数字
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
		else if (i == "%" or i=="") { continue; }
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
			/**/
			cncPathData.Gstatus = "G01/G0";
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
			/*设置G状态码*/
			cncPathData.Gstatus = Gstatus;
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
			GetArcInterpolationPoints(i);
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

void CncProcess::DisPlayToolPath(DisplayCore* displayCore)
{
	double point0[3] = { 0.0,0.0,0.0 };
	double point1[3] = { 0.0,0.0,0.0 };
	for (auto cncdata : cncPathDataList)
	{
		if (cncdata.pathType == Line)
		{
			point0[0] = cncdata.startPointX;
			point0[1] = cncdata.startPointY;
			point0[2] = cncdata.startPointZ;
			point1[0] = cncdata.endPointX;
			point1[1] = cncdata.endPointY;
			point1[2] = cncdata.endPointZ;
			if (point0[1]==point1[1] and point0[0]==point1[0] and point0[2]==point1[2])
			{
				continue;
			}
			GC_MakeSegment* aSegment = new GC_MakeSegment(gp_Pnt(point0[0], point0[1], point0[2]), gp_Pnt(point1[0], point1[1], point1[2]));
			TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(aSegment->Value()).Edge();
			Handle(AIS_Shape) ais_line = new AIS_Shape(anEdge);
			auto drawer = ais_line->Attributes();
			auto acolor = Quantity_Color(255.0 / 255.0, 200.0 / 255.0, 135.0 / 255.0, Quantity_TOC_RGB);
			Handle(Prs3d_LineAspect) asp = new Prs3d_LineAspect(acolor, Aspect_TOL_SOLID, 1.0);
			drawer->SetLineAspect(asp);
			ais_line->SetAttributes(drawer);
			displayCore->Context->Display(ais_line, true);
			// 处理事件，确保 UI 在长时间任务过程中仍能响应
			QApplication::processEvents();
		}
		else if (cncdata.pathType == Arc )
		{
			double i = cncdata.I;
			double j = cncdata.J;
			double k = cncdata.endPointZ;
			double x0 = cncdata.startPointX;
			double y0 = cncdata.startPointY;
			double z0 = cncdata.startPointZ;
			double x = cncdata.endPointX;
			double y = cncdata.endPointY;
			double z = cncdata.endPointZ;
			// 圆心坐标
			gp_Pnt circle_center(x0+i, y0+j, k);
			// 计算半径
			double r = std::sqrt(std::pow(i, 2) + std::pow(j, 2));
			// 使用 gp_Pnt 创建圆心坐标
			gp_Pnt Location(circle_center.X(), circle_center.Y(), circle_center.Z());
			// 创建法线方向
			gp_Dir Axis(0,0,1.0);
			if (cncdata.Gstatus=="G02")
			{
				Axis.SetZ(-1.0);
			}
			else if (cncdata.Gstatus == "G03")
			{
				Axis.SetZ(1.0);
			}
			// 定义圆的轴
			gp_Ax2 CircleAxis(Location, Axis);
			// 创建圆
			gp_Circ Circle(CircleAxis, r);
			// 创建圆弧
			/*定义圆弧的两个端点*/
			gp_Pnt P1(x0, y0, z0);
			gp_Pnt P2(x, y, z);
			if (1)
			{
				GC_MakeArcOfCircle ArcofCircle0(Circle, P1, P2, true);
				BRepBuilderAPI_MakeEdge ArcofCircle1(ArcofCircle0.Value());
				TopoDS_Edge arcEdge = ArcofCircle1.Edge();
				Handle(AIS_Shape) ais_curve = new AIS_Shape(arcEdge);
				auto drawer = ais_curve->Attributes();
				auto acolor = Quantity_Color(255.0 / 255.0, 200.0 / 255.0, 135.0 / 255.0, Quantity_TOC_RGB);
				Handle(Prs3d_LineAspect) asp = new Prs3d_LineAspect(acolor, Aspect_TOL_SOLID, 1.0);
				drawer->SetLineAspect(asp);
				ais_curve->SetAttributes(drawer);
				displayCore->Context->Display(ais_curve, true);
			}
			
			// 创建圆弧边
			
			// 处理事件，确保 UI 在长时间任务过程中仍能响应
			QApplication::processEvents();
			
		}
		
	}

}

// Compare this snippet from CncProcess.cpp: