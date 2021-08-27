#include "serialib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>                 //Used for UART
#include <fcntl.h>                  //Used for UART
#include <termios.h>                //Used for UART
#include <errno.h>




static int uart_init(char *port){
    //O_RDWR | O_NOCTTY | O_NDELAY
    int uart_fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

    if(uart_fd == -1){
        fprintf(stderr, "Failed to open uart.\n");
        return -1;
    }
	
    int flags = fcntl(uart_fd, F_GETFL);
    flags &= ~O_NDELAY;
    fcntl(uart_fd, F_SETFL, flags);
    if (tcflush(uart_fd, TCIOFLUSH) < 0) {
        printf("tcflush() %d %s\n", errno, strerror(errno));
        return -1;
    }

    struct termios settings;
    tcgetattr(uart_fd, &settings);

        settings.c_cflag |= (CREAD | CLOCAL);
	settings.c_iflag = IGNPAR;
	settings.c_oflag = 0;
	settings.c_lflag = 0;
	settings.c_cc[VINTR]  = _POSIX_VDISABLE;
	settings.c_cc[VQUIT]  = _POSIX_VDISABLE;
	settings.c_cc[VSTART] = _POSIX_VDISABLE;
	settings.c_cc[VSTOP]  = _POSIX_VDISABLE;
	settings.c_cc[VSUSP]  = _POSIX_VDISABLE;
	settings.c_cc[VEOF]   = _POSIX_VDISABLE;
	settings.c_cc[VEOL]   = _POSIX_VDISABLE;
	settings.c_cc[VERASE] = _POSIX_VDISABLE;
	settings.c_cc[VKILL]  = _POSIX_VDISABLE;
        settings.c_cc[VTIME]=10;
        settings.c_cc[VMIN]=0;

    //tcsetattr(uart_fd, &settings);

    /*  CONFIGURE THE UART
    *  The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
    *       Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
    *       CSIZE:- CS5, CS6, CS7, CS8
    *       CLOCAL - Ignore modem status lines
    *       CREAD - Enable receiver
    *       IGNPAR = Ignore characters with parity errors
    *       ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
    *       PARENB - Parity enable
    *       PARODD - Odd parity (else even) */

    /* Set baud rate - default set to 9600Hz */

    /* Baud rate fuctions
    * cfsetospeed - Set output speed
    * cfsetispeed - Set input speed
    * cfsetspeed  - Set both output and input speed */

    //cfsetspeed(&settings, baud_rate);
	
    cfsetispeed(&settings, B38400);
    cfsetospeed(&settings, B38400);

    //settings.c_cflag &= ~PARENB; /* no parity */
    //settings.c_cflag &= ~CSTOPB; /* 1 stop bit */
    //settings.c_cflag &= ~CSIZE;
    //settings.c_cflag |= CS8; // | CLOCAL; /* 8 bits */
    //settings.c_lflag = ICANON; /* canonical mode */
    //settings.c_oflag &= ~OPOST; /* raw output */
    tcflush(uart_fd, TCIFLUSH);

    /* Setting attributes */
    tcflush(uart_fd, TCIFLUSH);
    tcsetattr(uart_fd, TCSANOW, &settings);


	




    return uart_fd;
}


int serial_open(serial **s, char* port, const unsigned int baud){

	 *s = (serial *) malloc(sizeof(serial));

        (*s)->port = (char *) malloc((strlen(port) + 1) * sizeof(char));
	
	 (*s)->fd = uart_init(port);


	return 0;


}


int serial_read_char(serial *s,unsigned char *p)
{
/*
	if (read(s,fdm p, 1) )
	while (read(s.fd, p, 1) != 1)
		;
	return 1;
*/
	//printf("reading from %d in serial_read_char\n", s->fd);
	return (read(s->fd, p, 1) == 1) ? 0 : -1; 
}

int serial_read(serial *s, unsigned char *buf,  unsigned int len)
{ 
	int count=0;
	//printf("reading from %d \n", s->fd);
	for(int i=0;i<len;i++){
		
	  //printf("reading from %d ------------------\n", s->fd);
	  int j = serial_read_char(s, &buf[i]);
          if (j != 0){
               return count;
          }
	  count = i+1;
	
	}
	return count;

}

int serial_write_char(serial *s, unsigned char p)
{
	return (write(s->fd, &p, 1) == 1) ? 0: -1;
}

int serial_write(serial *s, unsigned char* str,unsigned int length)
{
	
	if (write(s->fd, str, length) != length)
		return -1;
	return 0;
}


void timer_init(timer **t)
{
	*t = (timer *) malloc(sizeof(timer));
	gettimeofday(*t, NULL);
}

unsigned long int timer_elapsed(timer *t)
{
    timer CurrentTime;
    int sec,usec;
    gettimeofday(&CurrentTime, NULL);                                   // Get current time
    sec=CurrentTime.tv_sec - t->tv_sec;
    usec=CurrentTime.tv_usec - t->tv_usec;
    if (usec<0) {                                                                                                               // If the previous usec is higher than the current one
        usec=1000000-t->tv_usec + CurrentTime.tv_usec;          // Recompute the microseonds
        sec--;                                                                                                          // Substract one second
    }
    return sec*1000+usec/1000;
	

}



