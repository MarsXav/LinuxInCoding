#include <arpa/inet.h>
#include <stdio.h>

// p:point-number IP string, n: network, integer of internet byte order
// int inet_pton(int af, const char *src, void *dst);
// 	af: adress family, AF_INET AF_INET6
// 	src: IP string that needs to be converted
// 	dst: the results of conversion stores in this
//
// convert IPv4 and IPv6 to IP string
// const char* inet_ntop(int af, const void *src, char*dst, socklen_t size);
// 	af
// 	src: ip integers needs to be converted
// 	dst: result string
// 	size: size of the third parameter
// 	returns the adress of the data after conversion
int main(){
	//create a ip string
	char buf[] = "192.168.1.5";
	unsigned int num = 0;
	int ret = inet_pton(AF_INET, buf, &num);
	
	unsigned char* p = (unsigned char*)&num;
	printf("ip: %d %d %d %d\n", *p, *(p+1), *(p+2), *(p+3));
	
	//convert the IP integers to point-number IP string
	char ip[16] = "";
	const char* str = inet_ntop(AF_INET, &num, ip, 16);
	printf("str: %s\n", str);

	return 0;
}
