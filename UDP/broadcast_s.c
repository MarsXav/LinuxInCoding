#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>

// int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
// 	parameters: 
// 		sockfd,
// 		level: SOL_SOCKET
// 		optname: SO_BROADCAST
// 		optval: integer, 1 as allow broadcast
// 		optlen: length of optval
//
int main(){
	// create a socket
	int fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (fd == -1) {perror("socket"); exit(-1);}

	// set broadcst attribute
	int op = 1;
	setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &op, sizeof(op));

	// create broadcast address
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9999);
	inet_pton(AF_INET, "192.168.125.255", &addr.sin_addr.s_addr);
	
	// comminicate
	int num = 0;
	while(1){
		char sendBuf[128];
		sprintf(sendBuf, "hello world : %d\n", num++);	
		sendto(fd, sendBuf, strlen(sendBuf)+1, 0, (struct sockaddr*)&addr, sizeof(addr));
		printf("Broadcast data: %s\n", sendBuf);
		sleep(1);
	}
	close(fd);
	return 0;
}
