#include "controlDevices.h"



int fireAlarmInit(int pinNum)
{
	pinMode(pinNum,INPUT);
	digitalWrite(pinNum,HIGH);
}

int fireAlarmStatusRead(int pinNum)
{
	return digitalRead(pinNum);
}

struct Devices fireAlarm = {

	.devicesName = "fireAlarm",
	.pinNum = 25,

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


