#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>

// ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr* dst_addr, socklen_t addrlen);
//	parameters:
//		sockfd: socket fd
//		buf: data to be sent
//		len: length of the buf
//		flags: nomally 0
//		dst_addr: the address info of the receiver
//		addrlen: length of the destination address
// 	returns the number successfully sent on success
//
// ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
// 	parameters: 
// 		sockfd
// 		buf: buf to receive the data
// 		len: length of the buf
// 		flags: 0
// 		src_addr: store the adderss info of the sender
// 		addrlen: 
// 	returns bytes of data received on success
//
int main(){
	// create a socket
	int fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (fd == -1) {perror("socket"); exit(-1);}

	//bind
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9999);
	addr.sin_addr.s_addr = INADDR_ANY;
	int ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	if (ret == -1) {perror("bind"); exit(-1);}
	
	// comminicate
	while(1){
		// receive
		char buf[128], ipbuf[16];
		struct sockaddr_in cliaddr;
		int len = sizeof(cliaddr);
		int num = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr*)&cliaddr, &len);	
		
		printf("client IP: %s, Port: %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ipbuf, sizeof(ipbuf)), ntohs(cliaddr.sin_port));
		printf("client data: %s\n", buf);
	
		// send data
		sendto(fd, buf, strlen(buf)+1, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
	}
	close(fd);

	return 0;

}
