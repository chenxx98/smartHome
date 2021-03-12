#include "controlDevices.h"

/*
struct Devices
{
	char devicesName[128];
	int status;
	int pinNum; 
	
	int (*open)(int pinNum);
	int (*close)(int pinNum);
	int (*devicesInit)(int pinNum);

	int (*readStatus)();
	int (*changeStatus)(int status);

	struct Devices *next;
	
}
*/

int upstairLightOpen(int pinNum)
{
	digitalWrite(pinNum,LOW);
}

int upstairLightClose(int pinNum)
{
	digitalWrite(pinNum,HIGH);
}

int upstairLightCloseInit(int pinNum)
{
	pinMode(pinNum,OUTPUT);
	digitalWrite(pinNum,HIGH);
}

int upstairLightCloseStatus(int status)
{

}

struct Devices upstairLight = {

	.devicesName = "upstairLight",
	.pinNum = 23,
	.open = upstairLightOpen,
	.close = upstairLightClose,
	.devicesInit = upstairLightCloseInit,
	.changeStatus = upstairLightCloseStatus
	
};

struct Devices* addUpstairLightToDevicesLink(struct Devices *phead)
{
	if(phead == NULL){
		return &upstairLight;
	}
	else{
		upstairLight.next = phead;
		phead = &upstairLight;
	}
}



