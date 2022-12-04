#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main(){
	// 1. create a socket
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	// 2. connect to the server
	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr.s_addr);
	serveraddr.sin_port = htons(9999);
	int ret = connect(fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	// 3. communicate
	int i = 0;
	char buf[1024] = {0};
	while(1){
		sprintf(buf, "data : %d\n", i++);
		write(fd, buf, strlen(buf));
		sleep(1);
		int len = read(fd, buf, sizeof(buf));
		if (len == -1) {perror("read"); exit(-1);}
		else if (len > 0) {printf("receive server: %s\n", buf);}
		else if (len == 0) {printf("client closed\n");}
	}
	close(fd);
	return 0;
}
