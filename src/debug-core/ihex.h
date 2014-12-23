#ifndef _IHEX_H
#define _IHEX_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

char ihex_load_file( const char *filename,
					 char *memory,
					 uint32_t *start,
					 uint32_t *end );

void ihex_save_file( const char *filename,
					 char *memory,
					 uint32_t start_addr,
					 uint32_t len );

void ihex_hexout( FILE *fhex,
				  int byte,
				  int memory_location,
				  int end );

#ifdef __cplusplus
}
#endif

#endif
