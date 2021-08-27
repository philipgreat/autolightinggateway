#include "serialib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>                 //Used for UART
#include <fcntl.h>                  //Used for UART
#include <termios.h>                //Used for UART
#include <errno.h>


int
set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        if (tcgetattr (fd, &tty) != 0)
        {
                //error_message ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                //error_message ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void
set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0){
		printf ("error %d setting term attributes", 1);
	}
}




int serial_open00(serial **s, char* port, const unsigned int baud){
	*s = (serial *) malloc(sizeof(serial));
	(*s)->port = (char *) malloc((strlen(port) + 1) * sizeof(char));

	 (*s)->fd = open(port,  O_RDWR | O_NOCTTY | O_SYNC);
	 if((*s)->fd < 0){
	 	printf("error open %s",port);
	 	return -1;
	 }
	
	set_interface_attribs ((*s)->fd , B38400, 0);  // set speed to 115,200 bps, 8n1 (no parity)
	set_blocking ((*s)->fd , 1);                // set no blocking




	return 0;

}

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

int serial_open007(serial **s, char* port, const unsigned int baud)
{
	*s = (serial *) malloc(sizeof(serial));

	(*s)->port = (char *) malloc((strlen(port) + 1) * sizeof(char));
	strcpy((*s)->port, port);

	
	struct termios options;
	
	(*s)->fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
	
	if ((*s)->fd == -1)
		return -1;
	
	fcntl((*s)->fd, F_SETFL, FNDELAY);	
	
	tcgetattr((*s)->fd, &options);
	bzero(&options, sizeof(options));
	speed_t speed;
	switch (baud){
		case 110  :     speed=B110; break;
		case 300  :     speed=B300; break;   
		case 600  :     speed=B600; break;
		case 1200 :     speed=B1200; break;
		case 2400 :     speed=B2400; break;
		case 4800 :     speed=B4800; break;
		case 9600 :     speed=B9600; break;
		case 19200 :    speed=B19200; break;
		case 38400 :    speed=B38400; break;
		case 57600 :    speed=B57600; break;
		case 115200 :   speed=B115200; break;
	}
	
	cfsetispeed(&options, speed);
        cfsetospeed(&options, speed);

	options.c_cflag |= ( CLOCAL | CREAD |  CS8);
	//options.c_iflag |= ( IGNPAR | IGNBRK );     
	options.c_cc[VTIME]=100;			
	options.c_cc[VMIN]=1;			
	tcsetattr((*s)->fd, TCSANOW, &options);
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
	int i = 0;
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



