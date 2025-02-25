#include "MachineControl.h"

MachineControl::MachineControl()
{

}

void MachineControl::SetMachineWorkTable(Handle(AIS_Shape) MachineWorkTable)
{
	machineWorkTableAis_shape = MachineWorkTable;
	machineWorkTableeShape = MachineWorkTable->Shape();
}

void MachineControl::SetMachineSpindle(Handle(AIS_Shape) MachineSpindle)
{
	machineSpindleAis_shape = MachineSpindle;
	machineSpindleShape = MachineSpindle->Shape();
}

void MachineControl::MachineSpindleMove(double x,double y,double z)
{
	gp_Trsf T;
	T.SetTranslation(gp_Vec(x, y, z));
	TopLoc_Location loc = TopLoc_Location(T);
	machineSpindleShape.Location(loc);
	machineSpindleAis_shape->SetShape(machineSpindleShape);
	//qDebug() >> x >> y >> z;

}

