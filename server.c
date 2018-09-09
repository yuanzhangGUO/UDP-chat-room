#include <stdio.h>
#include <stdlib.h>
#include "head.h"

struct bar
{
	int port;
	char ip[128];
	char send[128];
	char recv[128];
}bao;



typedef struct node
{
	int port;
	char ip[128];
	struct node *next;
}node_t;
node_t *linklistcyc_node_init(node_t *head);
node_t *linklistcyc_node_add_tail(node_t *head,int val,char *ch);
int linklistcyc_del_val(node_t *head,int val);
int linklistcyc_check(node_t *head,int val);
void linklistcyc_send(node_t *head,int *sockfd);
void linklistcyc_node_gai(node_t *head,int val);


node_t *linklistcyc_node_init(node_t *head)
{
	head->port        = 0;
	bzero(head->ip,sizeof(head->ip));
	head->next      = head;

	return head;
}

node_t *linklistcyc_node_add_tail(node_t *head,int val,char *ch)
{
	node_t *new = (node_t*)malloc(sizeof(node_t));
	new->port     = val;
	strcpy(new->ip,ch);
	node_t *cur = head;
	for(;cur->next!=head;cur=cur->next)
	{}
	new->next = head;
	cur->next = new;

	return new;	
}

int linklistcyc_del_val(node_t *head,int val)
{
	node_t *prev = head;
	node_t *cur  = head->next;
	while(cur != head)
	{
		if(cur->port ==val)
		{
			prev->next = cur->next;
			free(cur);
			return 1;
		}
		else
		{
			prev = cur;
			cur  = cur->next;
		}
	}
	return 0;
}

void linklistcyc_send(node_t *head,int *sockfd)
{
	node_t *cur = head->next;
	int i=0;
	while(cur != head)
	{

		struct sockaddr_in  claddr;
		claddr.sin_family		= AF_INET;
		claddr.sin_port			= htons(cur->port);
		claddr.sin_addr.s_addr	= inet_addr(cur->ip);
		socklen_t len1 = sizeof(claddr);
		printf("port=%d  ip= %s \n",cur->port,cur->ip);
		strcpy(bao.send,bao.recv);
		int ret1=sendto(*sockfd,&bao,sizeof(bao),0,(struct sockaddr*)&claddr,len1);
		i++;
		if(ret1 < 0)
		{
			perror("recvfrom ");
			break;
		}
		cur = cur->next;
	}
	printf("i=%d\n",i);
}

int linklistcyc_check(node_t *head,int val)
{
	node_t *cur = head->next;
	while(cur != head)
	{
		if(cur->port == val) return 0;
		cur = cur->next;
	}
	return 1;
}

int main(int argc, char *argv[])
{
	node_t head;
	linklistcyc_node_init(&head);
	if(argc != 3)
	{
		printf("Usage: %s ip port\n",argv[0]);
		return -1;
	}
	int ret  = 0;
	int ret1  = 0;
	int sockfd  = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd < 0)
	{
		perror("socket");
		return -1;
	}

	struct sockaddr_in  myaddr,claddr;
	myaddr.sin_family		= AF_INET;
	myaddr.sin_port			= htons(atoi(argv[2]));
	myaddr.sin_addr.s_addr	= inet_addr(argv[1]);
	socklen_t len = sizeof(myaddr);
	socklen_t len1 = sizeof(claddr);

	ret  = bind(sockfd,(struct sockaddr *)&myaddr,len);
	if(ret < 0)
	{
		perror("bind");
		goto  error;
	}
	while(1)
	{
		ret  = recvfrom(sockfd,&bao,sizeof(bao),0,
				(struct sockaddr*)&claddr,&len1);
		printf("%s\n",bao.recv);
		if(ret < 0)
		{
			perror("recvfrom ");
			goto error;
		}
		else
		{
			int chk = linklistcyc_check(&head,ntohs(claddr.sin_port));
			if(chk == 1)
			{
				printf("port = %d 已登录\nbuff = %s\n",
						ntohs(claddr.sin_port),bao.recv);
				linklistcyc_node_add_tail(&head,ntohs(claddr.sin_port),inet_ntoa(claddr.sin_addr));
			}
			else
			{
				printf("port = %d say: %s\n",
						ntohs(claddr.sin_port),bao.recv);
			}
			bao.port = ntohs(claddr.sin_port); 
			strcpy(bao.ip,inet_ntoa(claddr.sin_addr));
			linklistcyc_send(&head,&sockfd);
			if((strcmp(bao.recv,"quit\n"))==0)
			linklistcyc_del_val(&head,ntohs(claddr.sin_port));
			
		}
	}
	close(sockfd);
	return 0;

error:
	close(sockfd);
	return -1;
}
