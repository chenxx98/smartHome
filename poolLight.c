#include "controlDevices.h"

int poolLightOpen(int pinNum)
{
	digitalWrite(pinNum,LOW);
}

int poolLightClose(int pinNum)
{
	digitalWrite(pinNum,HIGH);
}

int poolLightCloseInit(int pinNum)
{
	pinMode(pinNum,OUTPUT);
	digitalWrite(pinNum,HIGH);
}


struct Devices poolLight = {

	.devicesName = "poolLight",
	.pinNum = 4,
	.open = poolLightOpen,
	.close = poolLightClose,
	.devicesInit = poolLightCloseInit
};

struct Devices* addPoolLightToDevicesLink(struct Devices *phead)
{
	if(phead == NULL){
		return &poolLight;
	}
	else{
		poolLight.next = phead;
		phead = &poolLight;
	}
}


