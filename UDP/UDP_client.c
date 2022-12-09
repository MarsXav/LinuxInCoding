#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main(){
	// create a socket
	int fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (fd == -1) {perror("socket"); exit(-1);}

	// server address info
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(9999);
	inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr.s_addr);
	
	// comminicate
	while(1){
		//send 
		char sendBuf[128], recvbuf[128];
		sprintf(sendBuf, "hello world\n");

		sendto(fd, sendBuf, strlen(sendBuf)+1, 0, (struct sockaddr*)&saddr, sizeof(saddr));
		
		//receive
		int num = recvfrom(fd, recvbuf, sizeof(recvbuf), 0, NULL, NULL);	
		printf("server data: %s\n", recvbuf);

		sleep(1);
	
	}
	close(fd);

	return 0;

}
