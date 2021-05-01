#include <wiringSerial.h>
#include <unistd.h>

#include "inputCommand.h"

int voiceGetCommand(struct inputCommander *voicer)
{
	int nread = 0;
	
	nread = read(voicer->fd, voicer->command,sizeof(voicer->command));

	return nread;
}

int voiceInit(struct inputCommander *voicer, char *ipAdress, char *port)
{  
	int fd;
	if((fd = serialOpen(voicer->devicesName,9600))==-1){
			exit(-1);  
	}
	voicer->fd = fd;
	return fd;
}

struct inputCommander voiceControl = {

	.commandName = "voice",
	.devicesName = "/dev/ttyAMA0",
	.command = {'\0'},
	.Init = voiceInit,
	.getCommand = voiceGetCommand,
	.log = {'\0'},
	.next = NULL
};


struct inputCommander* addVoiceControlToInputCommandLink(struct inputCommander *phead)
{
	if(phead == NULL){
		return &voiceControl;
	}
	else{
		voiceControl.next = phead;
		phead = &voiceControl;
	}

}






