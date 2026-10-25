#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>

#define UART_CFLAG_DEBUG	0x8000
#define UART_CFLAG_DEVON	0x10000

int main(int argc, char *argv[])
{
	int c, devnum = -1, br = 0, get = 0, data = 0, stop = 0, fd, i, baud;
	char *dev_op = NULL, *parity = NULL, *fc = NULL, *dbg_op = NULL;
	char msg[64] = {0};
	char uname[][8] = {"uart2", "uart3", "uart4", "uart5"};
	char dev[][16] = {"/dev/ttyS20","/dev/ttyS30","/dev/ttyS40","/dev/ttyS50"};
	int speed_arr[]={B921600,B460800,B230400,B115200,B57600,B38400,B19200,B9600,B4800,B2400,B1200,B600,B300,B110};
	int name_arr[]={921600,460800,230400,115200,57600,38400,19200,9600,4800,2400,1200,600,300,110};
	struct termios opt;
	
	opterr = 0;

	while ((c = getopt(argc, argv, "v:o:b:d:p:s:c:gw:m:")) != -1) {
		switch (c) {
			case 'v':
				for(i = 0; i < 4; i++) {
					if(!strcmp(optarg, uname[i])) {
						devnum = i;
						break;
					}
				}
				if(devnum == -1) {
					printf("Wrong device name (-v), please input uart[2-5].\n");
					return -1;
				}
				break;
			case 'o':
				if(!strcmp(optarg, "on") || !strcmp(optarg, "off")) {
					dev_op = optarg;
				} else {
					printf("Wrong device operation (-o), please input enable/disable.\n");
					return -1;
				}
				break;
			case 'b':
				br = atoi(optarg);
				for( i = 0; i < sizeof(speed_arr)/sizeof(int); i++) {
					if (br == name_arr[i]) {
						break;
					}
				}
				if(i == sizeof(speed_arr)/sizeof(int)) {
					printf("Wrong baud rate (-b), please input 921600/460800/230400/115200/57600/38400/19200/9600/4800/2400/1200/600/300/110.\n");
					return -1;
				}
				break;
			case 'd':
				data = atoi(optarg);
				if((data < 5) || (data > 8)) {
					printf("Wrong data bit (-d), please input 5/6/7/8.\n");
					return -1;
				}
				break;
			case 'p':
				if(!strcmp(optarg, "none") || !strcmp(optarg, "odd") || !strcmp(optarg, "even")) {
					parity = optarg;
				} else {
					printf("Wrong parity check (-p), please input none/odd/even.\n");
					return -1;
				}
				break;
			case 's':
				stop = atoi(optarg);
				if((stop < 1 ) || (stop > 2)) {
					printf("Wrong stop bit (-s), please input 1/2.\n");
					return -1;
				}
				break;
			case 'c':
				if(!strcmp(optarg, "enable") || !strcmp(optarg, "disable")) {
					fc = optarg;
				} else {
					printf("Wrong hardware flow control operation (-c), please input enable/disable.\n");
					return -1;
				}
				break;
			case 'g':
				get = 1;
				break;
			case 'w':
				strncpy(msg,optarg, sizeof(msg));
				msg[(sizeof(msg)-1)] = '\0';
				break;
			case 'm':
				if(!strcmp(optarg, "on") || !strcmp(optarg, "off")) {
					dbg_op = optarg;
				} else {
					printf("Wrong debug operation (-m), please input on/off.\n");
					return -1;
				}
				break;
		}
	}
	
	if(devnum == -1) {
		printf("Device(-v) is mandatory, please assign device.\n");
		return -1;
	}

	fd = open(dev[devnum], O_RDWR|O_NOCTTY);
	if(fd < 0) {
		printf("Unable to open %s\n", dev[devnum]);
		return -1;
	}
	
	if (tcgetattr(fd, &opt) != 0) {
		printf("tcgetattr failed\n");
		close(fd);
		return -1;
	}
	
	if(dev_op) {
		if(!strcmp(dev_op,"on")) {
			opt.c_cflag |= UART_CFLAG_DEVON;
		}else {
			opt.c_cflag &= ~UART_CFLAG_DEVON;
		}
	}
	
	for( i = 0; i < sizeof(speed_arr)/sizeof(int); i++) {
		if (br == name_arr[i]) {
			cfsetispeed(&opt, speed_arr[i]);
			cfsetospeed(&opt, speed_arr[i]);
			break;
		}
	}
	
	if(data) {
		opt.c_cflag &= ~CSIZE;
		if(data == 5) {
			opt.c_cflag |= CS5;
		} else if(data == 6) {
			opt.c_cflag |= CS6;
		} else if(data == 7) {
			opt.c_cflag |= CS7;
		} else if(data == 8) {
			opt.c_cflag |= CS8;
		}
	}
	
	if(parity) {
		if(!strcmp(parity,"none")) {
			opt.c_cflag &= ~PARENB;
		}else {
			opt.c_cflag |= PARENB;
			if(!strcmp(parity,"odd")) {
				opt.c_cflag |= PARODD;
			} else {
				opt.c_cflag &= ~PARODD;
			}
		}
	}
	
	if(stop == 2) {
		opt.c_cflag |= CSTOPB;
	} else {
		opt.c_cflag &= ~CSTOPB;
	}
	
	if(dbg_op) {
		if(!strcmp(dbg_op, "on"))
			opt.c_cflag |= UART_CFLAG_DEBUG;
		else
			opt.c_cflag &= ~UART_CFLAG_DEBUG;
	}
	
	if(fc) {
		if(!strcmp(fc, "enable"))
			opt.c_cflag |= CRTSCTS;
		else
			opt.c_cflag &= ~CRTSCTS;
	}
	
	opt.c_cflag |= (CLOCAL | CREAD);
	
	if(tcsetattr(fd, TCSANOW, &opt)!=0) {
		printf("tcsetattr failed");
		close(fd);
		return -1;
	}
	
	if( '\0' != msg[0] ) {
		if(write(fd, msg, sizeof(msg)/sizeof(char)) < 0) {
			printf("Write msg to %s failed\n", dev[devnum]);
			close(fd);
			return -1;
		}
	}
	
	if(get) {
		if (tcgetattr(fd, &opt) != 0) {
			printf("tcgetattr failed\n");
			close(fd);
			return -1;
		}
		
		if(opt.c_cflag & UART_CFLAG_DEVON)
			printf("Status : On\n");
		else
			printf("Status : Off\n");
		
		baud = opt.c_cflag & CBAUD;
		for(i = 0; i < 11; i++) {
			if(baud == speed_arr[i]) {
				printf("Baud rate : %d\n", name_arr[i]);
				break;
			}
		}
		
		switch (opt.c_cflag & CSIZE)
		{
			case CS5:
				printf("Data bit : 5\n");
				break;
			case CS6:
				printf("Data bit : 6\n");
				break;
			case CS7:
				printf("Data bit : 7\n");
				break;
			case CS8:
				printf("Data bit : 8\n");
				break;
		}
		
		if(opt.c_cflag & CSTOPB)
			printf("Stop bit : 2\n");
		else
			printf("Stop bit : 1\n");
		
		if(opt.c_cflag & PARENB) {
			if(opt.c_cflag & PARODD)
				printf("Parity check : Odd\n");
			else
				printf("Parity check : Even\n");
		} else
			printf("Parity check : None\n");
		
		if(opt.c_cflag & CRTSCTS)
			printf("Hardware flow control : Enable\n");
		else
			printf("Hardware flow control : Disable\n");
		
		if(opt.c_cflag & UART_CFLAG_DEBUG)
			printf("Debug message : On\n");
		else
			printf("Debug message : Off\n");
	}
	
	close(fd);
		
	return 0;
}






















