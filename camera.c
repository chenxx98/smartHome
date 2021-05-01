#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <curl/curl.h>

#include "controlDevices.h"

char* faceRes = NULL;

char* getBase64(char* photoPath)
{
	char cmd[256] = {'\0'};
	int fd1;
	int size1;
	
	sprintf(cmd, "base64 %s > photoBase64File", photoPath);
	system(cmd);
	
	fd1 = open("./photoBase64File", O_RDWR);
	size1 = lseek(fd1, 0, SEEK_END) + 1;
	lseek(fd1, 0, SEEK_SET);
	
	char* res1 = (char*)malloc(size1);
	memset(res1, '\0', size1);
	
	read(fd1, res1, size1);
	close(fd1);
	
	system("rm -f photoBase64File");
	return res1;
}

size_t handle(void *ptr, size_t size, size_t nmemb, void *stream )	
{
	int size2 = strlen(ptr) + 128;
	faceRes = (char*)malloc(size2);
	memset(faceRes, '\0', size2);
	strncpy(faceRes, ptr, size2);
}

bool postUrl()
{
	//printf("%s|%s|%d\n",   __FILE__, __func__, __LINE__);

	CURL *curl = NULL;
	CURLcode res = CURLE_OK;
	
	char* message = NULL;
	
	char* img1 = getBase64("./cjx.jpg");

	char* openCamera = "raspistill -q 5 -t 1 -o visitor.jpg";
	system(openCamera);	
	char* img2 = getBase64("./visitor.jpg");
	system("rm ./visitor.jpg");


	//OCR Key,Secret
	const char* key = "JkLp8ax76PbZytBvYkxgUU";
	char* secret = "a01f9b48d4f649f2b1456e19da60ad05";
	int typeId = 9999;
	char* format = "xml";
	int size = strlen(img1)+strlen(img2)+strlen(key)+strlen(secret)+strlen(format)+1024;

	message = (char*)malloc(size);
	memset(message, '\0', size);

	sprintf(message, "&img1=%s&img2=%s&key=%s&secret=%s&typeId=%d&format=%s",
		img1, img2, key, secret, typeId, format);

	curl = curl_easy_init(); 
	if(curl)
	{	
		curl_easy_setopt(curl, CURLOPT_URL, "https://netocr.com/api/faceliu.do");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}

	if(res != 23){
		printf("post failed!\n");
		free(faceRes);
		free(img1);
		free(img2);
	}
	else
	{
		printf("post successful!\n");
		//printf("%s\n",faceRes);
		if(strstr(faceRes, "Êò?) != NULL)
		{
			printf("This is the same person!\n");
			free(faceRes);
			free(img1);
			free(img2);
			return true;
		}
		else if(strstr(faceRes, "Âê?) != NULL)
		{
			printf("Tow different persons!\n");
			free(faceRes);
			free(img1);
			free(img2);
			return false;
		}	
	}
}


struct Devices camera = {

	.devicesName = "camera",
	.Ocamera = postUrl,
};

struct Devices* addCameraToDevicesLink(struct Devices *phead)
{
	if(phead == NULL){
		return &camera;
	}
	else{
		camera.next = phead;
		phead = &camera;
	}
}



