#ifndef _IHEX_H
#define _IHEX_H

#include <stdint.h>

void ihex_load_file( char *filename, uint8_t *memory,
					 uint32_t *start, uint32_t *end  );

void ihex_save_file( char *filename, uint8_t *memory,
					 uint32_t start_addr, uint32_t len );
void ihex_hexout( FILE *fhex, int byte,
				  uint32_t memory_location, uint32_t end );

#endif
