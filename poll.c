#include <poll.h>
#include <sys/time.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

// struct pollfd {int fd; short events;short revents}
// 	objects: 
// 		fd: fds kernal needs to check
// 		events: what events kernal needs to lokk into
// 		revents: actual events happened
// int poll(struct pollfd *fds, nfds_t nfds, int timeout);
// 	parameters:
// 		fds: a struct array of struct pollfd, this is a fd set of all files needs to be checked
// 		nfds:
// 		timeout: block time
// 	returns -1 on fail, number of changes on success
int main(){
	//create socket
	int lfd = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in saddr;
	saddr.sin_port = htons(9999);
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = INADDR_ANY;

	//bind
	bind(lfd, (struct sockaddr*)&saddr, sizeof(saddr));
	
	//listen
	listen(lfd, 8);

	// initialise fds
	struct pollfd fds[1024];
	for (int i = 0; i < 1024; i++){
		fds[i].fd = -1;
		fds[i].events = POLLIN;
	}
	fds[0].fd = lfd;
	int nfds = 0;
	
	while(1){
		//envoke select
		int ret = poll(fds, nfds+1, -1);
		if (ret == -1) {perror("poll"); exit(-1);}
		else {// modification of fd set detected
			if (fds[0].revents & POLLIN){//new clients connected
				struct sockaddr cliaddr;
				int len = sizeof(cliaddr);
				int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);		
				// add new fd to the fd set
				for (int i = 1; i < 1024; i++){
					if (fds[i].fd == -1){
						fds[i].fd = cfd;
						fds[i].events = POLLIN;
						break;
					}
				}
				// update maximuma fd index 
				nfds = nfds > cfd ? nfds : cfd;

			}
			for (int i = 1; i <=  nfds + 1; i++){
				if (fds[0].revents & POLLIN){
					//the client sends data
					char buf[1024] = {0};
					int len = read(fds[i].fd, buf, sizeof(buf));
					if (len == -1) {perror("read"); exit(-1);}
					else if (len == 0) {
						printf("client closed\n");
						close(i);
						fds[i].fd = -1;
					} else {
						printf("read buf = %s\n", buf);
						write(fds[i].fd, buf, strlen(buf + 1));	
					}
				}
			}
		}
	}
	close(lfd);
	return 0;
}
