#include "controlDevices.h"

int buzzerOpen(int pinNum)
{
	digitalWrite(pinNum,LOW);
}

int buzzerClose(int pinNum)
{
	digitalWrite(pinNum,HIGH);
}

int buzzerCloseInit(int pinNum)
{
	pinMode(pinNum,OUTPUT);
	digitalWrite(pinNum,HIGH);
}

struct Devices buzzer = {

	.devicesName = "buzzer",
	.pinNum = 3,
	.open = buzzerOpen,
	.close = buzzerClose,
	.devicesInit = buzzerCloseInit
};

struct Devices* addBuzzerToDevicesLink(struct Devices *phead)
{
	if(phead == NULL){
		return &buzzer;
	}
	else{
		buzzer.next = phead;
		phead = &buzzer;
	}
}


