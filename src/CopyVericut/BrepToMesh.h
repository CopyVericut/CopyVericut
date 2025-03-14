#pragma once
#include <opencascade/gp_Pnt.hxx>
#include <opencascade/gp_Vec.hxx>
#include <opencascade/BRepBuilderAPI_MakeEdge.hxx>
#include <opencascade/BRep_Tool.hxx>
#include <opencascade/GProp_GProps.hxx>
#include <opencascade/BRepGProp.hxx>
#include <opencascade/TopExp.hxx>
#include <opencascade/TopoDS_Edge.hxx>
#include <opencascade/TopoDS_Shape.hxx>
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
#include <opencascade/BRepMesh_IncrementalMesh.hxx>

class BrepToMesh
{
public:
	BrepToMesh(TopoDS_Shape ashape);
	~BrepToMesh() = default;
	void SetBrepShape(TopoDS_Shape ashape);
	void ToMesh();
public:
	Standard_Real linearDeflection = 0.01;
	Standard_Real angularDeflection = 0.5;
	BRepMesh_IncrementalMesh mesher;
	TopoDS_Shape shape;
	std::vector<gp_Pnt> points;
	std::vector<std::array<int, 3>> triangles;
};

