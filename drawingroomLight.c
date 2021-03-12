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

int drawingLightOpen(int pinNum)
{
	digitalWrite(pinNum,LOW);
}

int drawingLightClose(int pinNum)
{
	digitalWrite(pinNum,HIGH);
}

int drawingLightCloseInit(int pinNum)
{
	pinMode(pinNum,OUTPUT);
	digitalWrite(pinNum,HIGH);
}

int drawingLightCloseStatus(int status)
{

}

struct Devices drawingLight = {

	.devicesName = "drawingLight",
	.pinNum = 21,
	.open = drawingLightOpen,
	.close = drawingLightClose,
	.devicesInit = drawingLightCloseInit,
	.changeStatus = drawingLightCloseStatus
	
};

struct Devices* addDrawingLightToDevicesLink(struct Devices *phead)
{
	if(phead == NULL){
		return &drawingLight;
	}
	else{
		drawingLight.next = phead;
		phead = &drawingLight;
	}
}




