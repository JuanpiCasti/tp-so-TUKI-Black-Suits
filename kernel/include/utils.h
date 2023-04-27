#ifndef UTILS_KERNEL_H
#define UTILS_KERNEL_H

#include <stdio.h>
#include <commons/collections/list.h>
#include "shared_utils.h"

typedef struct
{
  void *AX;
  void *BX;
  void *CX;
  void *DX;
  void *EAX;
  void *EBX;
  void *ECX;
  void *EDX;
  void *RAX;
  void *RBX;
  void *RCX;
  void *RDX;
} t_registros_cpu;

typedef struct
{
  uint32_t pid;
  t_list *instrucciones;
  uint32_t program_counter;
  t_registros_cpu *registros_cpu;
  t_list *tabla_segmentos;
  float estimado_HRRN;
  time_t tiempo_ready;
  t_list *archivos_abiertos;
} t_pcb;

extern uint32_t next_pid;
t_list *deserializar_instrucciones(void *stream, uint32_t tam_instrucciones);
t_pcb *crear_pcb(t_list *instrucciones, double estimacion_inicial);
void imprimir_pcb(t_pcb *pcb);

#endif
