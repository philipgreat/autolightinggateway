


#include "serialib.h"
#include "udp_server.h"

#include <syslog.h>
#include <stdio.h>
/*
typedef struct str{
	char *name;
	int id;
} sss;

void sss_init(sss **s, char* name, int id)
{
	*s = (sss *) malloc(sizeof(sss));
	(*s)->name = name;
	(*s)->id = id;
}
*/


static void  logInfo(const char*message)
{

    openlog("udp-server", LOG_PID|LOG_CONS, LOG_USER);
    //syslog(LOG_INFO, message);
    closelog();

}

static int switchOnAllLights(serial *s)
{

	unsigned char buffer[128]={0x28,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x80,0x05,0x98,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x00,0x08,0x12,0x21,0x00,0x00,0x64,0x00,0x64,0x00,0x64,0x00,0x64,0x00,0x64,0x00,0xFC,0x63};
	serial_write(s, buffer, 40);
	//system("sleep 2");
	int length = serial_read(s, buffer, 128);

	return length;
}

static int switchOffAllLights(serial *s)
{

	unsigned char buffer[128]={0x28,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x08,0x00,
                                             0x00,0x00,0x80,0x05,0x98,0x00,0x00,0x00,0x00,0x00,
                                             0x00,0x1F,0x00,0x08,0x12,0x21,0x00,0x00,0x00,0x00,
                                             0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x57};
	serial_write(s, buffer, 40);
	//system("sleep 2");
	int length = serial_read(s, buffer, 128);

	return length;
}
const int BIND_PORT=8899;
const int MAX_RECEIVE_BUFFER_LENGTH = 1500;

const char *LIGH_ALL_ON="switchOnAllLights";
const char *LIGH_ALL_OFF="switchOffAllLights";


 int controlLightLoop()
 {

 	logInfo("start to run the light control\n");
	serial *serialPort;
	if (serial_open(&serialPort, "/dev/ttyUSB0", 38400) == 0){
		logInfo("Port opened with /dev/ttyUSB0, 38400 .\n");
	} else {
		logInfo("Problem with port opening\n");
		return -1;
	}

	udp_server * server=udp_server_new();

    server->bind(server,BIND_PORT);
    remote_node remote_client;
    char buffer[1500];//udp port
    while(1){
       int length=server->recv(server,&remote_client,buffer, MAX_RECEIVE_BUFFER_LENGTH);

        if(length<=0){
            continue;
        }
        buffer[length]=0;
        printf("received command: %s\n",buffer);
        if(strncmp(LIGH_ALL_ON,buffer,strlen(LIGH_ALL_ON))==0){
            switchOnAllLights(serialPort);
            continue;
        }
        if(strncmp(LIGH_ALL_OFF,buffer,strlen(LIGH_ALL_OFF))==0){
            switchOffAllLights(serialPort);
            continue;
        }
    }
    udp_server_delete(server);
	return 0;


 }


int testLight(int argc, char** argv)
 {




    if(argc<3){

        printf("Command supported lights 127.0.0.1 %s or lights 127.0.0.1 %s\n",LIGH_ALL_ON,LIGH_ALL_OFF);
        return 1;
    }

   printf("trying to send command '%s' to remote host: '%s'\n", argv[2],argv[1]);
	udp_server * server=udp_server_new();
	server->client(server);
    printf("wait..\n");
    remote_node remote;
    build_address(&remote,argv[1],BIND_PORT);
    int length2send=strlen(argv[2]);
        printf("wait..2 \n");
    int length=0;

    length=server->send(server,&remote, argv[2],length2send);

     printf("command '%s' sent length : %d\n", argv[2],length);
    return 0;
}

int main(int argc, char **argv)
{


  if(argc>1){
        return testLight(argc, argv);
        return 0;
    }

  return controlLightLoop();


}
