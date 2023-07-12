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

typedef struct {
    uint32_t pid;
    uint32_t id;
    uint32_t base;
    uint32_t limite;
} t_segmento; // Para marcar un segmento de la memoria

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
extern t_list* LISTA_GLOBAL_SEGMENTOS;
extern pthread_mutex_t mutex_memoria;

void levantar_loggers_memoria();
void levantar_config_memoria();
void levantar_estructuras_administrativas();
bool comparador_base(void* data1, void* data2);
bool comparador_base_segmento(void* data1, void* data2);
void print_lista_esp(t_list* lista);
void* crear_tabla_segmentos();
char* leer(uint32_t dir_fisca , uint32_t size);
void escribir(uint32_t dir_fisca, void* data, uint32_t size);
cod_op_kernel crear_segmento(uint32_t tam, uint32_t* base_resultante);
void borrar_segmento(uint32_t base, uint32_t limite);
void* crear_tabla_segmentos();
void print_lista_segmentos();
void compactar();
#endif
