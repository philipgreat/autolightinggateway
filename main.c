#include "serialib.h"
#include "udp_server.h"
#include <unistd.h>
#include <syslog.h>
#include <stdio.h>

static void  logInfo(const char*message)
{

    openlog("udp-server", LOG_PID|LOG_CONS, LOG_USER);
    //syslog(LOG_INFO, message);
    printf("%s",message);
    closelog();

}

static void banner(){

	printf("\n\n\nBUILD INFO %s@%s@%s\n\n",__FILE__,__DATE__,__TIME__);

}

static int initNetwork(serial *s)
{
	banner();	

	unsigned char buffer[128]={0x0A,0x00,0x80,0x0A,0x00,0x00,0x00,0x00,0xFF,0x6C};

	printf("initNetwork\n");
        int writeLength = serial_write(s, buffer, 10);
	printf("writes '%d'\n",writeLength);
        //sleep(10);
	
        int length = serial_read(s, buffer, 40);
	printf("returns '%d'\n",length);
	for(int i=0;i<length;i++){
		printf(" %02X",buffer[i]);
	}
        return length;
	
	return 0;
}



 int controlLightLoop(char *port)
 {
	printf("trying to open port %s ",port);
 	logInfo("start to run the light control\n");
	serial *serialPort;
	if (serial_open(&serialPort, port, 38400) == 0){
		printf("Port opened with %s , 38400 .\n",port);
	} else {
		logInfo("Problem with port opening\n");
		return -1;
	}
	initNetwork(serialPort);
	return 0;

 }



int main(int argc, char **argv)
{
   banner();


  return controlLightLoop(argv[1]);


}


