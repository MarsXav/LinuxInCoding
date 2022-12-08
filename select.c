#include <sys/time.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

// int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
//
// 	parameters:
// 		nfds: maximum number of the fd_set  + 1
// 		readfds: list of fds needs to be watched of if their read buffer has data
// 			normally watch read operation
// 			the corresponding operation is send operation by the sender
// 			afferent, outgoing parameter
// 		writefds: list of fds needs to be watched of if their write buffer has empty space remaining
// 		exceptfds: detect unusual fds
// 		timeout: set timeout
// 			NULL - forever blocked, until a change in fds
// 			tv_sec = 0, tv_usec = 0 - nonblock
// 			tv_sec > 0, tv_usec > 0 - block period
// 		return: -1 on fail, return number of fds with changes made
//
// void FD_CLR(int fd, fd_set *set); 
// 	function: bezero all fd flags
// 
// void FD_ISSET(int fd, fd_set *set);
// 	function: identify if the flag of fd is true or false
//
// void FD_SET(int fd, fd_set *set);
// 	function: set the fd flag to 1
//
// void FD_ZERO(fd_set *set);
// 	function: bezero all bits of the fd set to 0
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
	
	// create a fd_set, with the fds needs to be detected
	fd_set rdset, temp;
	FD_ZERO(&rdset);//bezero
	FD_SET(lfd,&rdset);
	int maxfd = lfd;

	while(1){
		temp = rdset;
		//envoke select
		int ret = select(maxfd + 1, &temp, NULL, NULL, NULL);
		if (ret == -1) {perror("select"); exit(0);}
		else {// modification of fd set detected
			if (FD_ISSET(lfd, &temp)){//new clients connected
				struct sockaddr cliaddr;
				int len = sizeof(cliaddr);
				int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);		
				// add new fd to the fd set
				FD_SET(cfd, &rdset);
				// update maximum fd
				maxfd = maxfd > cfd ? maxfd : cfd;

			}
			for (int i = lfd + 1; i <=  maxfd + 1; i++){
				if (FD_ISSET(i, &rdset)){
					//the client sends data
					char buf[1024] = {0};
					int len = read(i, buf, sizeof(buf));
					if (len == -1) {perror("read"); exit(0);}
					else if (len == 0) {
						printf("client closed\n");
						FD_CLR(i, &rdset);
						close(i);
					} else {
						printf("read buf = %s\n", buf);
						write(i, buf, strlen(buf + 1));	
					}
				}
			}
		}
	}
	close(lfd);
	return 0;
}
