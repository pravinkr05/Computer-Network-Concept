#include <string.h>
#include<stdlib.h>
#include <fcntl.h>
#include<stdio.h>
#include<unistd.h>
#include <sys/stat.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <sys/types.h>
#include <time.h>
#define MAXLEN 100
#define PORT 12345

int main(int argc, char **argv)
{
 char response[30];
int s;
struct sockaddr_in saddr;
if(argc!=2)
{
printf("Usage:%s <Server’s address> \n",argv[0]);
exit(0);
}
s = socket(AF_INET, SOCK_STREAM, 0);
if(s<0)
{
perror("socket");
exit(0);
}
saddr.sin_family=AF_INET;
saddr.sin_port=htons(PORT);
if(inet_pton(AF_INET,argv[1],&saddr.sin_addr)<=0)
{
printf("Error. Invalid IP address\n");
exit(0);
}
 connect(s, (struct sockaddr*)&saddr, sizeof(saddr));
 printf("connected successfully\n");
recv(s, response, 29, 0);
 printf("Time from server: %s\n", response);

close(s);
}

