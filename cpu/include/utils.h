#ifndef CPU_UTILS_H
#define CPU_UTILS_H

extern void* AX;
extern void* BX;
extern void* CX;
extern void* DX;
extern void* EAX;
extern void* EBX;
extern void* ECX;
extern void* EDX;
extern void* RAX;
extern void* RBX;
extern void* RCX;
extern void* RDX;
extern uint32_t program_counter;

void inicializar_registros();

#endif