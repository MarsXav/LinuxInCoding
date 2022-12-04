#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <wait.h>

void recycleChild(int arg){
	while(1){// make sure to caputre all the signals
		int ret = waitpid(-1, NULL, WNOHANG);
		if (ret == -1) break;		
		if (ret == 0) break; // child process still remains
		if (ret > 0) {
			printf("child process %d is recycled\n", ret);
		}
	}
}


int main(){
	// signal capture
	struct sigaction act;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	act.sa_handler = recycleChild;

	sigaction(SIGCHLD, &act, NULL);
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
	
	// pending client connections
	while(1){
		struct sockaddr_in clientaddr;
		int len = sizeof(clientaddr);
		int cfd = accept(lfd, (struct sockaddr*)&clientaddr, &len);	
		if (errno == EINTR) continue;
		if (cfd == -1) {perror("accept"); exit(-1);}
		
		// every connection established, a child process will be created to communicate with the server
		pid_t pid = fork();
		if (pid == 0){
			// child process
			//obtain server info
			char clientIP[16];
			inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
			unsigned short clientPort = ntohs(clientaddr.sin_addr.s_addr);
			printf("chilent ip is : %s, port is %d\n", clientIP, clientPort);

			// receive data sent from the server
			char buf[1024] = {0};
			while(1) {
				len = read(cfd, &buf, sizeof(buf));
				if (len == -1){perror("read"); exit(-1);}
				else if (len > 0) printf("receive client: %s\n", buf);
				else if (len == 0) printf("client closed.. \n");

				write(cfd, buf, strlen(buf));
			}
			close(cfd);
			exit(0); // exit current child process
		}
	}
	close(lfd);
	return 0;
}
