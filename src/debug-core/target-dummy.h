// Dummy Target
// sink for all opperations when no real target exsists.
//
// (C) Ricky White 2007
// GPLv2 or later.
////////////////////////////////////////////////////////////////////////////////
#ifndef TARGETDUMMY_H
#define TARGETDUMMY_H
#include <stdint.h>
#include "target.h"


/** Dummy target sink for all opperations when no real target exsists.
	@author Ricky White <rickyw@neatstuff.co.nz>
*/
class TargetDummy : public Target
{
public:
    TargetDummy();
    virtual ~TargetDummy();
	virtual bool connect();
	virtual bool disconnect();
	virtual bool is_connected();
	virtual string port();
	virtual bool set_port( string port );
	virtual string target_name();
	virtual string target_descr();
	virtual string device();
	virtual uint32_t max_breakpoints()		{ return 4; }
//	virtual bool load_file( string name );
	virtual bool command( string cmd );	

	// device control
	virtual void reset();
	virtual uint16_t step();
	virtual bool add_breakpoint(uint16_t addr);
	virtual bool del_breakpoint(uint16_t addr);
	virtual void clear_all_breakpoints();
	virtual void run_to_bp(int ignore_cnt=0);
	virtual bool is_running();
	virtual void stop();

	// memory reads
	virtual void read_data( uint8_t addr, uint8_t len, unsigned char *buf );
	virtual void read_sfr( uint8_t addr, uint8_t len, unsigned char *buf );
	virtual void read_sfr( uint8_t addr, uint8_t page, uint8_t len, unsigned char *buf );
	virtual void read_xdata( uint16_t addr, uint16_t len, unsigned char *buf );
	virtual void read_code( uint16_t addr, uint16_t len, unsigned char *buf );
	virtual uint16_t read_PC();
	
	// memory writes
	virtual void write_data( uint8_t addr, uint8_t len, unsigned char *buf );
	virtual void write_sfr( uint8_t addr, uint8_t len, unsigned char *buf );
	virtual void write_sfr( uint8_t addr, uint8_t page, uint8_t len, unsigned char *buf );
	virtual void write_xdata( uint16_t addr, uint16_t len, unsigned char *buf );
	virtual void write_code( uint16_t addr, uint16_t len, unsigned char *buf );
	virtual void write_PC( uint16_t addr );

protected:
	bool is_connected_flag;
};

#endif
