#ifndef UTILS_KERNEL_H
#define UTILS_KERNEL_H

#include <stdio.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include "shared_utils.h"

typedef struct
{
  char AX[4];
  char BX[4];
  char CX[4];
  char DX[4];
  char EAX[8];
  char EBX[8];
  char ECX[8];
  char EDX[8];
  char RAX[16];
  char RBX[16];
  char RCX[16];
  char RDX[16];
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
  int socket_consola;
} t_pcb;

typedef struct{
  char nombre[20];
  uint32_t instancias_disponibles;
  t_list* cola_bloqueados; // Lista de t_pcb
} t_recurso;

// Logger del kernel
extern t_log *logger_kernel_extra;
extern t_log *logger_kernel;


extern uint32_t next_pid;
extern pthread_mutex_t mutex_next_pid;

//Grado multiprogramacion
extern uint32_t GRADO_ACTUAL_MPROG;
extern pthread_mutex_t mutex_mp;

// Colas de procesos
extern t_list *NEW;
extern t_list *READY;
extern t_list *BLOCKED;
extern t_pcb *RUNNING;
extern t_list *EXIT;
extern t_list* RECURSOS;
extern pthread_mutex_t mutex_NEW;
extern pthread_mutex_t mutex_READY;
extern pthread_mutex_t mutex_BLOCKED;
extern pthread_mutex_t mutex_RUNNING;
extern pthread_mutex_t mutex_EXIT;
extern pthread_mutex_t mutex_RECURSOS;

// Configs
extern t_config *CONFIG_KERNEL;
extern char *PUERTO_ESCUCHA_KERNEL;
extern char *IP_MEMORIA;
extern char *PUERTO_MEMORIA;
extern char *IP_FILESYSTEM;
extern char *PUERTO_FILESYSTEM;
extern char *IP_CPU;
extern char *PUERTO_CPU;
extern double ESTIMACION_INICIAL;
extern uint32_t GRADO_MAX_MULTIPROGRAMACION;
extern char *ALGORITMO_PLANIFICACION;
extern char** RECURSOS_EXISTENTES;
extern char** INSTANCIAS_RECURSOS;

void inicializar_loggers_kernel(); // crea loggers (oficial y extra)
void levantar_config_kernel();     // setea todas las variables globales de configuracion
void inicializar_colas();          // Inicializa las colas de procesos
void inicializar_semaforos();      // Inicializa los semaforos usados en el modulo

t_pcb *crear_pcb(t_list *instrucciones, int socket_consola); // Recibe lista de instrucciones y crea pcb

// Manda el proceso a la cola deseada,
// por ejemplo, para mandar a NEW: encolar_proceso(new_pcb, NEW, mutex_NEW);

void imprimir_pcb(t_pcb *pcb);
void loggear_cambio_estado(char *estado_anterior, char *estado_actual, t_pcb *pcb);
void loggear_cola_ready();
void loggear_fin_proceso(t_pcb* pcb, cod_op_kernel exit_code);

t_list* levantar_recursos(); // Levanta recursos e instancias de la lista de configuracion
void imprimir_lista_recursos(t_list* lista);

#endif
