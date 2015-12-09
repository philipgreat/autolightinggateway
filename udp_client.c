/*===========================================================*\
  文件: udp_client.c	
  版权: 青牛(北京)技术有限公司
  日期: 2006-05-09
  描述: 提供一个封装过的UDP客户端, 接收为select模型, 进行超时控制, 
  参考: 
  作者: zhangxl@channel.com
  修订记录:	

\*===========================================================*/
#include "udp_client.h"



#ifdef  _WIN32
#pragma comment(lib,"ws2_32")
#include <winsock2.h>
#include <windows.h>
#include <winsock.h>
#else
/*berkerly and unix/unix-like systems*/
#include<errno.h>
#include<netdb.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h> 
#include<sys/wait.h> 
#include<sys/types.h>
#endif

#include <stdlib.h>
#include <string.h>



static void udp_client_build_addr(struct sockaddr_in * server,
								  const char *host, 
								  unsigned short port);
static int udp_client_connect(void *p,const char *host, unsigned short port);
static void udp_client_close(void *p);
static int udp_client_send(void *p,const char *buffer, int length);
static int udp_client_recv(void *p,char *buffer, int length);

#ifdef _WIN32
void init_environment()
{
	WSADATA  wsaData;
	WSAStartup(0x101, &wsaData);

}
#endif
udp_client_t * udp_client_new()
{
	udp_client_t *p=(udp_client_t *)malloc(sizeof(udp_client_t));
	if(p == NULL) return NULL;
	p->connect = udp_client_connect;
	p->close = udp_client_close;
	p->send = udp_client_send;
	p->recv = udp_client_recv;

	return p;

}
void udp_client_asign(udp_client_t *p)
{
	if(p==NULL){
		return;
	}
	p->connect = udp_client_connect;
	p->close = udp_client_close;
	p->send = udp_client_send;
	p->recv = udp_client_recv;



}
static int udp_client_connect(void *p,const char *host, unsigned short port)
{

	udp_client_t *this = (udp_client_t *)p;
	struct   sockaddr_in server;

	if(strlen(host)+1>1024){
		printf("tooooo long the host name\n");
		return -1;
	}
	this->server_port = port;
	strncpy(this->server_host,host,strlen(host)+1);
	udp_client_build_addr(&server,host,port);

	this->socket = socket(AF_INET, SOCK_DGRAM, 0);

	return connect(this->socket,(struct sockaddr*)&server,sizeof(server));

}

void udp_client_delete(udp_client_t *p){
	
	free(p);
}

static int udp_client_send(void *p,const char *buffer, int length)
{
	int ret = 0;
	udp_client_t *this = (udp_client_t *)p;


	struct sockaddr_in server;	
	udp_client_build_addr(&server,this->server_host,this->server_port);

	ret=sendto(this->socket, (char *)buffer, length, 0, 
		(struct sockaddr *)&server,
		sizeof(server));

	return ret;
}

static int udp_client_recv(void *p,char *buffer, int length)
{

	udp_client_t *this = (udp_client_t *)p;
	struct sockaddr_in server;

	int n = 0;
	unsigned int fromlen = 0;
	int result = 0;
	struct timeval tv;

	fd_set writefs ;
	FD_ZERO(&writefs);
	FD_SET(this->socket, &writefs);
	tv.tv_sec=1;
	tv.tv_usec=0;
	
	result = select(this->socket+1, &readfds, (fd_set*)NULL, (fd_set*)NULL, &tv);	
	//this mechanism works well under ce, setsockopt does not work well under 
	//berkely options ;)
	if(result>0){
		fromlen=sizeof(struct sockaddr);	
		udp_client_build_addr(&server,this->server_host,this->server_port);
		n=recvfrom(this->socket, buffer, length, 0, (struct sockaddr *)&server, &fromlen);
		//printf(" recv clientsocket = %d server port = %d\n",this->clientSocket,serverAddr.sin_port);
		return n;
	}
	if(result ==0){
		//timeout
		printf("wait time out n = %d \n",result );
		return -1;
	}
	return -2;


}


static void udp_client_close(void *p)
{
	udp_client_t *this = (udp_client_t *)p;
#ifdef _WIN32
	closesocket(this->socket);
#else
	close(this->socket);
#endif

}


static void udp_client_build_addr(struct sockaddr_in * server,const char *host, unsigned short port)
{
	server->sin_addr.s_addr = inet_addr(host);
	server->sin_family = AF_INET;
	server->sin_port   = htons(port);
}

