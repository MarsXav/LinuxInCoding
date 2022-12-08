#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

struct sockInfo {
	int fd; // file descriptor
	pthread_t tid; //thread id
	struct sockaddr_in addr;
};

void * func (void *arg){
	// communicate with child thread
	//obtain server info
	struct sockInfo *pinfo = (struct sockInfo *) arg;

	char clientIP[16];

	inet_ntop(AF_INET, &pinfo -> addr.sin_addr.s_addr, clientIP, sizeof(clientIP));
	unsigned short clientPort = ntohs(pinfo -> addr.sin_port);
	printf("chilent ip is : %s, port is %d\n", clientIP, clientPort);

	// receive data sent from the server
	char buf[1024] = {0};
	while(1) {
		int len = read(pinfo -> fd, &buf, sizeof(buf));
		if (len == -1){perror("read"); exit(-1);}
		else if (len > 0) printf("receive client: %s\n", buf);
		else if (len == 0) printf("client closed.. \n");

		write(pinfo -> fd, buf, strlen(buf));
	}
	close(pinfo -> fd);
}

struct sockInfo sockinfos[128];

int main(){
	// create a socket
	int lfd = socket(PF_INET, SOCK_STREAM, 0);
	if (lfd == -1) {perror("socket"); exit(-1);}

	// bind
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(9999);
	saddr.sin_addr.s_addr = INADDR_ANY;
	int ret = bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));
	if (ret == -1) {perror("bind"); exit(-1);}

	// listen
	ret = listen(lfd, 5);
	if (ret == -1) {perror("listen"); exit(-1);}

	//initialise data
	int maxi = sizeof(sockinfos)/sizeof(sockinfos[0]);

	for (int i = 0; i < maxi; i++){
		bzero(&sockinfos[i], sizeof(sockinfos[i]));
		sockinfos[i].fd = -1;
		sockinfos[i].tid = -1;
	}

	// wait for client to connect, create a child thread when a connection is established
	while(1){
		struct sockaddr_in clientaddr;
		int len = sizeof(clientaddr);
		int cfd = accept(lfd, (struct sockaddr*)&clientaddr, &len);	
		if (errno == EINTR) continue;
		if (cfd == -1) {perror("accept"); exit(-1);}

		struct sockInfo *pinfo;
		for (int i = 0; i < maxi; i++){
			// find a workable sockInfo element in the array
			if (sockinfos[i].fd == -1){
				pinfo = &sockinfos[i];
				break;	
			}	
			if (i == maxi - 1) {
				sleep(1);
				i--;	
			}
		}
		pinfo -> fd = cfd;
		memcpy(&pinfo -> addr, &clientaddr, len);

		// create child thread
		pthread_t tid;
		pthread_create(&pinfo->tid, NULL, func, pinfo);

		pthread_detach(pinfo->tid);
	}

	close(lfd);
	return 0;
}
