#include "controlDevices.h"

int fireAlarmInit(int pinNum)
{
	pinMode(pinNum,INPUT);
}

int fireAlarmStatusRead(int pinNum)
{
	return digitalRead(pinNum);
}

struct Devices fireAlarm = {

	.devicesName = "fireAlarm",
	.pinNum = 7,
	.devicesInit = fireAlarmInit,
	.readStatus = fireAlarmStatusRead
	
};

struct Devices* addFireAlarmToDevicesLink(struct Devices *phead)
{
	if(phead == NULL){
		return &fireAlarm;
	}
	else{
		fireAlarm.next = phead;
		phead = &fireAlarm;
	}
}


