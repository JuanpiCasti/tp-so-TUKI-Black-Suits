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
}

t_bitarray *levantar_bitmap() // Se podría hacer con mmap, sería mejor??
{
  // Calcular la longitud del bitmap en bits
  int bitmap_size = BLOCK_COUNT / 8;
  void *puntero_a_bits = malloc(bitmap_size);

  // Crear el bitarray
  t_bitarray *bitarray = bitarray_create_with_mode(puntero_a_bits, bitmap_size, LSB_FIRST);
  for (int i = 0; i < BLOCK_COUNT; i++)
  {
    bitarray_clean_bit(bitarray, i);
  }

  // Abrir el archivo de bitmap de bloques existente o crear uno nuevo si no existe
  FILE *bitmap_file = fopen("./blocks/bitmap.bin", "r+");
  if (bitmap_file == NULL)
  {
    bitmap_file = fopen("./blocks/bitmap.bin", "w+");
    // Escribir los bits del bitarray en el archivo de bitmap
    fwrite(bitarray->bitarray, sizeof(char), bitmap_size, bitmap_file);
  }

  // Leer los bits existentes del archivo de bitmap y cargarlos en el bitarray
  fread(bitarray->bitarray, sizeof(char), bitmap_size, bitmap_file);

  // for (int i = 0; i < BLOCK_COUNT; i++)
  // {
  //   printf("Pos. %d: %d\n", i+1, bitarray_test_bit(bitarray, i));
  // }

  fclose(bitmap_file); // ??
  free(puntero_a_bits);
  // bitarray_destroy(bitarray); // Se debería destruír una vez terminado el programa ??
  return bitarray;
}

char *levantar_bloques()
{
  char *blocks_buffer = calloc(BLOCK_COUNT, BLOCK_SIZE); // Crea un buffer con todo inicializado en ceros

  FILE *blocks_file = fopen("./blocks/blocks.bin", "r+");
  if (blocks_file == NULL)
  {
    blocks_file = fopen("./blocks/blocks.bin", "w+");
    fwrite(blocks_buffer, BLOCK_SIZE, BLOCK_COUNT, blocks_file);
  }

  fread(blocks_buffer, BLOCK_SIZE, BLOCK_COUNT, blocks_file);

  fclose(blocks_file); // ??
  //free(blocks_buffer); // Se debería destruír una vez terminado el programa ??
  return blocks_buffer;
}

void modificar_bloque(char *blocks_buffer, int numero_bloque, char *bloque_nuevo)
{
  size_t offset = (numero_bloque - 1) * BLOCK_SIZE;
  memcpy(blocks_buffer + offset, bloque_nuevo, BLOCK_SIZE);

  FILE *blocks_file = fopen("./blocks/blocks.bin", "r+");
  fseek(blocks_file, offset, SEEK_SET);
  fwrite(blocks_buffer + offset, BLOCK_SIZE, 1, blocks_file);

  free(bloque_nuevo);
}
