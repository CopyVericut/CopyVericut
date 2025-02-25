#pragma once
#include <opencascade/AIS_Shape.hxx>
class MachineControl
{
public:
	MachineControl();
	~MachineControl() = default;
public:
	void SetMachineWorkTable(Handle(AIS_Shape) MachineWorkTable);
	void SetMachineSpindle(Handle(AIS_Shape) MachineSpindle);
	void MachineSpindleMove(double x, double y, double z);

public:
	Handle(AIS_Shape) machineWorkTableAis_shape;
	Handle(AIS_Shape) machineSpindleAis_shape;
	TopoDS_Shape machineWorkTableeShape;//TopoDS_Shape
	TopoDS_Shape machineSpindleShape;//TopoDS_Shape
};

