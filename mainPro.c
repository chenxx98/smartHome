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

/* 全局变量，多线程共享  */
struct inputCommander *pCommandHead = NULL; //链表头
struct Devices		  *pDevicesHead = NULL;
struct inputCommander *socketHeadler = NULL;  
struct Devices 		  *gateLockHeadler = NULL;
/* 互斥锁、条件变量定义，宏初始化 */
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

/* 用于保存socket服务端 accept连接新的客户端返回的 socket描述符 */
int socket_fd; 

/*
 * @description		: 遍历设备端工厂查找输出控制设备
 * @param - name 	: 要查找的设备名
 * @param - phead 	: 设备工厂链表
 * @return 			: 成功,返回该设备结构指针;失败，返回NULL
 */
static struct Devices* findDeviceByName(char *name,struct Devices *phead)
{
	struct Devices *tmp = phead;
	if(tmp == NULL){
		return NULL;
	}	
	while(tmp != NULL)
	{
		if(strcmp(tmp->devicesName,name)==0){
			return tmp;
		}
		tmp = tmp->next;					
	}
	return NULL;	
}
/*
 * @description		: 遍历控制端工厂查找输入控制设备
 * @param - name 	: 要查找的设备名
 * @param - phead 	: 指令工厂链表
 * @return 			: 成功,返回该设备结构指针;失败，返回NULL
 */
static struct inputCommander* findCommandByName(char *name,struct inputCommander *phead)
{
	struct inputCommander *tmp = phead;
	if(tmp == NULL){
		return NULL;
	}
	while(tmp != NULL)
	{
		if(strcmp(tmp->commandName,name)==0){
			return tmp;
		}
		tmp = tmp->next;					
	}
	return NULL;	
}

/* @description:  控制设备函数 */
static void control_device(const struct inputCommander *controlHeadler)
{
	pthread_t cameraThread;
	pthread_mutex_lock(&mutex);
	struct Devices *openHeadler = NULL;
	struct Devices *closeHeadler = NULL;
	if(strstr(controlHeadler->command,"01") != NULL){	
		openHeadler  = findDeviceByName("FirstFloorLight",pDevicesHead);
	}
	else if(strstr(controlHeadler->command,"11") != NULL){
		closeHeadler = findDeviceByName("FirstFloorLight",pDevicesHead);
	}
	else if(strstr(controlHeadler->command,"02") != NULL){
		openHeadler  = findDeviceByName("bedroomLight",pDevicesHead);
	}
	else if(strstr(controlHeadler->command,"22") != NULL){
		closeHeadler = findDeviceByName("bedroomLight",pDevicesHead);
	}
	else if(strstr(controlHeadler->command,"03") != NULL){
		openHeadler  = findDeviceByName("bathroomLight",pDevicesHead);
	}
	else if(strstr(controlHeadler->command,"33") != NULL){
		closeHeadler = findDeviceByName("bathroomLight",pDevicesHead);
	}
	else if(strstr(controlHeadler->command,"04") != NULL){
		openHeadler  = findDeviceByName("poolLight",pDevicesHead);
	}
	else if(strstr(controlHeadler->command,"44") != NULL){
		closeHeadler = findDeviceByName("poolLight",pDevicesHead);
	}
	else if(strstr(controlHeadler->command,"05") != NULL){
		pthread_cond_signal(&cond); //唤醒识别线程
	}
	if(openHeadler != NULL){
		openHeadler->devicesInit(openHeadler->pinNum);
		openHeadler->open(openHeadler->pinNum);
		openHeadler = NULL;
	}	
	if(closeHeadler != NULL){
		closeHeadler->devicesInit(closeHeadler->pinNum);
		closeHeadler->close(closeHeadler->pinNum);
		closeHeadler = NULL;
	}
	pthread_mutex_unlock(&mutex);	
}

//每个线程要运行的例程:
/* @description:  人脸识别开锁线程 */	
static void * camera_thread(void * arg)
{
	struct Devices *cameraHeadler = NULL;
	cameraHeadler  = findDeviceByName("camera",pDevicesHead);
	if(cameraHeadler == NULL){
		printf("find camera error\n");
		exit(-1);
	}
	/* 门锁初始化 */
	gateLockHeadler = findDeviceByName("gateLock",pDevicesHead);
	if(gateLockHeadler == NULL){
		printf("find gateLock error\n");
		return -1;
	}
	else{
		gateLockHeadler->devicesInit(gateLockHeadler->pinNum);
		printf("%s init success\n",gateLockHeadler->devicesName);
	}
	 /* curl库初始化 */
	curl_global_init(CURL_GLOBAL_ALL); 
	printf("camera_thread init success\n");
	while(1)
	{
		pthread_cond_wait(&cond, &mutex); //睡眠等待
		if(cameraHeadler->open(cameraHeadler->pinNum) == true){  //人脸验证
			gateLockHeadler->open(gateLockHeadler->pinNum);		
			sleep(3);
			gateLockHeadler->close(gateLockHeadler->pinNum);
		}
	}
}

/* @description:  语音控制线程 */	
static void * voice_thread(void *arg)
{
	int nread;
	struct inputCommander *voiceHeadler = NULL;
 	voiceHeadler = findCommandByName("voice",pCommandHead);	
	if(voiceHeadler == NULL){
		printf("find voice error\n");
		exit(-1);
	}
	else{
		if(voiceHeadler->Init(voiceHeadler,NULL,NULL) < 0){
			printf("voice Init error\n");
			exit(-1);
		}
		else{
			printf("%s init success\n",voiceHeadler->commandName);
		}
		while(1)
		{
			memset(voiceHeadler->command,'\0',sizeof(voiceHeadler->command));
			nread = voiceHeadler->getCommand(voiceHeadler);
			if(nread == 0){
				//printf("no data from voice\n");
				continue;
			}
			else{
			    printf("do voice control:%s\n",voiceHeadler->command);
				control_device(voiceHeadler);
			}
		}
	}
}
/* @description:  接收手机客户端信号线程 */
static void * read_thread(void *arg)
{
	int n_read,new_fd;
	new_fd = socket_fd；
	while(1)
	{	
		memset(socketHeadler->command,'\0',sizeof(socketHeadler->command));
		n_read = read(new_fd, socketHeadler->command,sizeof(socketHeadler->command));
		if(n_read > 0){
			//printf("\nget: %d,%s\n",n_read,socketHeadler->command);
			control_device(socketHeadler);
		}
		else if(n_read <= 0){ 
			perror("read");
			close(new_fd);
			return;
		}
	}
}
/* @description:  socket服务端线程 */
static void * socket_thread(void *arg)
{
	int ret_fd = 0;
	pthread_t readThread;
	struct sockaddr_in c_addr;
	memset(&c_addr,0,sizeof(struct sockaddr_in));
	int clen = sizeof(struct sockaddr_in);

 	socketHeadler = findCommandByName("socketServer",pCommandHead);
	if(socketHeadler == NULL){
		printf("find socket error\n");
		exit(-1);
	}
	else{
		ret_fd = socketHeadler->Init(socketHeadler,NULL,NULL);
		if(ret_fd == -1){
			perror("socket init error\n");
			exit(-1);
		}
		else
			printf("%s init success\n",socketHeadler->commandName);
	}

	while(1)
	{	//accept无连接时，将堵塞等待
		socket_fd = accept(socketHeadler->sfd,(struct sockaddr *)&c_addr,&clen); 
		pthread_create(&readThread,NULL,read_thread,NULL); //起线程处理客户端接入
	}
}
/* @description:  火情监控报警线程 */
static void * fire_thread(void *arg)
{
	int fire = 0;
	struct Devices *buzzerHeadler = NULL;
	struct Devices *fireHeadler = NULL;

	buzzerHeadler = findDeviceByName("buzzer",pDevicesHead);
	if(buzzerHeadler == NULL){
		printf("find buzzer error\n");
		exit(-1);
	}
	else{
		buzzerHeadler->devicesInit(buzzerHeadler->pinNum);
		printf("%s init success\n",buzzerHeadler->devicesName);
	}
	fireHeadler = findDeviceByName("fireAlarm",pDevicesHead);
	if(fireHeadler == NULL){
		printf("find fire error\n");
		exit(-1);
	}
	else{
		fireHeadler->devicesInit(fireHeadler->pinNum);
		printf("%s init success\n",fireHeadler->devicesName);
	}
	while(1)
	{
		fire = fireHeadler->readStatus(fireHeadler->pinNum);
		if(fire == 1){
			gateLockHeadler->open(gateLockHeadler->pinNum);
			buzzerHeadler->open(buzzerHeadler->pinNum);
		}
		else if(fire == 0){
			buzzerHeadler->close(buzzerHeadler->pinNum);
		}
	}
}

int main(void)
{
	pthread_t th[4];
	/* 树莓派引脚初始化 */
	if(-1 == wiringPiSetup()){
		printf("PiSetup init error\n");
        return -1;
    }
	
	/* 指令工厂链表 */
	pCommandHead = addVoiceControlToInputCommandLink(pCommandHead); 
	pCommandHead = addSocketControlToInputCommandLink(pCommandHead);
	/* 设备工厂链表 */
	pDevicesHead = addFirstFloorLightToDevicesLink(pDevicesHead); 	
	pDevicesHead = addPoolLightToDevicesLink(pDevicesHead);			
	pDevicesHead = addBathroomLightToDevicesLink(pDevicesHead);		
	pDevicesHead = addBedroomLightToDevicesLink(pDevicesHead);		
	pDevicesHead = addFireAlarmToDevicesLink(pDevicesHead);			
	pDevicesHead = addGateLockToDevicesLink(pDevicesHead);				
	pDevicesHead = addBuzzerToDevicesLink(pDevicesHead);			
	pDevicesHead = addCameraToDevicesLink(pDevicesHead);

	/* 线程创建 */
	pthread_create(&th[0], NULL, fire_thread, NULL);
	pthread_create(&th[1], NULL, socket_thread, NULL);
	pthread_create(&th[2], NULL, voice_thread, NULL);
	pthread_create(&th[3], NULL, camera_thread, NULL);
	
	for(int i = 0; i <= 3; i++){
		pthread_join(th[i],NULL); //主线程挂起等待
	}
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	
	return 0;
}

