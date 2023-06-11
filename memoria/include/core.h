#ifndef MEMORIA_CORE_H
#define MEMORIA_CORE_H
#include "utils.h"

void* crear_tabla_segmentos();
void* mov_in(uint32_t dir_fisca , uint32_t size);
void mov_out(uint32_t dir_fisca, void* data, uint32_t size);
void crear_segmento(uint32_t tam);
void borrar_segmento(uint32_t base, uint32_t limite);

#endif