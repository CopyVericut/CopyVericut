#include "MillSimulation.h"
#include "CncProcess.h"
#include <QFileDialog>
#include <RenderWindow.h>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <regex>
#include <QApplication>
#include <QCoreApplication>
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
#include <opencascade/BRepPrimAPI_MakeBox.hxx>
#include <opencascade/gp_Trsf.hxx>
#include <opencascade/TopLoc_Location.hxx>
#include <opencascade/BRepPrimAPI_MakeCylinder.hxx>
#include <opencascade/BRepAlgoAPI_BooleanOperation.hxx>
#include <opencascade/BRepAlgoAPI_Cut.hxx>
#include <opencascade/BOPAlgo_Builder.hxx>
#include <opencascade/OSD_Parallel.hxx>
#include <opencascade/BRepTools.hxx>
#include <opencascade/BRepBuilderAPI_Transform.hxx>
#include <opencascade/ShapeUpgrade_UnifySameDomain.hxx>
#include <opencascade/STEPControl_Writer.hxx>
#include <opencascade/IFSelect_ReturnStatus.hxx>
#include <opencascade/STEPControl_Reader.hxx>
#include "MachineControl.h"
#include "BrepToMesh.h"

MillSimulation::MillSimulation()
{
	machineControl = new MachineControl();
	cut.SetRunParallel(true);
	cut.SetFuzzyValue(0.011);
	arguments.Append(BlankShape);
	builder.MakeCompound(compound);
	cut.Build();
	cut.RunParallel();
	// 启用并行计算
	OSD_Parallel::SetUseOcctThreads(true);
}

void MillSimulation::CreateBlankShape(double L, double W, double H)
{
		BlankShape = BRepPrimAPI_MakeBox(L, W, H).Shape();
		gp_Trsf T;
		double locationX = -L / 2;//把键槽移动到合适的位置
		double locationY = -W / 2;//把键槽移动到合适的位置
		T.SetTranslation(gp_Vec(locationX, locationY, 0));
		TopLoc_Location loc = TopLoc_Location(T);
		BlankShape.Location(loc);
		offsetZ = H;
		CreateToolShape();
}

void MillSimulation::CreateToolShape(double length, double diameter)
{
	double radius = diameter/2.0;
	double height = length;
	// 创建圆柱的轴线 (gp_Ax2)
	gp_Pnt origin(0.0, 0.0, 0.0);  // 圆柱的基准点 (原点)
	gp_Dir direction(0.0, 0.0, 1.0);  // 圆柱的方向 (Z轴方向)
	gp_Ax2 axis(origin, direction);  // 创建轴线
	// 使用 BRepPrimAPI_MakeCylinder 创建圆柱
	BRepPrimAPI_MakeCylinder cylinderMaker(axis, radius, height);
	// 获取圆柱对象
	 CuttingToolShape = cylinderMaker.Shape();
	
}

void MillSimulation::CreateToolShape()
{
	double radius = CuttingToolDiameter / 2.0;
	double height = CuttingToolLength;
	// 创建圆柱的轴线 (gp_Ax2)
	gp_Pnt origin(0.0, 0.0, 0.0);  // 圆柱的基准点 (原点)
	gp_Dir direction(0.0, 0.0, 1.0);  // 圆柱的方向 (Z轴方向)
	gp_Ax2 axis(origin, direction);  // 创建轴线
	// 使用 BRepPrimAPI_MakeCylinder 创建圆柱
	BRepPrimAPI_MakeCylinder cylinderMaker(axis, radius, height);
	// 获取圆柱对象
	CuttingToolShape = cylinderMaker.Shape();
	
}

void MillSimulation::SetBlankShape(TopoDS_Shape BlankShape)
{
}

void MillSimulation::SetWorkPieceShape(TopoDS_Shape WorkPieceShape)
{
}

void MillSimulation::SetToolPathShape(TopoDS_Shape ToolPathShape)
{
}

void MillSimulation::SetToolSweepShape(TopoDS_Shape ToolSweepShape)
{
}

void MillSimulation::SetDisPlayCore(DisplayCore* displayCore)
{
	this->displayCore = displayCore;
	machineControl->SetDisPlayCore(displayCore);
}

void MillSimulation::SetCncPathDataList(vector<CncPathData> acncPathDataList)
{
	this->cncPathDataList = acncPathDataList;
}
void MillSimulation::SetTextBrowser(QTextBrowser* textBrowser)
{
	this->textBrowser = textBrowser;
}

void MillSimulation::Cutting(double x,double y,double z)
{
	TopoDS_Shape mergedTool;
	double radius = CuttingToolDiameter / 2.0;
	double height = CuttingToolLength;
	 //创建圆柱的轴线 (gp_Ax2)
	gp_Pnt origin(x, y, z);  // 圆柱的基准点 (原点)
	gp_Dir direction(0.0, 0.0, 1.0);  // 圆柱的方向 (Z轴方向)
	gp_Ax2 axis(origin, direction);  // 创建轴线
	// 使用 BRepPrimAPI_MakeCylinder 创建圆柱
	CuttingToolShape = BRepPrimAPI_MakeCylinder(axis, radius, height).Shape();
	//gp_Trsf transform;
	//transform.SetTranslation(gp_Vec(x, y, z));  // 平移 (10, 20, 30)
	// 对 shape 应用变换
	//BRepBuilderAPI_Transform brepTransform(CuttingToolShape, transform);
	//TopoDS_Shape transformedShape = brepTransform.Shape();
	auto start = std::chrono::high_resolution_clock::now();
	///builder.Add(compound, CuttingToolShape);
	//mergedTool = compound;
	cut= BRepAlgoAPI_Cut(BlankShape, CuttingToolShape);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;
	std::cout << "布尔运算时间: " << duration.count() << " ms" << std::endl;
	
	if (cut.IsDone())
	{
		// 获取减去操作后的结果
		BRepTools::Clean(BlankShape);
		//BlankShape = cut.Shape();
		cut.SimplifyResult();  // 简化结果
		auto start = std::chrono::high_resolution_clock::now();
		BlankAis_shape->SetShape(cut.Shape());
		displayCore->Context->Redisplay(BlankAis_shape, true, false);
		displayCore->Context->UpdateCurrentViewer();
		// 记录结束时间
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> duration = end - start;
		std::cout << "渲染更新时间: " << duration.count() << " ms" << std::endl;
	}
	
}

void MillSimulation::RefreshBlankShape()
{
	displayCore->Context->Redisplay(BlankAis_shape, true, false);
	displayCore->Context->UpdateCurrentViewer();
}

void MillSimulation::DisPlayBlankShape()
{
	
	Quantity_Color color(0.5, 0.6, 0.7, Quantity_TOC_RGB);
	if (!BlankShape.IsNull())
	{
		displayCore->DisplayShape(BlankShape, color, 1, "blank");
		BlankAis_shape = displayCore->ShapeManeger["blank"]->AisShape;
	}
	

}

void MillSimulation::ResetBlankShape()
{
	if (!BlankAis_shape.IsNull())
	{
		BlankAis_shape->SetShape(BlankShape);
		BlankAis_shape->Redisplay(true);
		displayCore->Context ->UpdateCurrentViewer();
	}
}

void MillSimulation::PathSimulation()
{
	DisPlayToolPaths();
}

void MillSimulation::CuttingSimulation()
{
	vector<gp_Pnt> InterpolationPointsList;

	/*根据参数生成刀具*/
	CreateToolShape();
	for (auto i : cncPathDataList)
	{
		if (i.pathType == Line)
		{
			InterpolationPointsList=CncProcess().GetLinearInterpolationPoints(i,0.1);
		}
		else if (i.pathType == Arc)
		{
			InterpolationPointsList=CncProcess().GetArcInterpolationPoints(i,0.1);
		}
		/*显示此段路径*/
		DisPlayToolPath(i);
		for (auto j : InterpolationPointsList)
		{
			/*主轴运动*/
			machineControl->MachineSpindleMove(j.X(),j.Y(),j.Z()+offsetZ);
			/*刀具切削*/
			if (i.Gstatus!="G0")
			{	
				
				Cutting(j.X(), j.Y(), j.Z() + offsetZ);
				
				
			}
			// 处理事件，确保 UI 在长时间任务过程中仍能响应
			QApplication::processEvents();
		}
		// 处理事件，确保 UI 在长时间任务过程中仍能响应
		QApplication::processEvents();
	}
}

void MillSimulation::PerCuttingProcess()
{
	offsetZ = 0;
	IFSelect_ReturnStatus status;
	BOPAlgo_Builder builder;
	CgalProcess acgalProcess;
	BrepToMesh* toolBrepToMesh;
	BrepToMesh* blankBrepToMesh;
	/*获取当前exe运行的路径*/
	QString exeDir = QCoreApplication::applicationDirPath();
	string toolStlPath = exeDir.toStdString()+"/"+"tool.STL";
	string blankStlPath = exeDir.toStdString() + "/"+"blank.STL";
	string reulst = exeDir.toStdString() + "/"+"reulst.STL";
	cout<<toolStlPath << blankStlPath << reulst<<endl;
	/*导出毛坯*/
	blankBrepToMesh = new BrepToMesh(BlankShape);
	blankBrepToMesh->ExportToSTL(blankStlPath);
	/*根据参数生成刀具*/
	CreateToolShape();
	for (auto i : cncPathDataList)
	{
		if (i.pathType == Line)
		{
			InterpolationPointsList = CncProcess().GetLinearInterpolationPoints(i, 0.1);
		}
		else if (i.pathType == Arc)
		{
			InterpolationPointsList = CncProcess().GetArcInterpolationPoints(i, 0.1);
		}
		/*显示此段路径*/
		DisPlayToolPath(i);
		if (i.Gstatus != "G0")
		{
			auto start = std::chrono::high_resolution_clock::now();
			double radius = CuttingToolDiameter / 2.0;
			double height = CuttingToolLength;
			//创建圆柱的轴线 (gp_Ax2)
			gp_Pnt origin(i.endPointX, i.endPointX, i.endPointZ + offsetZ);  // 圆柱的基准点 (原点)
			gp_Dir direction(0.0, 0.0, 1.0);  // 圆柱的方向 (Z轴方向)
			gp_Ax2 axis(origin, direction);  // 创建轴线
			//BRepPrimAPI_MakeCylinder cylinderMaker(axis, radius, height);
			if (true)
			{
				//CuttingToolShape = BRepPrimAPI_MakeCylinder(axis, radius, height).Shape();
				//CuttingToolShape = cylinderMaker.Shape();
				gp_Trsf transform;
				transform.SetTranslation(gp_Vec(i.endPointX, i.endPointX, i.endPointZ));  // 平移 (10, 20, 30)
				// 对 shape 应用变换
				BRepBuilderAPI_Transform brepTransform(CuttingToolShape, transform);
				TopoDS_Shape transformedShape = brepTransform.Shape();
				toolBrepToMesh = new BrepToMesh(transformedShape);
				toolBrepToMesh->ExportToSTL(toolStlPath);
				acgalProcess.Cut(toolStlPath, blankStlPath);
				acgalProcess.ExportStl("reulst.stl");
				BRepTools::Clean(BlankShape, true);
				//BlankShape = cut.Shape();
				auto end = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double, std::milli> duration = end - start;
				std::cout << "写入一次brep的时间: " << duration.count() << " ms" << std::endl;
			}
			
		}
		// 处理事件，确保 UI 在长时间任务过程中仍能响应
		QApplication::processEvents();
		cout << i.Gcode<<endl;
	}
	
}

void MillSimulation::PrintGCode(QString Gcoge)
{
	QTextCursor cursor(textBrowser->textCursor());
	cursor.movePosition(QTextCursor::Down);  // 向下移动一行
	textBrowser->setTextCursor(cursor);  // 更新光标位置
	//QTextBlockFormat blockFormat;
	//blockFormat.setBackground(QBrush(Qt::yellow));
	//cursor.mergeBlockFormat(blockFormat);
}

void MillSimulation::DisPlayToolPaths()
{
	double point0[3] = { 0.0,0.0,0.0 };
	double point1[3] = { 0.0,0.0,0.0 };
	for (auto cncdata : cncPathDataList)
	{
		//PrintGCodeColor(cncdata.Gcode.c_str());
		if (cncdata.pathType == Line)
		{
			point0[0] = cncdata.startPointX;
			point0[1] = cncdata.startPointY;
			point0[2] = cncdata.startPointZ;
			point1[0] = cncdata.endPointX;
			point1[1] = cncdata.endPointY;
			point1[2] = cncdata.endPointZ;
			if (point0[1] == point1[1] and point0[0] == point1[0] and point0[2] == point1[2])
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
		else if (cncdata.pathType == Arc)
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
				gp_Dir Axis = CncProcess().GetFaceDirection(face);
				gp_Dir Axis2 = CncProcess().GetFaceDirection(face);
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

void MillSimulation::DisPlayToolPath(CncPathData cncdata)
{
	double point0[3] = { 0.0,0.0,0.0 };
	double point1[3] = { 0.0,0.0,0.0 };
	if (cncdata.pathType == Line)
	{
		point0[0] = cncdata.startPointX;
		point0[1] = cncdata.startPointY;
		point0[2] = cncdata.startPointZ;
		point1[0] = cncdata.endPointX;
		point1[1] = cncdata.endPointY;
		point1[2] = cncdata.endPointZ;
		if (point0[1] == point1[1] and point0[0] == point1[0] and point0[2] == point1[2])
		{

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

	}
	else if (cncdata.pathType == Arc)
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
			Handle(AIS_Shape) ais_curve = new AIS_Shape(circleEdge);
			//auto drawer = ais_curve->Attributes();
			Quantity_Color acolor = Quantity_Color(255.0 / 255.0, 0.0 / 255.0, 0.0 / 255.0, Quantity_TOC_RGB);
			//Handle(Prs3d_LineAspect) asp = new Prs3d_LineAspect(acolor, Aspect_TOL_SOLID, 1.0);
			//drawer->SetLineAspect(asp);
			//ais_curve->Attributes()->SetLineAspect(asp);
			ais_curve->SetColor(acolor);
			displayCore->Context->Display(ais_curve, true);

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
			gp_Dir Axis = CncProcess().GetFaceDirection(face);
			gp_Dir Axis2 = CncProcess().GetFaceDirection(face);
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
