#ifndef FILESYSTEM_CORE_H
#define FILESYSTEM_CORE_H

#include <utils.h>

uint32_t abrir_archivo(char f_name[30]);
uint32_t crear_archivo(char f_name[30]);
void truncar_archivo(char* f_name, uint32_t new_size);

#endif

