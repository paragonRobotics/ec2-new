// newcdb type definitions.
// Liscence : GPL V2
// (C) Ricky White 2006

#ifndef TYPES_H
#define TYPES_H
#include <stdint.h>

/** Line number in a file, 1 = first line, 0 is invalid
*/
typedef uint32_t	LINE_NUM;

#define LINE_INVALID 0

/** Address in the targets format.
	-ve values indicate an error,
	+ve values are assumed valid if returned from a function
*/
typedef int32_t		ADDR;

#define INVALID_ADDR -1



typedef uint32_t FLAT_ADDR;


/** Breakpoint ID
	-ve indicates invalid breakpoint
*/
typedef int32_t		BP_ID;
static const int BP_ID_INVALID = -1;


typedef int32_t		BLOCK;
typedef int32_t		LEVEL;

#endif
