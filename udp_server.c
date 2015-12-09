/*===========================================================*\
  文件: udp_server.c
  版权: 青牛(北京)技术有限公司
  日期: 2006-05-09
  描述: 提供一个封装过的UDP客户端, 接收为select模型, 进行超时控制,
  参考:
  作者: zhangxl@channel.com
  修订记录:

\*===========================================================*/
#include "udp_server.h"

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
#include <stdio.h>
#include <syslog.h>

static int udp_server_bind(void *p, unsigned short port);
static void udp_any_addr(struct sockaddr_in * server, unsigned short port);
static void udp_host_addr(struct sockaddr_in * server,const char * hostname, unsigned short port);
//static int udp_server_connect(void *p,const char *host, unsigned short port);
static void udp_server_close(void *p);
static int udp_server_send(void *p,const remote_node *remote,const char *buffer, int length);
static int udp_server_recv(void *p,remote_node *remote,char *buffer, int length);
static int udp_server_client(void *p);

void build_address(remote_node *remote, const char *hostname,  unsigned short port)
{
    strncpy(remote->remote_host,hostname,1023);
    remote->remote_port=port;
}


static void  logError(const char*message)
{

    openlog("udp-server", LOG_PID|LOG_CONS, LOG_USER);
    syslog(LOG_ERR, message);
    closelog();

}

static void  errExit(const char*message)
{

    openlog("slog", LOG_PID|LOG_CONS, LOG_USER);
    syslog(LOG_ERR, message);
    closelog();
    exit(1);

}

#ifdef _WIN32
void init_environment()
{
	WSADATA  wsaData;
	WSAStartup(0x101, &wsaData);

}
#endif
udp_server * udp_server_new()
{
	udp_server *p=(udp_server *)malloc(sizeof(udp_server));
	if(p==NULL){
        errExit("Not able to malloc for the udp server");
	}
	p->socket=-1;
	p->client = udp_server_client;
	p->bind = udp_server_bind;
	p->close = udp_server_close;
	p->send = udp_server_send;
	p->recv = udp_server_recv;

	return p;

}
void udp_server_asign(udp_server *p)
{
	if(p==NULL){
        errExit("Not able to assign values for NULL pointer");
		return;
	}
    //p->socket=-1;
    p->client = udp_server_client;
	p->bind = udp_server_bind;
	p->close = udp_server_close;
	p->send = udp_server_send;
	p->recv = udp_server_recv;


}



static int udp_server_client(void *p)
{

	if(p==NULL){
        errExit("Not able to operate for NULL pointer when bind()");
		return -1;
	}
	udp_server *this = (udp_server *)p;
	this->socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(this->socket<0){
		printf("The socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); execute failed, it returns: %d\n", this->socket);
		return -1;
	}
	return 0;

}

static int udp_server_bind(void *p, unsigned short port)
{

	if(p==NULL){
        errExit("Not able to operate for NULL pointer when bind()");
		return -1;
	}
	udp_server *this = (udp_server *)p;
	struct   sockaddr_in server;


	this->server_port = port;
	//strncpy(this->server_host,host,strlen(host)+1);
	udp_any_addr(&server,port);

	this->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(this->socket<0){
		printf("The socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); execute failed, it returns: %d\n", this->socket);
		return -1;
	}
	return bind(this->socket,(struct sockaddr*)&server,sizeof(server));

}



void udp_server_delete(udp_server *p){

	if(p==NULL){
        errExit("Not able to free for NULL pointer when delete()");
		return;
	}
	free(p);
}

 static int udp_server_send(void *p,const remote_node *remote,const char *buffer, int length)
{
    //printf("0..............\n");
	if(p==NULL){
       errExit("Not able to free for NULL pointer when udp_server_send()");
		return -1;
	}

	udp_server *this = (udp_server *)p;

    //printf("1\n");

	struct sockaddr_in client;
	//udp_server_build_addr(&server,this->server_host,this->server_port);
	udp_host_addr(&client, remote->remote_host,remote->remote_port);
   //printf("2\n");


   // printf("trying to send '%s' with length %d with sock %d\n",buffer,length,this->socket);

	int ret=sendto(this->socket, (char *)buffer, length, 0,
		(struct sockaddr *)&client,
		sizeof(client));

	return ret;
}

static int udp_server_recv(void *p,remote_node *remote,char *buffer, int length)
{

	if(p==NULL){
        errExit("Not able to free for NULL pointer when udp_server_recv()");
		return -1;
	}
    if(remote==NULL){
      errExit("Not able to assign address info after received!");
		return -1;
	}

	if(length==0){
      logError("Not able to assign address info after received!");
		return -1;
	}

	udp_server *this = (udp_server *)p;


	struct timeval tv;

	fd_set writefs ;
	FD_ZERO(&writefs);
	FD_SET(this->socket, &writefs);
	tv.tv_sec=1;
	tv.tv_usec=0;


	struct sockaddr_in client;

	int result = select(this->socket+1, &writefs, (fd_set*)NULL, (fd_set*)NULL, &tv);
	//this mechanism works well under ce, setsockopt does not work well under
	//berkely options ;)
	if(result>0){

		socklen_t fromlen=sizeof(struct sockaddr);

		//udp_host_addr(&client);
		//printf("code here %d\n",length);
		int  received_length=recvfrom(this->socket, buffer, length, 0, (struct sockaddr *)&client, &fromlen);

		char *remote_ip = inet_ntoa(client.sin_addr);
        strncpy(remote->remote_host,remote_ip, 1024);
		//printf(" recv clientsocket = %d server port = %d\n",this->clientSocket,serverAddr.sin_port);
		remote->remote_port=ntohs(client.sin_port);
		return received_length;
	}
	if(result ==0){
		return -1;
	}
	return -2;


}


static void udp_server_close(void *p)
{
	udp_server *this = (udp_server *)p;
#ifdef _WIN32
	closesocket(this->socket);
#else
	close(this->socket);
#endif

}



static void udp_any_addr(struct sockaddr_in * server, unsigned short port)
{

    memset(server,0,sizeof(struct sockaddr_in));
	server->sin_addr.s_addr = htonl(INADDR_ANY);
	server->sin_family = AF_INET;
	server->sin_port   = htons(port);
}

static void udp_host_addr(struct sockaddr_in * server,const char * hostname, unsigned short port)
{

    memset(server,0,sizeof(struct sockaddr_in));
	//struct hostent *hostinfo;
   //name->sin_addr = *(struct in_addr *) hostinfo->h_asddr;
    server->sin_addr.s_addr = inet_addr(hostname);
	//server->sin_addr.s_addr = hostinfo->h_addr;
	//memcpy(&server->sin_addr, hostinfo->h_addr_list[0], hostinfo->h_length);
	server->sin_family = AF_INET;
	server->sin_port   = htons(port);
	//server->sin_port   = port;

}



