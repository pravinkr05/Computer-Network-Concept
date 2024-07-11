#include <string.h>
#include<stdlib.h>
#include <fcntl.h>
#include<stdio.h>
#include<unistd.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <time.h>
#include <sys/types.h>
#define MAXLEN 100
#define PORT 12345

 
int
main (int argc, char **argv)
{
  
int s, connfd, len, n;
  
char str[100];
  
struct sockaddr_in saddr, caddr;
  
 
s = socket (AF_INET, SOCK_STREAM, 0);
  
 
if (s < 0)
    
 
    {
      
 
perror ("socket");
      
 
exit (0);
    
 
}
  
 
saddr.sin_family = AF_INET;
  
saddr.sin_addr.s_addr = htonl (INADDR_ANY);
  
saddr.sin_port = htons (PORT);
  
if (bind (s, (struct sockaddr *) &saddr, sizeof (saddr)) < 0)
    
 
    {
      
perror ("bind");
      
exit (0);
    
 
}
  
 
listen (s, 5);
  
 
int n_client = 0;
  
int i = 1;
  
while (i)
    {
      
printf ("\nWaiting for request...\n");
      
int len = sizeof (struct sockaddr_in);
      
int connfd = accept (s, (struct sockaddr *) &caddr, &len);
      
printf ("\nACCEPTED\n");
      
printf ("\nREQUEST FROM %s PORT %d\n",
	       inet_ntop (AF_INET, &caddr.sin_addr, str, sizeof (str)),
	       htons (caddr.sin_port));
      
n_client++;
      
time_t currentTime;
      
time (&currentTime);
      
printf ("Client %d requested for time at %s", n_client,
	       ctime (&currentTime));
      
send (connfd, ctime (&currentTime), 30, 0);
    
}

}
