#include "controlDevices.h"

int bedroomLightOpen(int pinNum)
{
	digitalWrite(pinNum,LOW);
}

int bedroomLightClose(int pinNum)
{
	digitalWrite(pinNum,HIGH);
}

int bedroomLightCloseInit(int pinNum)
{
	pinMode(pinNum,OUTPUT);
	digitalWrite(pinNum,HIGH);
}


struct Devices bedroomLight = {

	.devicesName = "bedroomLight",
	.pinNum = 6,
	.open = bedroomLightOpen,
	.close = bedroomLightClose,
	.devicesInit = bedroomLightCloseInit
};

struct Devices* addBedroomLightToDevicesLink(struct Devices *phead)
{
	if(phead == NULL){
		return &bedroomLight;
	}
	else{
		bedroomLight.next = phead;
		phead = &bedroomLight;
	}
}


