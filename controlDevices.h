#include <wiringPi.h>
#include  <stdlib.h>

#define true  1
#define false 0
typedef unsigned int bool;

struct Devices
{
	char devicesName[128];
	int status;
	int pinNum; 
	bool (*Ocamera)();
	int (*open)(int pinNum);
	int (*close)(int pinNum);
	
	int (*devicesInit)(int pinNum);
	int (*readStatus)(int pinNum);

	struct Devices *next;
	
};

struct Devices* addBathroomLightToDevicesLink(struct Devices *phead);
struct Devices* addPoolLightToDevicesLink(struct Devices *phead);
struct Devices* addFirstFloorLightToDevicesLink(struct Devices *phead);
struct Devices* addFireAlarmToDevicesLink(struct Devices *phead);
struct Devices* addBedroomLightToDevicesLink(struct Devices *phead);
struct Devices* addGateLockToDevicesLink(struct Devices *phead);
struct Devices* addBuzzerToDevicesLink(struct Devices *phead);
struct Devices* addCameraToDevicesLink(struct Devices *phead);
char* getBase64(char* photoPath);
size_t handle(void *ptr, size_t size, size_t nmemb, void *stream );



