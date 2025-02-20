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
#include <opencascade/BRepAdaptor_Surface.hxx>
#include <opencascade/GeomAbs_SurfaceType.hxx>
#include <opencascade/BRepLProp_SLProps.hxx>
#include <opencascade/BRepBuilderAPI_MakeWire.hxx>
#include <opencascade/TopoDS_Wire.hxx>
#include <opencascade/BRepBuilderAPI_MakeFace.hxx>
CncProcess::CncProcess()
{}
void CncProcess::ReadCncFile(QString filePath)
{
	textBrowser->clear();
	/*创建 QFile 对象*/
	QFile file(filePath); 
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {}
	QTextStream in(&file);
	while (!in.atEnd())
	{
		/*读取一行*/
		QString line = in.readLine();  
		cncContentList.push_back(line);
		// 处理事件，确保 UI 在长时间任务过程中仍能响应
		QApplication::processEvents();
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
	std::regex patternK(R"(K(-?\d*\.\d+|-?\d+))");  // 匹配大写字母J后面的数字
	std::smatch match;
	std::string text;
	CncPathData cncPathData;
	string Gstatus{""};
	double currentPointX{ 0.0 }, currentPointY{ 0.0 }, currentPointZ{ 10.0 }, currentI{ 0.0 }, currentJ{0.0};
	for (auto i : cncContentList)
	{
		PrintGCode(i);
		/*确定Gstatus状态*/
		if (i.contains(QString(" G00 ")) or i.contains(QString(" G0 ")) or i.contains(QString(" G01 ")) or i.contains(QString(" G1 "))) { Gstatus = "G01/G0"; }
		else if (i.contains(QString(" G2 ")) or i.contains(QString(" G02 "))) { Gstatus = "G02"; }
		else if (i.contains(QString(" G3 ")) or i.contains(QString(" G03 "))) { Gstatus = "G03"; }
		else if (i == "%" or i=="") { continue; }
		/*写入Gcode*/
		cncPathData.Gcode = i.toStdString();
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
			cncPathData.K = 0.0;
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
			else
			{
				cncPathData.I = 0.0;
			}
			/*J值*/
			if (std::regex_search(text, match, patternJ))
			{
				cncPathData.J = std::stod(match[1]);
			}
			else
			{
				cncPathData.J = 0.0;
			}
			cncPathDataList.push_back(cncPathData);
			/*K值*/
			if (std::regex_search(text, match, patternK))
			{
				cncPathData.K = std::stod(match[1]);
			}
			else
			{
				cncPathData.K = 0.0;
			}
			cncPathDataList.push_back(cncPathData);
		}
		// 处理事件，确保 UI 在长时间任务过程中仍能响应
		QApplication::processEvents();

	}
	qDebug() << "解析完成";
	textBrowser->moveCursor(QTextCursor::Start);//返回到文本框的开始
	return true;
}

void CncProcess::SetTextBrowser(QTextBrowser* textBrowser)
{
	this->textBrowser = textBrowser;
}

gp_Dir CncProcess::GetFaceDirection(const TopoDS_Face& face)
{
	BRepAdaptor_Surface surface(face);

	if (surface.GetType() == GeomAbs_Plane) {
		// 如果是平面，直接获取法向
		gp_Pln plane = surface.Plane();
		return plane.Axis().Direction();
	}
	else {
		// 计算曲面局部点的法向
		Standard_Real u = 0.5, v = 0.5;  // 选取 UV 参数空间中的一个点
		BRepLProp_SLProps props(surface, 1, Precision::Confusion());

		if (!props.IsNormalDefined()) {
			throw std::runtime_error("Cannot determine normal for this face.");
		}

		gp_Dir normal = props.Normal();  // 获取曲面法向
		return normal;
	}
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

void CncProcess::GetArcInterpolationPoints(CncPathData cncdata,double step)
{
	InterpolationPointsList.clear();
	double i = cncdata.I;
	double j = cncdata.J;
	double k = cncdata.K;
	double x0 = cncdata.startPointX;
	double y0 = cncdata.startPointY;
	double z0 = cncdata.startPointZ;
	double x = cncdata.endPointX;
	double y = cncdata.endPointY;
	double z = cncdata.endPointZ;
	BRepBuilderAPI_MakeEdge ArcofCircle1;
	TopoDS_Edge arcEdge;
	/*定义圆弧的两个端点*/
	gp_Pnt P1(x0, y0, z0);
	gp_Pnt P2(x, y, z);
	// 圆心坐标
	gp_Pnt circle_center(x0 + i, y0 + j, z0 + k);
	// 计算半径
	double r = std::sqrt(std::pow(i, 2) + std::pow(j, 2) + std::pow(k, 2));
	// 使用 gp_Pnt 创建圆心坐标
	gp_Pnt Location(circle_center.X(), circle_center.Y(), circle_center.Z());

	if (P1.IsEqual(P2, 0.01))/*整圆*/
	{
		// 创建法线方向
		gp_Dir Axis(0, 0, 1);
		if (cncdata.Gstatus == "G02")
		{
			Axis.SetZ(-1.0);
		}
		else if (cncdata.Gstatus == "G03")
		{
			Axis.SetZ(1.0);
		}
		// 创建一个圆 (圆心, 法向量, 半径)
		gp_Ax2 axis(circle_center, Axis);  // 定义圆的坐标系
		gp_Circ circle(axis, r);  // 创建圆

		// 将圆转换为边 (Edge)
		BRepBuilderAPI_MakeEdge edge(circle);
		TopoDS_Edge circleEdge = edge.Edge();  // 返回一个边对象
		// 创建边
		arcEdge = circleEdge;
	}
	else/*非整圆圆弧*/
	{
		// 创建三条边
		TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(P1, P2).Edge();
		TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(P2, circle_center).Edge();
		TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(circle_center, P1).Edge();
		// 生成 Wire
		BRepBuilderAPI_MakeWire wireMaker(E1, E2, E3);
		TopoDS_Wire wire = wireMaker.Wire();
		// 生成 Face
		TopoDS_Face face = BRepBuilderAPI_MakeFace(wire).Face();
		// 创建法线方向
		gp_Dir Axis = GetFaceDirection(face);
		gp_Dir Axis2 = GetFaceDirection(face);
		//gp_Dir Axis(0, 0, 1);
		if (cncdata.Gstatus == "G02")
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
		GC_MakeArcOfCircle ArcofCircle0(Circle, P1, P2, true);
		 ArcofCircle1.Init(ArcofCircle0.Value());
		 // 创建边
		 arcEdge = ArcofCircle1.Edge();
	}

	
	

	// 获取曲线
	Handle(Geom_Curve) aCurve;
	Standard_Real ufirst, ulast;
	aCurve = BRep_Tool::Curve(arcEdge, ufirst, ulast);
	// 适配几何曲线
	GeomAdaptor_Curve gac(aCurve, ufirst, ulast);
	// 计算等间距参数
	GCPnts_UniformAbscissa ua(gac, step);
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
	DisPlayToolPath(displayCore);
}

void CncProcess::DisPlayToolPath(DisplayCore* displayCore)
{
	double point0[3] = { 0.0,0.0,0.0 };
	double point1[3] = { 0.0,0.0,0.0 };
	for (auto cncdata : cncPathDataList)
	{
		PrintGCodeColor(cncdata.Gcode.c_str());
;		if (cncdata.pathType == Line)
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
			//auto drawer = ais_line->Attributes();
			Quantity_Color acolor = Quantity_Color(0.0 / 255.0, 0.0 / 255.0, 255.0 / 255.0, Quantity_TOC_RGB);
			//Handle(Prs3d_LineAspect) asp = new Prs3d_LineAspect(acolor, Aspect_TOL_SOLID, 1.0);
			//drawer->SetLineAspect(asp);
			//ais_line->Attributes()->SetLineAspect(asp);
			displayCore->Context->Display(ais_line, true);
			ais_line->SetColor(acolor);
			// 处理事件，确保 UI 在长时间任务过程中仍能响应
			QApplication::processEvents();
		}
		else if (cncdata.pathType == Arc )
		{
			double i = cncdata.I;
			double j = cncdata.J;
			double k = cncdata.K;
			double x0 = cncdata.startPointX;
			double y0 = cncdata.startPointY;
			double z0 = cncdata.startPointZ;
			double x = cncdata.endPointX;
			double y = cncdata.endPointY;
			double z = cncdata.endPointZ;
			/*定义圆弧的两个端点*/
			gp_Pnt P1(x0, y0, z0);
			gp_Pnt P2(x, y, z);
			// 圆心坐标
			gp_Pnt circle_center(x0+i, y0+j, z0+k);
			// 计算半径
			double r = std::sqrt(std::pow(i, 2) + std::pow(j, 2)+ std::pow(k, 2));
			// 使用 gp_Pnt 创建圆心坐标
			gp_Pnt Location(circle_center.X(), circle_center.Y(), circle_center.Z());
			if (P1.IsEqual(P2,0.01))/*整圆*/
			{
				// 创建法线方向
				gp_Dir Axis(0, 0, 1);
				if (cncdata.Gstatus == "G02")
				{
					Axis.SetZ(-1.0);
				}
				else if (cncdata.Gstatus == "G03")
				{
					Axis.SetZ(1.0);
				}
				// 创建一个圆 (圆心, 法向量, 半径)
				gp_Ax2 axis(circle_center, Axis);  // 定义圆的坐标系
				gp_Circ circle(axis, r);  // 创建圆

				// 将圆转换为边 (Edge)
				BRepBuilderAPI_MakeEdge edge(circle);
				TopoDS_Edge circleEdge = edge.Edge();  // 返回一个边对象
				Handle(AIS_Shape) ais_curve = new AIS_Shape(circleEdge);
				//auto drawer = ais_curve->Attributes();
				Quantity_Color acolor = Quantity_Color(255.0 / 255.0, 0.0 / 255.0, 0.0 / 255.0, Quantity_TOC_RGB);
				//Handle(Prs3d_LineAspect) asp = new Prs3d_LineAspect(acolor, Aspect_TOL_SOLID, 1.0);
				//drawer->SetLineAspect(asp);
				//ais_curve->Attributes()->SetLineAspect(asp);
				ais_curve->SetColor(acolor);
				displayCore->Context->Display(ais_curve, true);
				continue;
			}
			else/*非整圆圆弧*/
			{
				// 创建三条边
				TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(P1, P2).Edge();
				TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(P2, circle_center).Edge();
				TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(circle_center, P1).Edge();
				// 生成 Wire
				BRepBuilderAPI_MakeWire wireMaker(E1, E2, E3);
				TopoDS_Wire wire = wireMaker.Wire();
				// 生成 Face
				TopoDS_Face face = BRepBuilderAPI_MakeFace(wire).Face();
				// 创建法线方向
				gp_Dir Axis = GetFaceDirection(face);
				gp_Dir Axis2 = GetFaceDirection(face);
				//gp_Dir Axis(0, 0, 1);
				if (cncdata.Gstatus == "G02")
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
				GC_MakeArcOfCircle ArcofCircle0(Circle, P1, P2, true);
				BRepBuilderAPI_MakeEdge ArcofCircle1(ArcofCircle0.Value());
				TopoDS_Edge arcEdge = ArcofCircle1.Edge();
				Handle(AIS_Shape) ais_curve = new AIS_Shape(arcEdge);
				//auto drawer = ais_curve->Attributes();
				Quantity_Color acolor = Quantity_Color(255.0 / 255.0, 0.0 / 255.0, 0.0 / 255.0, Quantity_TOC_RGB);
				//Handle(Prs3d_LineAspect) asp = new Prs3d_LineAspect(acolor, Aspect_TOL_SOLID, 1.0);
				//drawer->SetLineAspect(asp);
				//ais_curve->Attributes()->SetLineAspect(asp);
				ais_curve->SetColor(acolor);
				displayCore->Context->Display(ais_curve, true);
			}
			// 处理事件，确保 UI 在长时间任务过程中仍能响应
			QApplication::processEvents();
			
		}
		
	}

}

void CncProcess::CuttingSimulation(DisplayCore* displayCore)
{
	for (auto i : cncPathDataList)
	{
		if (i.pathType == Line)
		{
			GetLinearInterpolationPoints(i);
		}
		else if (i.pathType == Arc)
		{
			GetArcInterpolationPoints(i);
		}
		for (auto j : InterpolationPointsList)
		{

			Handle(Geom_Point) p = new Geom_CartesianPoint(j);
			Handle(AIS_Point) ais_point = new AIS_Point(p);
			auto drawer = ais_point->Attributes();
			auto acolor = Quantity_Color(65.0 / 255.0, 15.0 / 255.0, 135.0 / 255.0, Quantity_TOC_RGB);
			Handle(Prs3d_PointAspect) asp = new Prs3d_PointAspect(Aspect_TOM_POINT, acolor, 6);
			drawer->SetPointAspect(asp);
			ais_point->SetAttributes(drawer);
			displayCore->Context->Display(ais_point, true);
			// 处理事件，确保 UI 在长时间任务过程中仍能响应
			QApplication::processEvents();
		}
		// 处理事件，确保 UI 在长时间任务过程中仍能响应
		QApplication::processEvents();
	}
}

void CncProcess::PrintGCode(QString Gcoge)
{
	textBrowser->append(Gcoge);
}

void CncProcess::PrintGCodeColor(QString Gcode)
{
	QTextCursor cursor(textBrowser->textCursor());
	cursor.movePosition(QTextCursor::Down);  // 向下移动一行
	textBrowser->setTextCursor(cursor);  // 更新光标位置
	//QTextBlockFormat blockFormat;
	//blockFormat.setBackground(QBrush(Qt::yellow));
	//cursor.mergeBlockFormat(blockFormat);
}

// Compare this snippet from CncProcess.cpp: