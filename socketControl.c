
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "inputCommand.h"


int SocketInit(struct inputCommander *socketMes, char *ipAdress, char *port)
{
	int s_fd;

	struct sockaddr_in s_addr;
	
	memset(&s_addr,0,sizeof(struct sockaddr_in));

	
	//1.socket
	s_fd = socket(AF_INET,SOCK_STREAM, 0);
	if(s_fd == -1){
		perror("socket");
		exit(-1);
	}

	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(atoi(socketMes->port));
	inet_aton(socketMes->ipAddress,&s_addr.sin_addr);

	//2. bind
	bind(s_fd,(struct sockaddr *)&s_addr,sizeof(struct sockaddr_in));

	//3.listen
	listen(s_fd,10);	
	printf("listen....\n");
	socketMes->sfd = s_fd;
	
	return s_fd;

}


int SocketGetCommand(struct inputCommander *socketMes)
{
	int c_fd;
	int n_read = 0;
	
	struct sockaddr_in c_addr;
	memset(&c_addr,0,sizeof(struct sockaddr_in));
	int clen = sizeof(struct sockaddr_in);

	c_fd = accept(socketMes->sfd,(struct sockaddr *)&c_addr,&clen);
	
	n_read = read(c_fd, socketMes->command,sizeof(socketMes->command));
	if(n_read == -1){
		perror("read");
	}else if(n_read > 0){
		printf("\nget: %d\n",n_read);
	}else{
		printf("client quit\n");
	}
			return n_read;
}



struct inputCommander socketControl = {

	.commandName = "socketServer",
	.command = {'\0'},
	.port = "8083",
	.ipAddress = "192.168.1.123",
	.Init = SocketInit,
	.getCommand = SocketGetCommand,
	.log = {'\0'},
	.next = NULL
};


struct inputCommander* addSocketControlToInputCommandLink(struct inputCommander *phead)
{
	if(phead == NULL){
		return &socketControl;
	}
	else{
		socketControl.next = phead;
		phead = &socketControl;
	}

}






