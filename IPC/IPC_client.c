#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/un.h>

int main(){
	unlink("server.sock");
	// create a socket
	int cfd = socket(AF_LOCAL, SOCK_STREAM, 0);
	//bind
	struct sockaddr_un addr;
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, "client.sock");
	int ret = bind(cfd, (struct sockaddr*)&addr, sizeof(addr));
	//listen
	ret = listen(cfd, 100);
	//connect 
	struct sockaddr_un saddr;
	saddr.sun_family = AF_LOCAL;
	strcpy(saddr.sun_path, "server.sock");
	ret = connect(cfd, (struct sockaddr*)&saddr, sizeof(saddr));
	//comminicate
	int num = 0;
	while(1){
		char buf[128];
		sprintf(buf, "hello %d\n", num++);
		int len = send(cfd, buf, strlen(buf), 0);	
		printf("client: %s\n", buf);
		
		len = recv(cfd, buf, sizeof(buf), 0);
		if (len == -1){
			perror("recv");
			exit(-1);	
		} else if (len == 0){
			printf("server closed");
			break;
		} else {
			printf("server say: %s\n", buf);
			send(cfd, buf, len, 0);	
		}
		sleep(1);
	}
	close(cfd);
	return 0;
}
