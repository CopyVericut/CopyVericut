#include "MachineControl.h"
#include "DisplayCore.h"
using namespace TubeNext;
MachineControl::MachineControl()
{
	
}

void MachineControl::SetDisPlayCore(DisplayCore* adisplayCore)
{
	displayCore = adisplayCore;
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
	if (!machineSpindleShape.IsNull())
	{
		gp_Trsf T;
		T.SetTranslation(gp_Vec(x, y, z));
		TopLoc_Location loc = TopLoc_Location(T);
		displayCore->Context->SetLocation(machineSpindleAis_shape, T);
		displayCore->Context->UpdateCurrentViewer();
	}
	

}

