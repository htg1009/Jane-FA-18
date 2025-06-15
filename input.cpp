#include "F18.h"

//************************************************************************************************
void GetStickInputs(void)
{

	ReadJoyVarD(dwDeadZone);
	RollStickPos =  ((signed int)GetJoyDeadX()-32768) >> 8;
	PitchStickPos = ((signed int)GetJoyDeadY()-32768) >> 8;
	Throttle  =     (GetJoyZ())>>9;
	Rudder = 0;
}
