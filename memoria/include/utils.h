#ifndef MEMORIA_UTILS_H
#define MEMORIA_UTILS_H

#include <commons/config.h>
#include "shared_utils.h"

typedef enum {
    FIRST,
    BEST,
    WORST
} t_algo_asig;

extern char* t_algo_asig_desc[3];

typedef struct {
    uint32_t base;
    uint32_t limite;
} t_esp; // Para marcar un hueco de la memoria

extern t_log *logger_memoria;
extern t_log *logger_memoria_extra;
extern t_config* config_memoria;

extern int PUERTO_ESCUCHA_MEMORIA;
extern uint32_t TAM_MEMORIA;
extern uint32_t TAM_SEGMENTO_0;
extern uint32_t CANT_SEGMENTOS;
extern uint32_t RETARDO_MEMORIA;
extern uint32_t RETARDO_COMPACTACION;
extern t_algo_asig ALGORITMO_ASIGNACION;

extern void* ESPACIO_USUARIO;
extern uint32_t ESPACIO_LIBRE_TOTAL;
extern t_list* LISTA_ESPACIOS_LIBRES;

void levantar_loggers_memoria();
void levantar_config_memoria();
void levantar_estructuras_administrativas();
bool comparador_base(void* data1, void* data2);
void print_lista_esp(t_list* lista);
void* crear_tabla_segmentos();
char* leer(uint32_t dir_fisca , uint32_t size);
void escribir(uint32_t dir_fisca, void* data, uint32_t size);
void crear_segmento(uint32_t tam);
void borrar_segmento(uint32_t base, uint32_t limite);
void* crear_tabla_segmentos();

#endif