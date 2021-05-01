#include "controlDevices.h"

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

struct Devices bathroomLight = {

	.devicesName = "bathroomLight",
	.pinNum = 1,
	.open = bathroomLightOpen,
	.close = bathroomLightClose,
	.devicesInit = bathroomLightCloseInit,
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

