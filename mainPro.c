#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <curl/curl.h>

#include "controlDevices.h"
#include "inputCommand.h"

struct inputCommander *pCommandHead = NULL;   
struct inputCommander *socketHeadler = NULL;
struct Devices		  *pDevicesHead = NULL;
struct Devices 		  *gateLockHeadler = NULL;
struct Devices        *buzzerHeadler = NULL;

pthread_t cameraThread;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int c_fd;

struct Devices* findDeviceByName(char *name,struct Devices *phead)
{	
	struct Devices *tmp = phead;
	if(phead == NULL)
	{
		return NULL;
	}
	else
	{
		while(tmp != NULL)
		{
			//printf("devicesName1=%s###\n",tmp->devicesName);
			//printf("name2=%s###\n",name);
			if(strcmp(tmp->devicesName,name)==0)
			{
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
	if(phead == NULL)
	{
		return NULL;
	}
	else
	{
		while(tmp != NULL)
		{
			if(strcmp(tmp->commandName,name)==0)
			{
				return tmp;
			}
			tmp = tmp->next;					
		}
		return NULL;
	}	
}

	
void * camera_thread(void* datas)
{
	pthread_mutex_lock(&mutex);
	struct Devices *cameraHeadler = NULL;
	cameraHeadler  = findDeviceByName("camera",pDevicesHead);
	if(cameraHeadler == NULL)
	{
		printf("find camera error\n");
		pthread_exit(NULL);
	}
	else
	{
		if(cameraHeadler->Ocamera() == true)
		{
			gateLockHeadler->open(gateLockHeadler->pinNum);		
			sleep(3);
			gateLockHeadler->close(gateLockHeadler->pinNum);
		}
		pthread_mutex_unlock(&mutex);
		pthread_exit(NULL);
	}
}


void * voice_thread(void* datas)
{
	int nread;
	struct inputCommander *voiceHeadler = NULL;
	struct Devices *openHeadler = NULL;
	struct Devices *closeHeadler = NULL;
 	voiceHeadler = findCommandByName("voice",pCommandHead);
	
	if(voiceHeadler == NULL)
	{
		printf("find voice error\n");
		pthread_exit(NULL);
	}else
	{
		if(voiceHeadler->Init(voiceHeadler,NULL,NULL) < 0)
		{
			printf("voice Init error\n");
			pthread_exit(NULL);
		}
		else
		{
			printf("%s init success\n",voiceHeadler->commandName);
		}
		while(1)
		{
			memset(voiceHeadler->command,'\0',sizeof(voiceHeadler->command));
			nread = voiceHeadler->getCommand(voiceHeadler);
			if(nread == 0)
			{
				//printf("no data from voice\n");
			}
			else
			{
			    //printf("do voice control:%s\n",voiceHeadler->command);
				pthread_mutex_lock(&mutex);
				if(strstr(voiceHeadler->command,"01") != NULL)//GPIO5
				{	
				    openHeadler  = findDeviceByName("FirstFloorLight",pDevicesHead);
				}
				else if(strstr(voiceHeadler->command,"11") != NULL)
				{
					closeHeadler = findDeviceByName("FirstFloorLight",pDevicesHead);
				}
				else if(strstr(voiceHeadler->command,"02") != NULL)//GPIO4
				{
				    openHeadler  = findDeviceByName("bedroomLight",pDevicesHead);
				}
				else if(strstr(voiceHeadler->command,"22") != NULL)
				{
					closeHeadler = findDeviceByName("bedroomLight",pDevicesHead);
				}
				else if(strstr(voiceHeadler->command,"03") != NULL)//GPIO1
				{
				    openHeadler  = findDeviceByName("bathroomLight",pDevicesHead);
				}
				else if(strstr(voiceHeadler->command,"33") != NULL)
				{
					closeHeadler = findDeviceByName("bathroomLight",pDevicesHead);
				}
				else if(strstr(voiceHeadler->command,"04") != NULL)//GPIO6
				{
				    openHeadler  = findDeviceByName("poolLight",pDevicesHead);
				}
				else if(strstr(voiceHeadler->command,"44") != NULL)
				{
					closeHeadler = findDeviceByName("poolLight",pDevicesHead);
				}
				else if(strstr(voiceHeadler->command,"05") != NULL)
				{
					pthread_create(&cameraThread,NULL,camera_thread,NULL);
				}
				if(openHeadler != NULL)
				{
					openHeadler->devicesInit(openHeadler->pinNum);
					openHeadler->open(openHeadler->pinNum);
					openHeadler = NULL;
				}
				if(closeHeadler != NULL)
				{
					closeHeadler->devicesInit(closeHeadler->pinNum);
					closeHeadler->close(closeHeadler->pinNum);
					closeHeadler = NULL;
				}
				pthread_mutex_unlock(&mutex);

			}
		}
	}
}

void * read_thread(void* datas)
{
	int n_read;
	struct Devices *socketOpenHeadler = NULL;
	struct Devices *socketCloseHeadler = NULL;
	while(1)
	{	
		memset(socketHeadler->command,'\0',sizeof(socketHeadler->command));
		n_read = read(c_fd, socketHeadler->command,sizeof(socketHeadler->command));
		if(n_read == -1)
		{
			perror("read");
			exit(-1);
		}
		else if(n_read > 0)
		{
			//printf("\nget: %d,%s\n",n_read,socketHeadler->command);
			if(strstr(socketHeadler->command,"01") != NULL)
			{
				socketOpenHeadler  = findDeviceByName("FirstFloorLight",pDevicesHead);
			}
			else if(strstr(socketHeadler->command,"11") != NULL)
			{
				socketCloseHeadler = findDeviceByName("FirstFloorLight",pDevicesHead);
			}
			else if(strstr(socketHeadler->command,"02") != NULL)
			{
				socketOpenHeadler  = findDeviceByName("bedroomLight",pDevicesHead);
			}
			else if(strstr(socketHeadler->command,"22") != NULL)
			{
				socketCloseHeadler = findDeviceByName("bedroomLight",pDevicesHead);
			}
			else if(strstr(socketHeadler->command,"03") != NULL)
			{
					socketOpenHeadler  = findDeviceByName("bathroomLight",pDevicesHead);
			}
			else if(strstr(socketHeadler->command,"33") != NULL)
			{
				socketCloseHeadler = findDeviceByName("bathroomLight",pDevicesHead);
			}
			else if(strstr(socketHeadler->command,"04") != NULL)
			{
				socketOpenHeadler  = findDeviceByName("poolLight",pDevicesHead);
			}
			else if(strstr(socketHeadler->command,"44") != NULL)
			{
				socketCloseHeadler = findDeviceByName("poolLight",pDevicesHead);
			}
			else if(strstr(socketHeadler->command,"05") != NULL)
			{
				pthread_create(&cameraThread,NULL,camera_thread,NULL);
			}			
			else if(strstr(socketHeadler->command,"06") != NULL)
			{
				buzzerHeadler->open(buzzerHeadler->pinNum);
			}
			else if(strstr(socketHeadler->command,"66") != NULL)
			{
				buzzerHeadler->close(buzzerHeadler->pinNum);
			}
			if(socketOpenHeadler != NULL)
			{
				socketOpenHeadler->devicesInit(socketOpenHeadler->pinNum);
				socketOpenHeadler->open(socketOpenHeadler->pinNum);
			 	socketOpenHeadler = NULL;
			}
			if(socketCloseHeadler != NULL)
			{
				socketCloseHeadler->devicesInit(socketCloseHeadler->pinNum);
				socketCloseHeadler->close(socketCloseHeadler->pinNum);
			 	socketCloseHeadler = NULL;
			}
		}
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
	if(socketHeadler == NULL)
	{
		printf("find socket error\n");
		pthread_exit(NULL);
	}
	else
	{
		socketHeadler->Init(socketHeadler,NULL,NULL);
		printf("%s init success\n",socketHeadler->commandName);
	}
	
	while(1)
	{
		c_fd = accept(socketHeadler->sfd,(struct sockaddr *)&c_addr,&clen);
		pthread_create(&readThread,NULL,read_thread,NULL);
	}
}



void * fire_thread(void* datas)
{
	int fire;
	struct Devices *fireHeadler = NULL;
	fireHeadler = findDeviceByName("fireAlarm",pDevicesHead);
	gateLockHeadler = findDeviceByName("gateLock",pDevicesHead);
	buzzerHeadler = findDeviceByName("buzzer",pDevicesHead);

	if(gateLockHeadler == NULL)
	{
		printf("find gateLock error\n");
	}
	else
	{
		gateLockHeadler->devicesInit(gateLockHeadler->pinNum);
		printf("%s init success\n",gateLockHeadler->devicesName);
	}
	if(buzzerHeadler == NULL)
	{
		printf("find buzzer error\n");
	}
	else
	{
		buzzerHeadler->devicesInit(buzzerHeadler->pinNum);
		printf("%s init success\n",buzzerHeadler->devicesName);
	}
	if(fireHeadler == NULL)
	{
		printf("find fire error\n");
		pthread_exit(NULL);
	}
	else
	{
		fireHeadler->devicesInit(fireHeadler->pinNum);
		printf("%s init success\n",fireHeadler->devicesName);
	}
	while(1)
	{
		fire = fireHeadler->readStatus(fireHeadler->pinNum);
		if(fire == 1)
		{
			buzzerHeadler->open(buzzerHeadler->pinNum);
		}
		else
		{
			buzzerHeadler->close(buzzerHeadler->pinNum);
		}
	}
}

int main()
{
	pthread_t voiceThread;
	pthread_t socketThread;
	pthread_t fireThread;
	if(-1 == wiringPiSetup())
	{
			printf("PiSetup init error\n");
            return -1;
    }
	curl_global_init(CURL_GLOBAL_ALL); 
	
	pCommandHead = addVoiceControlToInputCommandLink(pCommandHead); 
	pCommandHead = addSocketControlToInputCommandLink(pCommandHead);

	pDevicesHead = addFirstFloorLightToDevicesLink(pDevicesHead); 	
	pDevicesHead = addPoolLightToDevicesLink(pDevicesHead);			
	pDevicesHead = addBathroomLightToDevicesLink(pDevicesHead);		
	pDevicesHead = addBedroomLightToDevicesLink(pDevicesHead);		
	pDevicesHead = addFireAlarmToDevicesLink(pDevicesHead);			
	pDevicesHead = addGateLockToDevicesLink(pDevicesHead);				
	pDevicesHead = addBuzzerToDevicesLink(pDevicesHead);			
	pDevicesHead = addCameraToDevicesLink(pDevicesHead);
	
	pthread_create(&fireThread,NULL,fire_thread,NULL);
	pthread_create(&voiceThread,NULL,voice_thread,NULL);
	pthread_create(&socketThread,NULL,socket_thread,NULL);
	
	pthread_join(fireThread,NULL);
	pthread_join(voiceThread,NULL);
	pthread_join(socketThread,NULL);

	return 0;
}

