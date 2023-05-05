#ifndef UTILS_KERNEL_H
#define UTILS_KERNEL_H

#include <stdio.h>
#include <commons/collections/list.h>
#include <commons/string.h>
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

// Logger del kernel
extern t_log* logger_kernel_extra;
extern t_log* logger_kernel;

// Configs del kernel
extern uint32_t GRADO_MAX_MULTIPROGRAMACION;

extern uint32_t next_pid;
extern pthread_mutex_t mutex_next_pid;

// Colas de procesos
extern t_list* NEW;
extern t_list* READY;
extern t_list* BLOCKED;
extern t_pcb* RUNNING;
extern t_list* EXIT;
extern pthread_mutex_t mutex_NEW;
extern pthread_mutex_t mutex_READY;
extern pthread_mutex_t mutex_BLOCKED;
extern pthread_mutex_t mutex_RUNNING;
extern pthread_mutex_t mutex_EXIT;

// Configs
extern t_config* CONFIG_KERNEL;
extern char *PUERTO_ESCUCHA_KERNEL;
extern char *IP_MEMORIA;
extern char *PUERTO_MEMORIA;
extern char *IP_FILESYSTEM;
extern char *PUERTO_FILESYSTEM;
extern char *IP_CPU;
extern char *PUERTO_CPU;
extern double ESTIMACION_INICIAL;
extern uint32_t GRADO_MAX_MULTIPROGRAMACION;
extern char* ALGORITMO_PLANIFICACION;


void levantar_config_kernel(); // setea todas las variables globales de configuracion
void inicializar_colas(); // Inicializa las colas de procesos
void inicializar_semaforos(); // Inicializa los semaforos usados en el modulo

t_pcb *crear_pcb(t_list *instrucciones); // Recibe lista de instrucciones y crea pcb

// Manda el proceso a la cola deseada,
// por ejemplo, para mandar a NEW: encolar_proceso(new_pcb, NEW, mutex_NEW);
void encolar_proceso(t_pcb* new_pcb, t_list* cola, pthread_mutex_t* mutex_cola);
void imprimir_pcb(t_pcb *pcb);


void planificacion(); // Proceso del planificador

#endif
