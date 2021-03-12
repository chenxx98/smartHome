
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "controlDevices.h"
#include "inputCommand.h"

struct inputCommander *pCommandHead = NULL;  
struct Devices		  *pDevicesHead = NULL; 

struct inputCommander *socketHeadler = NULL;
int c_fd;

struct Devices* findDeviceByName(char *name,struct Devices *phead)
{
	struct Devices *tmp = phead;
	if(phead == NULL){
		return NULL;
	}else{
		while(tmp != NULL){
			if(strcmp(tmp->devicesName,name)==0){
				return tmp;
			}
				tmp = tmp->next;					
		}

		return NULL;
	}
	
}

struct inputCommander* findCommandByName(char *name,struct inputCommander *phead)
{
	struct inputCommander *tmp = phead;
	if(phead == NULL){
		return NULL;
	}else{
		while(tmp != NULL){
			if(strcmp(tmp->commandName,name)==0){
				return tmp;
			}
				tmp = tmp->next;					
		}

		return NULL;
	}
	
}


void * voice_thread(void* datas)
{
	int nread;
	struct inputCommander *voiceHeadler;
	
 	voiceHeadler = findCommandByName("voice",pCommandHead);
	;
	if(voiceHeadler == NULL){
		printf("find voiceHeadler error\n");
		pthread_exit(NULL);
	}else{
		if(voiceHeadler->Init(voiceHeadler,NULL,NULL) < 0){
			printf("voice Init error\n");
			pthread_exit(NULL);
		}else{
			printf("%s init success\n",voiceHeadler->commandName);
		}
			
		while(1){
			nread = voiceHeadler->getCommand(voiceHeadler);
			if(nread == 0){
			printf("nodata from voice\n");
			}else{
			printf("do voice control:%s\n",voiceHeadler->command);
			}
		}
	}
}


void * read_thread(void* datas)
{
	printf("read_thread Init\n");

	int n_read;
	memset(socketHeadler->command,'\0',sizeof(socketHeadler->command));
	n_read = read(c_fd, socketHeadler->command,sizeof(socketHeadler->command));
	if(n_read == -1){
		perror("read");
	}else if(n_read > 0){
		printf("\nget: %d,%s\n",n_read,socketHeadler->command);
	}else{
		printf("client quit\n");
	}
	
}

void * socket_thread(void* datas)
{

	int n_read = 0;
	pthread_t readThread;
	
	struct sockaddr_in c_addr;
	memset(&c_addr,0,sizeof(struct sockaddr_in));
	int clen = sizeof(struct sockaddr_in);

 	socketHeadler = findCommandByName("socketServer",pCommandHead);
	if(socketHeadler == NULL){
		printf("find socketHeadler error\n");
		pthread_exit(NULL);
	}else{
		printf("%s init success\n",socketHeadler->commandName);
	}

	socketHeadler->Init(socketHeadler,NULL,NULL);

	while(1){
		
		c_fd = accept(socketHeadler->sfd,(struct sockaddr *)&c_addr,&clen);
		pthread_create(&readThread,NULL,read_thread,NULL);
	}
}




int main()
{


	char name[128];
	struct Devices *tmp = NULL;
	pthread_t voiceThread;
	pthread_t socketThread;
	
	if(-1 == wiringPiSetup()){
            return -1;
    }
	 

	//1.指令工厂初始化
	pCommandHead = addVoiceControlToInputCommandLink(pCommandHead); 
	pCommandHead = addSocketControlToInputCommandLink(pCommandHead);

	//2.设备控制初始化	
	pDevicesHead = addDrawingLightToDevicesLink(pDevicesHead); //21
	pDevicesHead = addRestaurantLightToDevicesLink(pDevicesHead);//22
	pDevicesHead = addBathroomLightToDevicesLink(pDevicesHead);//23
	pDevicesHead = addUpstairLightToDevicesLink(pDevicesHead);//24
	pDevicesHead = addFireAlarmToDevicesLink(pDevicesHead);//25
	
	//3.线程池建立
	//3.1语音线程
	pthread_create(&voiceThread,NULL,voice_thread,NULL);
	//3.2socket线程
	pthread_create(&socketThread,NULL,socket_thread,NULL);
	

	//3.3摄像头线程

	//3.4火灾线程

	pthread_join(voiceThread,NULL);
	pthread_join(socketThread,NULL);

	return 0;
}

