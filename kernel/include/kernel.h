#ifndef KERNEL_H
#define KERNEL_H

#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <stdbool.h>
#include <sys/time.h>
#include "shared_utils.h"
#include "tests.h"
#include "comunicacion.h"


typedef struct
{
    void* AX;  
    void* BX;
    void* CX;
    void* DX; 
    void* EAX;
    void* EBX;
    void* ECX;
    void* EDX;
    void* RAX;
    void* RBX;
    void* RCX;
    void* RDX;
} t_registros_cpu;

typedef struct
{
	uint32_t pid;
    t_list* instrucciones;
    uint32_t program_counter;
    t_registros_cpu registros_cpu;
    t_list* tabla_segmentos;
    float estimado_HRRN;
    time_t tiempo_ready;
    t_list* archivos_abiertos;

} t_pcb;

#endif