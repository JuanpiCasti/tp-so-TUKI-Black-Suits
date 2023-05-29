#ifndef FILESYSTEM_UTILS_H
#define FILESYSTEM_UTILS_H
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <commons/bitarray.h>
#include "shared_utils.h"

extern t_log *logger_filesystem;

extern t_config *CONFIG_FILESYSTEM;
extern char *IP_MEMORIA;
extern char *PUERTO_MEMORIA;
extern char *PUERTO_ESCUCHA_FILESYSTEM;
extern char *PATH_SUPERBLOQUE;
extern char *PATH_BITMAP;
extern char *PATH_BLOQUES;
extern char *PATH_FCB;
extern char *RETARDO_ACCESO_BLOQUE;

extern t_config *CONFIG_SUPERBLOQUE;
extern int BLOCK_SIZE;
extern int BLOCK_COUNT;

void levantar_loggers_filesystem();
void levantar_config_filesystem();
void levantar_superbloque();
t_bitarray *levantar_bitmap();
char* levantar_bloques();
void modificar_bloque(char* blocks_buffer, int numero_bloque, const char* bloque_nuevo);

#endif
