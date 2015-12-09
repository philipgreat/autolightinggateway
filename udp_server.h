/*===========================================================*\
  �ļ�: udp_server.h
  ��Ȩ: ��ţ(����)�������޹�˾
  ����: 2006-05-09
  ����: ����ʵ�ִ���C�Ĵ���Ľ���, �ṩһ����װ����UDP�ͻ���
  �ο�:
  ����: zhangxl@channel.com
  �޶���¼:

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
