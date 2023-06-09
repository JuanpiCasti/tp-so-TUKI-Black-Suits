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

typedef enum
{
  OPEN,
  CREATE,
  TRUNCATE,
  READ,
  WRITE
} f_instruction;

typedef struct
{
  char f_name[30]; // Nombre del archivo
  uint32_t f_size; // Tamaño del archivo en bytes del archivo
  uint32_t f_dp;   // Puntero directo al primer bloque de datos del archivo
  uint32_t f_ip;   // Puntero indirecto al bloque que contiene los punteros a los siguientes bloques del archivo
} t_fcb;

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
void ocupar_bloque(t_bitarray *bitmap, int index);
void desocupar_bloque(t_bitarray *bitmap, int index);
char *levantar_bloques();
void modificar_bloque(char *blocks_buffer, int numero_bloque, char *bloque_nuevo);
int crear_archivo(char f_name[30]);
int abrir_archivo(char f_name[30]);
void truncar_archivo(char f_name[30], uint32_t new_size);

#endif
