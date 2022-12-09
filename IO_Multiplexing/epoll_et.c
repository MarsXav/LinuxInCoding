#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

// int epoll_create(int size);
// 	function: create a epoll entity, create 2 data structures in the kernel, one is to detect fds, the other is the ready list
// 	parameters: size, the argument can be ignored but must be greater than 0;
// 	returns -1 on fail, returns a fd on success
// int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
// 	function: manage epoll entity, like add fd info, delete info, modify info
// 	parameters:
// 		epfd: the fd epoll created
// 		op: operations needs to do
// 		fd: fd needs to be detected
// 		event: events to be taken
// 			EPOLLIN
// 			EPOLLET // edge triggered
// 			EPOLLOUT
// 			EPOLLERR
// int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
// 	parameters:
// 		epfd:
// 		events: fd info that changed stored in this structure
// 		maxevents: size of the second parameter
// 		timeout: block period
// 	return number of fds that changed on success, -1 on fail
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
	
	// envoke epoll_create to create an epoll entity
	int epfd = epoll_create(100);
	
	// add the listen fd to the epoll entity
	struct epoll_event epev;
	epev.events = EPOLLIN;
	epev.data.fd = lfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &epev);

	struct epoll_event epevs[1024];

	while(1) {
		// 
		int ret = epoll_wait(epfd, epevs, 1024, -1);
		if (ret == -1){perror("epoll_wait"); exit(-1);}	
		printf("number of files: %d\n", ret);
		for (int i = 0; i < ret; i++){
			if (epevs[i].data.fd == lfd){
			// clients detected	
				struct sockaddr_in cliaddr;
				int len = sizeof(cliaddr);
				int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &len);

				// set cfd as non block
				int flag = fcntl(cfd, F_GETFL);
				flag |= O_NONBLOCK;
				flag = fcntl(cfd, F_SETFL, flag);

				epev.events = EPOLLIN | EPOLLET; // added EPOLLET
				epev.data.fd = cfd;
				epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &epev);
			} else {
				if (epevs[i].events & EPOLLOUT) continue;

				char buf[5];
				int len = 0;
				while((len = read(epevs[i].data.fd, buf, sizeof(buf)))>0){
					printf("receive data : %s\n", buf);
				}
				if (len == 0) printf("client closed\n");
				else if (len == -1) {
					if (errno = EAGAIN) printf("data finished sent");
					else {
						perror("read");
						exit(-1);
					}
				}
				// data arrives
				//char buf[1024] = {0};
				//int len = read(epevs[i].data.fd, buf, sizeof(buf));
				//if (len == -1) {perror("read"); exit(-1);}
				//else if (len == 0){
				//	printf("client closed...\n");	
				//	epoll_ctl(epfd, EPOLL_CTL_DEL, epevs[i].data.fd, NULL);
				//	close(epevs[i].data.fd);
				//} else if (len > 0){
				//	printf("read buf = %s\n", buf);
				//	write(epevs[i].data.fd, buf, strlen(buf)+1);
			}
		}
	}
	close(lfd);
	close(epfd);

	return 0;
}
