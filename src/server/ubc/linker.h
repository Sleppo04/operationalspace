#ifndef LINKER_H
#define LINKER_H

#include "parser.h"

typedef struct ubcLinker_s
{
	/** TODO: Add stuff **/
} ubcLinker_t;

void Linker_CreateLinker(ubcLinker_t* linker);
void Linker_LinkClosure(ubcparserclosure_t closure);
void Linker_DestroyLinker(ubcLinker_t* linker);

#endif
