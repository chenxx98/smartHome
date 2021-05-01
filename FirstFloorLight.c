#include "controlDevices.h"

int FirstFloorLightOpen(int pinNum)
{
	digitalWrite(pinNum,LOW);
}

int FirstFloorLightClose(int pinNum)
{
	digitalWrite(pinNum,HIGH);
}

int FirstFloorLightCloseInit(int pinNum)
{
	pinMode(pinNum,OUTPUT);
	digitalWrite(pinNum,HIGH);
}

struct Devices FirstFloorLight = {

	.devicesName = "FirstFloorLight",
	.pinNum = 5,
	.open = FirstFloorLightOpen,
	.close =FirstFloorLightClose,
	.devicesInit = FirstFloorLightCloseInit
	
};

struct Devices* addFirstFloorLightToDevicesLink(struct Devices *phead)
{
	if(phead == NULL){
		return &FirstFloorLight;
	}
	else{
		FirstFloorLight.next = phead;
		phead = &FirstFloorLight;
	}
}




