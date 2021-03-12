
#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



struct inputCommander
{
	char commandName[128];
	char devicesName[128];
	char command[32];
	char log[1024];
	int fd;
	char port[12];
	char ipAddress[32];
	int sfd;
	
	int (*Init)(struct inputCommander *voicer, char *ipAdress, char *port);
	int (*getCommand)(struct inputCommander *voicer);

	struct inputCommander *next;
	
};

struct inputCommander* addVoiceControlToInputCommandLink(struct inputCommander *phead);
struct inputCommander* addSocketControlToInputCommandLink(struct inputCommander *phead);


