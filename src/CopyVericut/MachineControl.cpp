#include "MachineControl.h"

MachineControl::MachineControl()
{

}

void MachineControl::SetMachineWorkTable(Handle(AIS_Shape) MachineWorkTable)
{
	machineWorkTableAis_shape = MachineWorkTable;
}

void MachineControl::SetMachineSpindle(Handle(AIS_Shape) MachineSpindle)
{
	machineSpindleAis_shape = MachineSpindle;
}

