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

	// bind
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9999);
	addr.sin_addr.s_addr = INADDR_ANY;
	
	int ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	
	// comminicate
	while(1){
		char buf[128];
		int num = recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL);
		printf("server data: %s\n", buf);	
	}
	close(fd);
	return 0;
}
