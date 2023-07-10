#ifndef UTILS_KERNEL_H
#define UTILS_KERNEL_H

#include <stdio.h>
#include <time.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include "shared_utils.h"
#include <semaphore.h>

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
  double estimado_HRRN;
  double ultima_rafaga;
  time_t llegada_ready;
  time_t llegada_running;
  t_list *recursos_asignados;
  t_list *archivos_abiertos;
  int socket_consola;
} t_pcb;

typedef struct{
  char nombre[20];
  int32_t instancias_disponibles;
  t_list* cola_bloqueados; // Lista de t_pcb
} t_recurso;

typedef struct{
  char nombre[20];
  uint32_t instancias_asignadas;
} t_asig_r;

typedef struct{
  char nombre[30];
  t_list* cola_bloqueados;
} t_entrada_tabla_archivos;

typedef struct{
  char nombre[30];
  uint32_t puntero;
} t_archivo_abierto;

typedef struct {
  char f_name[30];
  uint32_t dir_fisica;
  uint32_t tamano;
  t_pcb* pcb;
} t_args_f_op;

typedef struct {
  uint32_t id_seg;
  uint32_t tam;
  t_pcb* pcb;
} t_args_compactacion;

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
extern t_list* PROCESOS_EN_MEMORIA;
extern pthread_mutex_t mutex_NEW;
extern pthread_mutex_t mutex_READY;
extern pthread_mutex_t mutex_BLOCKED;
extern pthread_mutex_t mutex_RUNNING;
extern pthread_mutex_t mutex_EXIT;
extern pthread_mutex_t mutex_RECURSOS;
extern pthread_mutex_t mutex_compactacion;
extern pthread_mutex_t mutex_PROCESOS_EN_MEMORIA;
extern sem_t semaforo_NEW;
extern sem_t semaforo_READY;
extern sem_t semaforo_mp;

extern t_list *tabla_archivos;

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
extern double HRRN_ALFA;
extern int socket_cpu;
extern int socket_memoria;
extern int socket_filesystem;

void inicializar_loggers_kernel(); // crea loggers (oficial y extra)
void levantar_config_kernel();     // setea todas las variables globales de configuracion
void inicializar_colas();          // Inicializa las colas de procesos
void inicializar_semaforos();      // Inicializa los semaforos usados en el modulo

// Manda el proceso a la cola deseada,
// por ejemplo, para mandar a NEW: encolar_proceso(new_pcb, NEW, mutex_NEW);

void imprimir_pcb(t_pcb *pcb);
void loggear_cambio_estado(char *estado_anterior, char *estado_actual, t_pcb *pcb);
void loggear_cola_ready(t_list *cola_ready);
void loggear_fin_proceso(t_pcb* pcb, cod_op_kernel exit_code);

t_list* levantar_recursos(); // Levanta recursos e instancias de la lista de configuracion
void imprimir_lista_recursos(t_list* lista);

t_recurso* buscar_recurso_por_nombre(char* nombre_deseado);

int recurso_asignado(t_pcb* proceso, char* nombre_recurso);
void destroy_t_asig_r(void* element);
bool segmento_activo(t_ent_ts *seg);
void destroy_ent_ts(void* seg);

#endif
