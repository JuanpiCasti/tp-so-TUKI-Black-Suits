#include "utils.h"

void levantar_loggers_filesystem()
{
  logger_filesystem = log_create("./log/filesystem.log", "FILESYSTEM", true, LOG_LEVEL_INFO);
}

void levantar_config_filesystem()
{
  CONFIG_FILESYSTEM = config_create("./cfg/filesystem.config");
  IP_MEMORIA = config_get_string_value(CONFIG_FILESYSTEM, "IP_MEMORIA");
  PUERTO_MEMORIA = config_get_string_value(CONFIG_FILESYSTEM, "PUERTO_MEMORIA");
  PUERTO_ESCUCHA_FILESYSTEM = config_get_string_value(CONFIG_FILESYSTEM, "PUERTO_ESCUCHA");
  PATH_SUPERBLOQUE = config_get_string_value(CONFIG_FILESYSTEM, "PATH_SUPERBLOQUE");
  PATH_BITMAP = config_get_string_value(CONFIG_FILESYSTEM, "PATH_BITMAP");
  PATH_BLOQUES = config_get_string_value(CONFIG_FILESYSTEM, "PATH_BLOQUES");
  PATH_FCB = config_get_string_value(CONFIG_FILESYSTEM, "PATH_FCB");
  RETARDO_ACCESO_BLOQUE = config_get_string_value(CONFIG_FILESYSTEM, "RETARDO_ACCESO_BLOQUE");
}

void levantar_superbloque()
{
  CONFIG_SUPERBLOQUE = config_create(PATH_SUPERBLOQUE);
  BLOCK_SIZE = config_get_int_value(CONFIG_SUPERBLOQUE, "BLOCK_SIZE");
  BLOCK_COUNT = config_get_int_value(CONFIG_SUPERBLOQUE, "BLOCK_COUNT");
  config_destroy(CONFIG_SUPERBLOQUE);
}

t_bitarray *levantar_bitmap()
{
  int bitmap_size = BLOCK_COUNT / 8;
  void *puntero_a_bits = malloc(bitmap_size);

  t_bitarray *bitarray = bitarray_create_with_mode(puntero_a_bits, bitmap_size, LSB_FIRST);
  for (int i = 0; i < BLOCK_COUNT; i++)
  {
    bitarray_clean_bit(bitarray, i);
  }

  FILE *bitmap_file = fopen(PATH_BITMAP, "rb");
  if (bitmap_file == NULL)
  {
    bitmap_file = fopen(PATH_BITMAP, "wb");
    fwrite(bitarray->bitarray, 1, bitmap_size, bitmap_file);
  }

  fread(bitarray->bitarray, 1, bitmap_size, bitmap_file);

  // for (int i = 0; i < BLOCK_COUNT; i++)
  // {
  //   printf("Pos. %d: %d\n", i + 1, bitarray_test_bit(bitarray, i));
  // }

  fclose(bitmap_file);
  // free(puntero_a_bits); TODO: Liberar esto al final del programa
  return bitarray;
}

void ocupar_bloque(t_bitarray *bitarray, int numero_bloque)
{
  int bitmap_size = BLOCK_COUNT / 8;

  bitarray_set_bit(bitarray, numero_bloque - 1);

  FILE *bitmap_file = fopen(PATH_BITMAP, "r+b");
  fwrite(bitarray->bitarray, 1, bitmap_size, bitmap_file);

  fclose(bitmap_file);
}

void desocupar_bloque(t_bitarray *bitarray, int numero_bloque)
{
  int bitmap_size = BLOCK_COUNT / 8;

  bitarray_clean_bit(bitarray, numero_bloque - 1);

  FILE *bitmap_file = fopen(PATH_BITMAP, "r+b");
  fwrite(bitarray->bitarray, 1, bitmap_size, bitmap_file);

  fclose(bitmap_file);
}

char *levantar_bloques()
{
  char *blocks_buffer = calloc(BLOCK_COUNT, BLOCK_SIZE); // Crea un buffer con todo inicializado en ceros

  FILE *blocks_file = fopen(PATH_BLOQUES, "r");
  if (blocks_file == NULL)
  {
    blocks_file = fopen(PATH_BLOQUES, "w");
    fwrite(blocks_buffer, BLOCK_SIZE, BLOCK_COUNT, blocks_file);
  }

  fread(blocks_buffer, BLOCK_SIZE, BLOCK_COUNT, blocks_file);

  fclose(blocks_file);
  return blocks_buffer;
}

void modificar_bloque(char *blocks_buffer, int numero_bloque, char *bloque_nuevo)
{
  size_t offset = (numero_bloque - 1) * BLOCK_SIZE;
  memcpy(blocks_buffer + offset, bloque_nuevo, BLOCK_SIZE);

  FILE *blocks_file = fopen(PATH_BLOQUES, "r+");
  fseek(blocks_file, offset, SEEK_SET);
  fwrite(blocks_buffer + offset, BLOCK_SIZE, 1, blocks_file);

  free(bloque_nuevo);
}

int abrir_archivo(char f_name[30])
{
  char path[46]; // 46 viene de los caracteres de: ./fs/fcb/f_name.config
  strcpy(path, PATH_FCB);
  strcat(path, "/");
  strcat(path, f_name);
  strcat(path, ".config");

  FILE *archivo_fcb = fopen(path, "r");
  if (archivo_fcb == NULL)
  {
    printf("El archivo no existe\n");
    return EXIT_FAILURE;
  }

  printf("Archivo abierto\n");
  fclose(archivo_fcb);
  return EXIT_SUCCESS;
}

int crear_archivo(char f_name[30])
{
  t_fcb *new_fcb = malloc(sizeof(t_fcb));
  strncpy(new_fcb->f_name, f_name, 29); // Si la cadena de origen tiene menos de 29 caracteres, los faltantes se llenan con caracteres nulos
  new_fcb->f_name[29] = '\0';           // Agrega el carácter nulo al final
  new_fcb->f_size = 0;
  new_fcb->f_dp = 0;
  new_fcb->f_ip = 0;

  char path[46]; // 46 viene de los caracteres de: ./fs/fcb/f_name.config
  strcpy(path, PATH_FCB);
  strcat(path, "/");
  strcat(path, f_name);
  strcat(path, ".config");

  FILE *archivo_fcb = fopen(path, "w");
  fprintf(archivo_fcb, "NOMBRE_ARCHIVO=%s\n", new_fcb->f_name);
  fprintf(archivo_fcb, "TAMANIO_ARCHIVO=%u\n", new_fcb->f_size);
  fprintf(archivo_fcb, "PUNTERO_DIRECTO=%u\n", new_fcb->f_dp);
  fprintf(archivo_fcb, "PUNTERO_INDIRECTO=%u\n", new_fcb->f_ip);

  printf("Archivo creado\n");
  fclose(archivo_fcb);
  free(new_fcb);
  return EXIT_SUCCESS;
}

t_fcb *levantar_fcb(char f_name[30])
{
  char path[46]; // 46 viene de los caracteres de: ./fs/fcb/f_name.config
  strcpy(path, PATH_FCB);
  strcat(path, "/");
  strcat(path, f_name);
  strcat(path, ".config");

  t_config *FCB = config_create(path);
  t_fcb *fcb = malloc(sizeof(t_fcb));
  strncpy(fcb->f_name, f_name, 29); // Si la cadena de origen tiene menos de 29 caracteres, los faltantes se llenan con caracteres nulos
  fcb->f_name[29] = '\0';           // Agrega el carácter nulo al final
  fcb->f_size = config_get_int_value(FCB, "TAMANIO_ARCHIVO");
  fcb->f_dp = config_get_int_value(FCB, "PUNTERO_DIRECTO");
  fcb->f_ip = config_get_int_value(FCB, "PUNTERO_INDIRECTO");
  config_destroy(FCB);

  return fcb;
}

int calcular_bloques_por_size(uint32_t size)
{
  int resultado = size / BLOCK_SIZE;
  if (size % BLOCK_SIZE != 0)
  {
    resultado++;
  }
  return resultado;
}

void truncar_archivo(char f_name[30], uint32_t new_size)
{
  t_fcb *fcb = levantar_fcb(f_name);

  uint32_t prev_size = fcb->f_size;
  fcb->f_size = new_size;

  if (prev_size < new_size)
  {
    int bloques_necesarios = calcular_bloques_por_size(new_size);
    printf("Ampliar a: %d\n", bloques_necesarios);
    // TODO: Lógica de ampliar cantidad de bloques
  }
  else if (prev_size > new_size)
  {
    int bloques_necesarios = calcular_bloques_por_size(new_size);
    printf("Reducir a: %d\n", bloques_necesarios);
    // TODO: Lógica de reducir cantidad de bloques
  }

  char path[46]; // 46 viene de los caracteres de: ./fs/fcb/f_name.config
  strcpy(path, PATH_FCB);
  strcat(path, "/");
  strcat(path, f_name);
  strcat(path, ".config");

  FILE *archivo_fcb = fopen(path, "r+");
  fprintf(archivo_fcb, "NOMBRE_ARCHIVO=%s\n", fcb->f_name);
  fprintf(archivo_fcb, "TAMANIO_ARCHIVO=%u\n", fcb->f_size);
  fprintf(archivo_fcb, "PUNTERO_DIRECTO=%u\n", fcb->f_dp);
  fprintf(archivo_fcb, "PUNTERO_INDIRECTO=%u\n", fcb->f_ip);

  free(fcb);
}
