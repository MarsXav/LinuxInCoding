#include <stdio.h>
#include <arpa/inet.h>

// When internet communication, byte order needs to be converted from host byte order to IP byte order
// The other end converts to its host byte order according to needs
//
// convert port
// unit16_t htons(unit16_t hostshort); 
// unit16_t ntohs(unit16_t netshort);
//
//convert IP
// unit32_t htonl(unit32_t hostlong);
// uint32_t ntohl(uint32_t netlong); 
int main(){
	//htons
	unsigned short a = 0x0102;
	unsigned short b = htons(a);
	printf("%x\n", b);

	//convert IP

	//htonl
	char buf[4] = {192, 168, 1, 1};
	int num = *(int*) buf;
	unsigned int sum = htonl(num);
	unsigned char *p = (char*)&sum;

	printf("%d %d %d %d\n", *p, *(p+1), *(p+2), *(p+3));

	//ntohs
	unsigned char buf1[4] = {1, 1, 168, 192};
	int num1 = *(int*)buf1;
	int sum1 = ntohl(num1);
	unsigned char *p1 = (unsigned char*)&sum1;
	printf("%d %d %d %d\n", *p1, *(p1+1), *(p1+2), *(p1+3));
	
	return 0;	
}
