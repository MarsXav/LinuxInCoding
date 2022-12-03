#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

// int socket(int domain, int type, int protocol);
// 	function: create a socket
// 	parameters:
// 		domain protocal family:
// 			AF_UNIX, AF_LOCAL,(local, IPC) AF_INET(ipv4), AF_INET6(ipv6)
// 		type: specifies the communication semantics
// 			SOCK_STREAM: stream protocal
// 			SOCK_DGRAM: datagram protocal	
// 		protocol: specifies the specific protocol
// 			0 when only one protocol is used
// 		returns a file descriptor for the new socket, -1 on fail
////////////////////////////////////////////////////////////////////////////////////
// int bind(int socketfd, const struct sockaddr *addr, socklen_t addrlen);
// 	function: bind the fd and local IP and port
// 	parameters:
// 		sockfd: fd obtained from socket function
// 		addr: socket address needs to be binded, which contains ip and port info
// 		addrlen: memory usage of the second parameter
// 	returns 0 on success
///////////////////////////////////////////////////////////////////////////////////
// int listen(int sockfd, int backlog);
// 	function: listen if a client arrives
//	parameters:
//		sockfd: fd obtained from socket function
//		backlog: defines the maximum length to which the queue of pending connections for sockfd may grow, often set as 5
///////////////////////////////////////////////////////////////////////////////////
// int accept(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
// 	function: receive connection from server, on default blocked
// 	parameters:
// 		sockfd
// 		addr: records the client address after succesfully connects
// 		addrlen: assign the memory usage of the second parameter
// 	returns file descriptor for communication on success, -1 on fail
///////////////////////////////////////////////////////////////////////////////////
// int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
// 	function: connects the server to the client
//	parameters: 
//		sockfd
//		addr: address of the server clients needs to be conneted
//		addrlen
//////////////////////////////////////////////////////////////////////////////////
// ssize_t write(int fd, const void *buf, size_t count); //write data
// ssize_t read(int fd, void* buf, size_t count); //read data

// TCP server end
int main(){
	// 1. create a socket
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (lfd == -1) {perror("socket"); exit(-1);}
	
	// 2. bind
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	//inet_pton(AF_INET, "127.0.0.1", saddr.sin_addr.s_addr);
	saddr.sin_addr.s_addr = INADDR_ANY; 
	saddr.sin_port = htons(9999);
	int ret = bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));
	if (ret == -1) {perror("bind"); exit(-1);}

	// 3. listen
	ret = listen(lfd, 8);
	if (ret == -1) {perror("listen"); exit(-1);}

	//4. connect with client
	struct sockaddr_in clientaddr; 
	socklen_t len = sizeof(clientaddr);
	int cfd = accept(lfd, (struct sockaddr*)&clientaddr, &len);
	if (cfd == -1) {perror("accept"); exit(-1);}

	//export client info
	char clientIP[16];
	inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
	unsigned short clientPort = ntohs(clientaddr.sin_port);

	printf("client ip is %s, port is %d\n", clientIP, clientPort);

	// 5. Communication
	// Obtain client info
	char buf[1024] = {0};
	ret = read(cfd, buf, sizeof(buf));
	if (ret == -1){
		perror("read");
		exit(-1);
	} else if (ret > 0) {
		printf("receive client data : %s\n", buf);
	} else if (ret == 0){
		printf("client closed");
	}
	
	// send data to the server
	char *data = "hello";
	write (cfd, data, strlen(data));

	// close fd
	close(cfd); close(lfd);


	return 0;
}
