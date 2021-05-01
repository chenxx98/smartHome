#include "controlDevices.h"

int gateLockOpen(int pinNum)
{
	digitalWrite(pinNum,LOW);
}

int gateLockClose(int pinNum)
{
	digitalWrite(pinNum,HIGH);
}

int gateLockCloseInit(int pinNum)
{
	pinMode(pinNum,OUTPUT);
	digitalWrite(pinNum,HIGH);
}

struct Devices gateLock = {

	.devicesName = "gateLock",
	.pinNum = 26,
	.open = gateLockOpen,
	.close = gateLockClose,
	.devicesInit = gateLockCloseInit
};

struct Devices* addGateLockToDevicesLink(struct Devices *phead)
{
	if(phead == NULL){
		return &gateLock;
	}
	else{
		gateLock.next = phead;
		phead = &gateLock;
	}
}


