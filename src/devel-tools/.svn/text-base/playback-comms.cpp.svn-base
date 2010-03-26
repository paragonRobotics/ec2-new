#include "playback-comms.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

Ec2Sim::Ec2Sim()
{
}


Ec2Sim::~Ec2Sim()
{
}

bool Ec2Sim::loadFile( std::string file )
{
	ifstream inFile;
	string line;
	cout << "load file"<<endl;
	inFile.open(file.c_str());
	
	if( !inFile )
	{
		cerr << "Unable to open file '"<<file.c_str()<<"'";
		exit(-1);
	}
	
	int t,r;
	while( !inFile.eof() )
	{
		getline( inFile,line );
		cout << line;
		if( (line.length()==0) || (line.find("//",0)!=string::npos) )
			cout << "comment skipped" <<endl;
		else
		{
			cout << "parsing line"<<endl;
			t = line.find("T",0);
			r = line.find("R",0);
			if(t>=0 && r>=0 && r>t )
			{
				SIM_DATA_PAIR p;
				str2vec(line.substr(t, r-t),p.tx);
				str2vec(line.substr(r),p.rx);
				mSimData.push_back(p);
			}
		}
	}
	
	inFile.close();
	return true;
}

void Ec2Sim::str2vec( string str, BYTE_VEC &vec )
{
	str = str.substr(1);	// strip off leading T or R
	int start = str.find_first_of("0123456789abcdefABCDEF");
	int end = str.find_last_of("0123456789abcdefABCDEF");
	str = str.substr(start,end-start+1);
	size_t i=0;
	while( (i=str.find_first_of(" ")) )
	{
		if( i==string::npos )
		{
			// last one
			vec.push_back(strtoul(str.substr(0,i).c_str(),0,16));
			break;
		}
		vec.push_back(strtoul(str.substr(0,i).c_str(),0,16));
		str = str.substr(i+1);
	}
	print_vec( vec);
}

void Ec2Sim::print_vec( BYTE_VEC &vec )
{
	for(unsigned int i=0;i<vec.size();i++)
	{
		printf("0x%02x ",vec[i]);
	}
	printf("\n");
}


// wait for commands and handle them
void Ec2Sim::go()
{
	char buf[255];

	// we reverse tx / rx since we are an ec2 not a pc
	for( unsigned int i=0; i<mSimData.size(); i++)
	{
		cout << "waiting for : ";
		print_vec( mSimData[i].tx );
		while(!read_port( buf,mSimData[i].tx.size()) );
		
		for(unsigned int j=0; j<mSimData[i].rx.size(); j++)
			buf[j] = mSimData[i].rx[j];
		write_port( buf, mSimData[i].rx.size() );
	}
	
	while(1)
	{
		while(!read_port( buf,0x0C) );
	}
}


///////////////////////////////////////////////////////////////////////////////
/// COM port control functions                                              ///
///////////////////////////////////////////////////////////////////////////////
bool Ec2Sim::open_port( string port )
{
	fd = open( port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if( fd == -1 )
	{
	/*
		* Could not open the port.
	*/
		cout << "open_port: Unable to open "<< port<<endl;
			return false;
	}
	else
	{
		fcntl( fd, F_SETFL, 0 );
		struct termios options;
			// Get the current options for the port...
		tcgetattr( fd, &options );
	
	// Set the baud rates to 115200
		cfsetispeed(&options, B115200);
		cfsetospeed(&options, B115200);
		// Enable the receiver and set local mode...
		options.c_cflag |= (CLOCAL | CREAD);
		// set 8N1
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		options.c_cflag &= ~CSIZE;
		options.c_cflag |= CS8;
	
	// Disable hardware flow control
		options.c_cflag &= ~CRTSCTS;
	
	// Disable software flow control
		options.c_iflag = 0;	// raw mode, no translations, no parity checking etc.
	
	// select RAW input
		options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	// select raw output
		options.c_oflag &= ~OPOST;
		
	// Set the new options for the port...
		tcsetattr( fd, TCSANOW, &options );
	}
//		RTS( obj, TRUE );
//		DTR( obj, TRUE );
	return true;
}

bool Ec2Sim::write_port_ch( char ch )
{
	return write_port( &ch, 1 );
}

bool Ec2Sim::write_port( char *buf, int len )
{
	tx_flush();
	rx_flush();
	write( fd, buf, len );
	usleep(10000);				// without this we get TIMEOUT errors
	if( debug )
	{
		printf("TX: ");
		print_buf( buf, len );
	}
	return true;
}

int Ec2Sim::read_port_ch()
{
	char ch;
	if( read_port( &ch, 1 ) )
		return ch;
	else
		return -1;
}

bool Ec2Sim::read_port( char *buf, int len )
{
	fd_set			input;
	struct timeval	timeout;
		
	// Initialize the input set
	FD_ZERO( &input );
	FD_SET( fd, &input );
	fcntl( fd, F_SETFL, 0);	// block if not enough characters available

	// Initialize the timeout structure
	timeout.tv_sec  = 5;		// n seconds timeout
	timeout.tv_usec = 0;
		
	char *cur_ptr = buf;
	int cnt=0, r, n;
		
	// Do the select
	n = select( fd+1, &input, NULL, NULL, &timeout );
	if (n < 0)
	{
		perror("select failed");
		exit(-1);
		return false;
	}
	else if (n == 0)
	{
		puts("TIMEOUT");
		return false;
	}
	else
	{
		r = read( fd, cur_ptr, len-cnt );
		if( debug )
		{
			printf("RX: ");
			print_buf( buf, len );
		}
		return true;
	}
}


void Ec2Sim::rx_flush()
{
	tcflush( fd, TCIFLUSH );
}

void Ec2Sim::tx_flush()
{
	tcflush( fd, TCOFLUSH );
}

void Ec2Sim::close_port()
{
	close(fd);
}

void Ec2Sim::print_buf( char *buf, int len )
{
	while( len-- !=0 )
		printf("%02x ",(unsigned char)*buf++);
	printf("\n");
}


void Ec2Sim::DTR(bool on)
{
	int status;
	ioctl( fd, TIOCMGET, &status );
	if( on )
		status |= TIOCM_DTR;
	else
		status &= ~TIOCM_DTR;
	ioctl( fd, TIOCMSET, &status );
}

void Ec2Sim::RTS(bool on)
{
	int status;
	ioctl( fd, TIOCMGET, &status );
	if( on )
		status |= TIOCM_RTS;
	else
		status &= ~TIOCM_RTS;
	ioctl( fd, TIOCMSET, &status );
}
