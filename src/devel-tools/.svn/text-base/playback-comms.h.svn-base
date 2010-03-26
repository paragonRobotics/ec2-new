#ifndef EC2SIM_H
#define EC2SIM_H
#include <assert.h>
#include <stdio.h>
#include <string>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>			// UNIX standard function definitions
#include <fcntl.h>			// File control definitions
#include <errno.h>			// Error number definitions
#include <termios.h>		// POSIX terminal control definitions
#include <sys/ioctl.h>
#include <vector>

/**
	@author Ricky White <rickyw@neatstuff.co.nz>
*/
class Ec2Sim
{
public:
    Ec2Sim();
    ~Ec2Sim();
	
	bool open_port( std::string port );
	bool loadFile( std::string file );
	void go();
	
protected:	
	int fd;
	bool debug;
	
	bool write_port_ch( char ch );
	bool write_port( char *buf, int len );
	int read_port_ch();
	bool read_port( char *buf, int len );
	void rx_flush();
	void tx_flush();
	void close_port();
	void print_buf( char *buf, int len );


	void DTR(bool on);
	void RTS(bool on);
	
	typedef std::vector<uint8_t> BYTE_VEC;
	typedef struct
	{
		BYTE_VEC rx;
		BYTE_VEC tx;
	} SIM_DATA_PAIR;
//	SIM_DATA mSimData;
	typedef std::vector<SIM_DATA_PAIR> SIM_DATA_BLOCK;
	SIM_DATA_BLOCK mSimData;
	
	void str2vec( std::string str, BYTE_VEC &vec );
	void print_vec( BYTE_VEC &vec );
};

#endif
