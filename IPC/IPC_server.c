#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/un.h>

int main(){
	unlink("server.sock");//delete the server.sock file
	// create a socket
	int lfd = socket(AF_LOCAL, SOCK_STREAM, 0);
	//bind
	struct sockaddr_un addr;
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, "server.sock");
	int ret = bind(lfd, (struct sockaddr*)&addr, sizeof(addr));
	//listen
	ret = listen(lfd, 100);
	// accept
	struct sockaddr_un cliaddr;
	int len = sizeof(cliaddr);
	int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &len);
	printf("client socket filename: %s\n", cliaddr.sun_path);
	//comminicate
	while(1){
		char buf[128];
		int len = recv(cfd, buf, sizeof(buf), 0);	
		if (len == -1){
			perror("recv");
			exit(-1);	
		} else if (len == 0){
			printf("client closed");
			break;
		} else {
			printf("client say: %s\n", buf);
			send(cfd, buf, len, 0);	
		}
	}
	close(cfd);
	close(lfd);
	return 0;
}

