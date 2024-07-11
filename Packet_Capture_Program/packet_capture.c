#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netpacket/packet.h>
#include<net/ethernet.h>
#include<netinet/in.h>

int main()
{
int sockfd, len;
char buffer[2048];
struct sockaddr_ll pla;
sockfd=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
if(sockfd<0)
{
perror("packet_socket");
exit(0);
}
printf("Types of the captured packets are...\n");

while(1)
{
len = sizeof(struct sockaddr_ll);
recvfrom(sockfd,buffer,sizeof(buffer),0,
(struct sockaddr *)&pla,&len);
switch(pla.sll_pkttype)
{
// these constant values are taken from linux/if_packet.h
case 0://PACKET_HOST
printf("For_Me\n");
break;
case 1://PACKET_BROADCAST
printf("Broadcast\n");
break;
case 2://PACKET_MULTICAST
printf("Multicast\n");
break;
case 3://PACKET_OTHERHOST
printf("Other_Host\n");
break;
case 4://PACKET_OUTGOING
printf("Outgoing\n");
break;
case 5://PACKET_LOOPBACK
printf("Loop_Back\n");
break;
case 6://PACKET_FASTROUTE
printf("Fast_Route\n");
break;
}}
}

