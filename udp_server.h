/*===========================================================*\
  文件: udp_server.h
  版权: 青牛(北京)技术有限公司
  日期: 2006-05-09
  描述: 用于实现纯的C的代码的解扰, 提供一个封装过的UDP客户端
  参考:
  作者: zhangxl@channel.com
  修订记录:

\*===========================================================*/

#ifndef CHANNEL_UDP_SERVER_HEADER
#define CHANNEL_UDP_SERVER_HEADER




#ifdef __cplusplus
extern "C"{
#endif
/*========================================================================*/
#define DECRYPTION_LENGTH	(4096)
typedef unsigned long       DWORD;
typedef unsigned char       BYTE;
typedef int				    BOOL;



typedef struct{
	//unsigned int	socket;
	char			remote_host[1024];
	unsigned short  remote_port;
}remote_node;


typedef struct{
	unsigned int	socket;
	//char			server_host[1024];
	unsigned short  server_port;
	int  (*bind) (void *p, unsigned short port);
	int  (*client) (void *p);
	void (*close)   (void *p);
	int  (*send)   (void *p, const remote_node *remote,const char *buffer, int length);
	int  (*recv)    (void *p,remote_node *remote,char *buffer, int length);

}udp_server;



#ifdef _WIN32
void init_environment();
#endif
void build_address(remote_node *remote, const char *hostname,  unsigned short port);
udp_server * udp_server_new();
void udp_server_asign(udp_server *p);
void udp_server_delete(udp_server *p);
void udp_server_debug_out(unsigned char *p, int size);



/*========================================================================*/

#ifdef __cplusplus
}
#endif
#endif
