#ifndef CPU_UTILS_H
#define CPU_UTILS_H
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "shared_utils.h"

typedef enum {
    SET,
    YIELD,
    EXIT
} as_instruction;

extern char AX[4];
extern char BX[4];
extern char CX[4];
extern char DX[4];
extern char EAX[8];
extern char EBX[8];
extern char ECX[8];
extern char EDX[8];
extern char RAX[16];
extern char RBX[16];
extern char RCX[16];
extern char RDX[16];
extern uint32_t PROGRAM_COUNTER;
extern t_list* INSTRUCTION_LIST;

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

void cambiar_contexto(void* buffer);   // Setea los registros y demas estructuras del CPU con los datos recibidos de un proceso
as_instruction decode(t_instruccion* instruccion); // Toma una instruccion y devuelve a que elemento del enum as_instruction corresponde
cod_op_kernel ejecutar_instrucciones(); // Ejecuta instrucciones hasta  que se encuentra con alguna que requiere terminar la ejecucion (break)
void imprimir_contexto_actual();

#endif