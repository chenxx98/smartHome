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

int restaurantLightOpen(int pinNum)
{
	digitalWrite(pinNum,LOW);
}

int restaurantLightClose(int pinNum)
{
	digitalWrite(pinNum,HIGH);
}

int restaurantLightCloseInit(int pinNum)
{
	pinMode(pinNum,OUTPUT);
	digitalWrite(pinNum,HIGH);
}

int restaurantLightCloseStatus(int status)
{

}

struct Devices restaurantLight = {

	.devicesName = "restaurantLight",
	.pinNum = 22,
	.open = restaurantLightOpen,
	.close = restaurantLightClose,
	.devicesInit = restaurantLightCloseInit,
	.changeStatus = restaurantLightCloseStatus
	
};

struct Devices* addRestaurantLightToDevicesLink(struct Devices *phead)
{
	if(phead == NULL){
		return &restaurantLight;
	}
	else{
		restaurantLight.next = phead;
		phead = &restaurantLight;
	}
}



