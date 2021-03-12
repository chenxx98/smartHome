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

int bathroomLightOpen(int pinNum)
{
	digitalWrite(pinNum,LOW);
}

int bathroomLightClose(int pinNum)
{
	digitalWrite(pinNum,HIGH);
}

int bathroomLightCloseInit(int pinNum)
{
	pinMode(pinNum,OUTPUT);
	digitalWrite(pinNum,HIGH);
}

int bathroomLightCloseStatus(int status)
{

}

struct Devices bathroomLight = {

	.devicesName = "bathroomLight",
	.pinNum = 24,
	.open = bathroomLightOpen,
	.close = bathroomLightClose,
	.devicesInit = bathroomLightCloseInit,
	.changeStatus = bathroomLightCloseStatus
	
};

struct Devices* addBathroomLightToDevicesLink(struct Devices *phead)
{
	if(phead == NULL){
		return &bathroomLight;
	}
	else{
		bathroomLight.next = phead;
		phead = &bathroomLight;
	}
}


