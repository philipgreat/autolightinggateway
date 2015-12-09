/*===========================================================*\
  �ļ�: udp_client.h	
  ��Ȩ: ��ţ(����)�������޹�˾
  ����: 2006-05-09
  ����: ����ʵ�ִ���C�Ĵ���Ľ���, �ṩһ����װ����UDP�ͻ���
  �ο�: 
  ����: zhangxl@channel.com
  �޶���¼:	

\*===========================================================*/

#ifndef CHANNEL_UDP_CLIENT_HEADER
#define CHANNEL_UDP_CLIENT_HEADER
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"{
#endif
/*========================================================================*/
#define DECRYPTION_LENGTH	(4096)
typedef unsigned long       DWORD;
typedef unsigned char       BYTE;
typedef int				    BOOL;

typedef struct{
	unsigned int	socket;
	char			server_host[1024];
	unsigned short  server_port;
	int  (*connect) (void *p, const char *host, unsigned short port);
	void (*close)   (void *p);
	int  (*send)    (void *p, const char *buffer, int length);
	int  (*recv)    (void *p, char *buffer, int length);

}udp_client_t;

#ifdef _WIN32
void init_environment();
#endif

udp_client_t * udp_client_new();
void udp_client_asign(udp_client_t *p);
void udp_client_delete(udp_client_t *p);
void udp_client_debug_out(unsigned char *p, int size);



/*========================================================================*/

#ifdef __cplusplus
}
#endif
#endif
