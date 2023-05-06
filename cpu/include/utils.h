#ifndef CPU_UTILS_H
#define CPU_UTILS_H
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include "shared_utils.h"

typedef enum {
    SET,
    YIELD,
    EXIT
} as_instruction;

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
extern uint32_t PROGRAM_COUNTER;

extern t_log* logger_cpu_extra;
extern t_log* logger_cpu;

extern t_config *CONFIG_CPU;
extern char *RETARDO_INSTRUCCION;
extern char *IP_MEMORIA;
extern char *PUERTO_MEMORIA;
extern char *PUERTO_ESCUCHA_CPU;
extern char *TAM_MAX_SEGMENTO;

void levantar_loggers_cpu();
void levantar_config_cpu();
void inicializar_registros();

void cambiar_contexto(); // Setea los registros y demas estructuras del CPU con los datos recibidos de un proceso
as_instruction decode(t_instruccion instruccion); // Toma una instruccion y devuelve a que elemento del enum as_instruction corresponde
void ejecutar_instrucciones(); // Ejecuta instrucciones hasta  que se encuentra con alguna que requiere terminar la ejecucion (break)

#endif