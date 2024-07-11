#include<netpacket/packet.h>
#include<string.h>
#include <sys/ioctl.h>
#include<linux/if.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netpacket/packet.h>
#include<net/ethernet.h>
#include<netinet/in.h>
#include<linux/if_ether.h>



int main(){


int sockfd, len;
char buffer[2048];
struct sockaddr_ll pla;


sockfd=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
struct ifreq req;
strcpy(req.ifr_name,"enp4s0");
int io = ioctl(sockfd,SIOCGIFINDEX,&req);
if(io<0)
printf("The interface eth0 does not exist");

//struct sockaddr_ll pla;
pla.sll_family=PF_PACKET;
pla.sll_protocol=htons(ETH_P_ALL);
pla.sll_ifindex=req.ifr_ifindex;
int b=bind(sockfd,(struct sockaddr *)&pla,sizeof(pla));
if(b<0)
perror("bind");


struct packet_mreq mr;
mr.mr_ifindex=req.ifr_ifindex;
mr.mr_type=PACKET_MR_PROMISC;
int s = setsockopt(sockfd,SOL_PACKET,PACKET_ADD_MEMBERSHIP,(void *)&mr,sizeof(mr));
if(s<0)
perror("Promiscuous Mode");



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
printf("For_Me %d \n",(pla.sll_protocol));
break;
case 1://PACKET_BROADCAST
printf("Broadcast %d \n",pla.sll_protocol);
break;
case 2://PACKET_MULTICAST
printf("Multicast %d \n",pla.sll_protocol);
break;
case 3://PACKET_OTHERHOST
printf("Other_Host %d \n",pla.sll_protocol);
break;
case 4://PACKET_OUTGOING
printf("Outgoing %d \n",pla.sll_protocol);
break;
case 5://PACKET_LOOPBACK
printf("Loop_Back %d \n",pla.sll_protocol);
break;
case 6://PACKET_FASTROUTE
printf("Fast_Route %d \n",pla.sll_protocol);
break;
}

 printf("Protocol: ");
        switch (ntohs(pla.sll_protocol)) {
            case ETH_P_IP:
                printf("IP\n");
                break;
            case ETH_P_ARP:
                printf("ARP\n");
                break;
            case ETH_P_IPV6:
                printf("IPv6\n");
                break;
            default:
                printf("Other (%x)\n", ntohs(pla.sll_protocol));
                break;
        }
}



}
