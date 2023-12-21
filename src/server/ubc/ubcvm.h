#ifndef UBCVM_H
#define UBCVM_H

#include <stdint.h> 
#include "ubc.h"

typedef struct ubcVM_s
{
	uint8_t status;
	uint32_t sp;
	uint32_t bp;
	void* memory;
} ubcVM_t;

void Ubc_CreateVM(ubcVM_t* vm);
void Ubc_DestroVM(ubcVM_t* vm);

#endif
