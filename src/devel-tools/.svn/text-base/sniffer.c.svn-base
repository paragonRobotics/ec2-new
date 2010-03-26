/** Serial Port Sniffer
	Initial version by  (C) Henri Geist 2006
	Modifications to output format (C) Ricky White 2006

	GPL V2 or later, see copying for details.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define USAGE	"Usage : Master Slave speed format\nwith speedin Kbps and format like [5-8][NOE][1-2]\0n"

struct termios tty;

/*
 * Serial port initialisation :
 * arg : port, baud rate, number of bits/byte, parity,
number of stop bits.
 */
void port_init(int portfd, char *speed, char NB_bits, char
		parity, char NB_stops)
{
	speed_t spd;
	tcgetattr(portfd, &tty); /*Get port attributs in struct
	tty*/

	/*Seting comunication speed*/
	switch(atol(speed))
	{
		case 0:		spd = B0;	break;
		case 300:	spd = B300;	break;
		case 600:	spd = B600;	break;
		case 1200:	spd = B1200;	break;
		case 2400:	spd = B2400;	break;
		case 4800:	spd = B4800;	break;
		case 9600:	spd = B9600;	break;
		case 19200:	spd = B19200;	break;
		case 38400:	spd = B38400;	break;
		case 57600:	spd = B57600;	break;
		case 115200:	spd = B115200;	break;
		case 230400:	spd = B230400;	break;
		default: printf (USAGE);	exit(1);
	}
	cfsetospeed(&tty, spd);	/*Set output speed in struct tty*/
	cfsetispeed(&tty, spd); /*Set input speed in struct tty*/
	
	/*Setting number of bits by byte*/
	switch(NB_bits)
	{
		case '5': tty.c_cflag = (tty.c_cflag & ~CSIZE) |
				CS5;	break;
				case '6': tty.c_cflag = (tty.c_cflag & ~CSIZE) |
						CS6;	break;
						case '7': tty.c_cflag = (tty.c_cflag & ~CSIZE) |
								CS7;	break;
								case '8': tty.c_cflag = (tty.c_cflag & ~CSIZE) |
										CS8;	break;
										default: printf(USAGE);					exit(2);
	}

	tty.c_iflag = 0;		/*To ignoring incomming break signal*/
	tty.c_lflag = 0;		/*Nothing special to do*/
	tty.c_oflag = 0;		/*Nothing special to do*/
	tty.c_cflag |= CLOCAL | CREAD;	/*Ignore modem ctrl &
	Enable
	Rx*/
	
	/*Setting parity*/
	switch(parity)
	{
		case 'O': tty.c_cflag |= PARODD;
		case 'E': tty.c_cflag |= PARENB;
		case 'N': 				break;
		default: printf(USAGE);			exit(3);
	}
	
	/*Setting number of stop bits*/
	switch(NB_stops)
	{
		case '1': tty.c_cflag &= ~CSTOPB;	break; 
		case '2': tty.c_cflag |= CSTOPB;	break;
		default: printf(USAGE);			exit(4);
	}
	tty.c_cc[VMIN] = 0;
	tty.c_cc[VTIME] = 0;
	/*Activating new settings*/
	tcsetattr(portfd, TCSANOW, &tty);
}

/*
 * Main fonction.
 */
int main( int argc, char **argv )
{
	char c;
	int portfdMaster;
	int portfdSlave;
	int MasterHandShake = 0;
	int SlaveHandShake = 0;
	int i = 0;
	int j = 0;

	bool master = true;

	/*Openning the serial port in read/write with nodelay mode*/
	portfdMaster = open(argv[1], O_RDWR | O_NONBLOCK | O_NOCTTY);
	portfdSlave = open(argv[2], O_RDWR | O_NONBLOCK | O_NOCTTY);

	/*Serial port initialisation with the right proprieties*/
	port_init(portfdMaster, argv[3], argv[4][0], argv[4][1],
			  argv[4][2]);

	port_init(portfdSlave, argv[3], argv[4][0], argv[4][1],
			  argv[4][2]);
	/*Flushing old IO*/
	tcflush(portfdMaster, TCIOFLUSH);
	tcflush(portfdSlave, TCIOFLUSH);

	printf("Master\n");
	/*Main loop*/
	while (true)
	{
		ioctl(portfdMaster, TIOCMGET, &i);
		if ((i & (TIOCM_DSR | TIOCM_CD)) != (MasterHandShake
				   &
				   (TIOCM_DSR | TIOCM_CD)))
		{
			if (!master)
			{
				master = true;
//RW				printf("\nMaster\n\t");
				printf("\nT ");
			}
//			printf("\n\t");
			if ((i & TIOCM_DSR) != (MasterHandShake &
						  TIOCM_DSR))
			{
				j = TIOCM_DTR;
				if (i & TIOCM_DSR)
				{
					ioctl(portfdSlave, TIOCMBIS, &j);
//RW					printf("DTR ON ");
				}
				else
				{
					ioctl(portfdSlave, TIOCMBIC, &j);
//RW					printf("DTR OFF ");
				}
				fflush(stdout);
			}
			if ((i & TIOCM_CD) != (MasterHandShake & TIOCM_CD))
			{
				j = TIOCM_RTS;
				if (i & TIOCM_CD)
				{
					ioctl(portfdSlave, TIOCMBIS, &j);
//RW					printf("RTS_ON ");
				}
				else
				{
					ioctl(portfdSlave, TIOCMBIC, &j);
//RW					printf("RTS_OFF ");
				}
				fflush(stdout);
			}
			MasterHandShake = i;
		}
		if (1 == read(portfdMaster, &c, 1))
		{
			if (!master)
			{
				master = true;
//RW				printf("\nMaster\n\t");
				printf("\nT ");
			}
			write(portfdSlave, &c, 1);
			printf("0x%02hhX ", c);
			fflush(stdout);
		}
		ioctl(portfdSlave, TIOCMGET, &i);
		if ((i & (TIOCM_DSR | TIOCM_CD)) != (SlaveHandShake &
				   (TIOCM_DSR | TIOCM_CD)))
		{
			if (master)
			{
				master = false;
//RW				printf("\nSlave\n\t");
				printf("\tR ");
			}
//			printf("\n\t");
			if ((i & TIOCM_DSR) != (SlaveHandShake & TIOCM_DSR))
			{
				j = TIOCM_DTR;
				if (i & TIOCM_DSR)
				{
					ioctl(portfdMaster, TIOCMBIS, &j);
//RW					printf("DTR ON ");
				}
				else
				{
					ioctl(portfdMaster, TIOCMBIC, &j);
//RW					printf("DTR OFF ");
				}
				fflush(stdout);
			}
			if ((i & TIOCM_CD) != (SlaveHandShake & TIOCM_CD))
			{
				j = TIOCM_RTS;
				if (i & TIOCM_CD)
				{
					ioctl(portfdMaster, TIOCMBIS, &j);
//RW					printf("RTS_ON ");
				}
				else
				{
					ioctl(portfdMaster, TIOCMBIC, &j);
//RW					printf("RTS_OFF ");
				}
				fflush(stdout);
			}
			SlaveHandShake = i;
		}
		if (1 == read(portfdSlave, &c, 1))
		{
			if (master)
			{
				master = false;
//RW				printf("\nSlave\n\t");
				printf("\tR ");
			}
			write(portfdMaster, &c, 1 );
			printf("0x%02hhX ", c);
			fflush(stdout);
		}
	}
	
	close(portfdMaster);
	close(portfdSlave);
	exit(0);
}




