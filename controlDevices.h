
#include <wiringPi.h>
#include  <stdlib.h>

struct Devices
{
	char devicesName[128];
	int status;
	int pinNum; 
	
	int (*open)(int pinNum);
	int (*close)(int pinNum);
	int (*devicesInit)(int pinNum);

	int (*readStatus)(int pinNum);
	int (*changeStatus)(int status);

	struct Devices *next;
	
};

struct Devices* addBathroomLightToDevicesLink(struct Devices *phead);
struct Devices* addUpstairLightToDevicesLink(struct Devices *phead);
struct Devices* addRestaurantLightToDevicesLink(struct Devices *phead);
struct Devices* addDrawingLightToDevicesLink(struct Devices *phead);
struct Devices* addFireAlarmToDevicesLink(struct Devices *phead);



