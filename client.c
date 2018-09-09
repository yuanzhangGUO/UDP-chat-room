#include <stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<sys/wait.h>


struct bar
{
	int port;
	char ip[128];
	char send[128];
	char recv[128];
}bao;
	
int main(int argc, char *argv[])
{   
	if(argc!=3)
	{
		printf("Usage: %s ip  port\n",argv[0]);
		return -1;
	}
	char buff[128] ={0};
	char buff1[128] ={0};
	int ret  = 0,ret1=0;
	int sockfd  = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd < 0)
	{
		perror("socket");
		return -1;
	}

	struct sockaddr_in  myaddr;
	myaddr.sin_family		= AF_INET;
	myaddr.sin_port			= htons(9999);
	myaddr.sin_addr.s_addr	= inet_addr("127.0.0.1");
	socklen_t len = sizeof(myaddr);

	struct sockaddr_in  addr;
	addr.sin_family		= AF_INET;
	addr.sin_port			= htons(atoi(argv[2]));
	addr.sin_addr.s_addr	= inet_addr(argv[1]);
	socklen_t len1 = sizeof(addr);
	pid_t pid=fork();
	if(pid > 0)
	{
	while(1)
	{
		printf("请输入聊天内容(输入quit退出）：\n");
		fgets(bao.recv,sizeof(bao.recv),stdin);
		if(strncmp(bao.recv,"\n",1)!=0)
		{
		int ret1 =	sendto(sockfd,&bao,sizeof(bao),0,(struct sockaddr *)&addr,len);
		if(strcmp(bao.recv,"quit\n") == 0) break;
		if(ret1 < 0)
		{
			perror("sendto");
			goto error;
		}
		}
		else
		{
		printf("聊天请认真对待，输入回车别人会不懂你的意思\n");
		}
	}
	}
	if(pid == 0)
	{
		while(1)
		{
		int ret2 = recvfrom(sockfd,&bao,sizeof(bao),0,(struct sockaddr *)&myaddr,&len);
		if(ret2 < 0)
		{
			perror("recvfrom");
			goto error;
		}
		if(strcmp(bao.send,"quit\n") == 0)
		{
		printf("ip 为 %s port 为 %d 离开群聊\n",bao.ip,bao.port);
		}
		else
		{
		printf("ip 为 %s port 为 %d 说：%s\n",bao.ip,bao.port,bao.send);
	   	}
		}
	}

	close(sockfd);

	return 0;

error:
	close(sockfd);
	return -1;
}
