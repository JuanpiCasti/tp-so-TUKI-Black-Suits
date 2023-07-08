#include "core.h"

uint32_t abrir_archivo(char f_name[30])
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
    return 1;
  }

  printf("Archivo abierto\n");
  fclose(archivo_fcb);
  return 0;
}

uint32_t crear_archivo(char f_name[30])
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
  return 0;
}

void truncar_archivo(char* f_name, uint32_t new_size)
{
  t_fcb *fcb = levantar_fcb(f_name);

  uint32_t prev_size = fcb->f_size;
  fcb->f_size = new_size;

  if (prev_size < new_size)
  {
    int bloques_actuales = calcular_bloques_por_size(prev_size);
    int bloques_necesarios = calcular_bloques_por_size(new_size);
    int bloques_adicionales = bloques_necesarios - bloques_actuales;

    if (bloques_adicionales > 0)
    {
      if (bloques_actuales == 0)
      {
        asignar_bloque_directo(fcb);
        asignar_bloque_indirecto(fcb);

        for (int i = 0; i < bloques_adicionales - 1; i++)
        {
          asignar_bloque_al_bloque_indirecto(fcb, i);
        }
      }
      else
      {
        for (int i = 0; i < bloques_adicionales; i++)
        {
          sleep(RETARDO_ACCESO_BLOQUE / 1000);
          asignar_bloque_al_bloque_indirecto(fcb, i + bloques_actuales - 1);
        }
      }
    }
  }
  else if (prev_size > new_size)
  {
    int bloques_actuales = calcular_bloques_por_size(prev_size);
    int bloques_necesarios = calcular_bloques_por_size(new_size);
    int bloques_sobrantes = bloques_actuales - bloques_necesarios;

    if (bloques_sobrantes > 0)
    {
      if (bloques_necesarios == 0)
      {
        for (int i = (bloques_sobrantes - 2); i >= 0; i--)
        {
          uint32_t puntero;
          sleep(RETARDO_ACCESO_BLOQUE / 1000);
          memcpy(&puntero, blocks_buffer + fcb->f_ip + sizeof(uint32_t) * i, sizeof(uint32_t));
          desocupar_bloque(puntero / BLOCK_SIZE);
          remover_puntero_de_bloque_indirecto(fcb, i);
        }

        desocupar_bloque(fcb->f_ip / BLOCK_SIZE);
        fcb->f_ip = 0;
        desocupar_bloque(fcb->f_dp / BLOCK_SIZE);
        fcb->f_dp = 0;
      }
      else
      {
        for (int i = (bloques_actuales - 2); i > (bloques_necesarios - 2); i--)
        {
          uint32_t puntero;
          sleep(RETARDO_ACCESO_BLOQUE / 1000);
          memcpy(&puntero, blocks_buffer + fcb->f_ip + sizeof(uint32_t) * i, sizeof(uint32_t));
          desocupar_bloque(puntero / BLOCK_SIZE);
          remover_puntero_de_bloque_indirecto(fcb, i);
        }
      }
    }
  }

  char path[46]; // 46 viene de los caracteres de: ./fs/fcb/f_name.config
  strcpy(path, PATH_FCB);
  strcat(path, "/");
  strcat(path, f_name);
  strcat(path, ".config");

  FILE *archivo_fcb = fopen(path, "w");
  fprintf(archivo_fcb, "NOMBRE_ARCHIVO=%s\n", fcb->f_name);
  fprintf(archivo_fcb, "TAMANIO_ARCHIVO=%u\n", fcb->f_size);
  fprintf(archivo_fcb, "PUNTERO_DIRECTO=%u\n", fcb->f_dp);
  fprintf(archivo_fcb, "PUNTERO_INDIRECTO=%u\n", fcb->f_ip);

  free(fcb);
  fclose(archivo_fcb);
}

void eferrait(char* f_name, uint32_t offset,uint32_t cantidad, char* data) {
  t_fcb* fcb = levantar_fcb(f_name);
  int desplazamiento = 0;
  int bloque_inicial = offset / BLOCK_SIZE;
  int bloque_final = (offset + cantidad) / BLOCK_SIZE;

  if (bloque_inicial == 0 && bloque_final == 0)
  {
    memcpy(blocks_buffer + fcb->f_dp + offset, data, cantidad);
  } else if (bloque_inicial == bloque_final) {
    uint32_t puntero_a_escribir;
    memcpy(&puntero_a_escribir, blocks_buffer + fcb->f_ip + sizeof(uint32_t) * (bloque_inicial - 1), sizeof(uint32_t));
    memcpy(blocks_buffer + puntero_a_escribir + (offset - BLOCK_SIZE * bloque_inicial), data, cantidad);
  } else {
    int bloques_a_escribir = (bloque_final - bloque_inicial) + 1;
    int cantidad_restante = cantidad;
    int desplazamiento = 0;

    if (bloque_inicial == 0) {
      memcpy(blocks_buffer + fcb->f_dp + offset, data, BLOCK_SIZE - offset);
      desplazamiento += BLOCK_SIZE - offset;
      cantidad_restante -= desplazamiento;
    } else {
      uint32_t puntero_primer_bloque_a_escribir;
      memcpy(&puntero_primer_bloque_a_escribir, blocks_buffer + fcb->f_ip + sizeof(uint32_t) * (bloque_inicial - 1), sizeof(uint32_t));
      memcpy(blocks_buffer + puntero_primer_bloque_a_escribir + (offset - BLOCK_SIZE * bloque_inicial), data + desplazamiento, BLOCK_SIZE * (bloque_inicial + 1) - offset);
      desplazamiento =+ BLOCK_SIZE * (bloque_inicial + 1) - offset;
      cantidad_restante -= BLOCK_SIZE * (bloque_inicial + 1) - offset;
    }
    bloques_a_escribir--;

    for (int i = bloque_inicial + 1; i <= bloque_final; i++)
    {
      uint32_t puntero;
      memcpy(&puntero, blocks_buffer + fcb->f_ip + sizeof(uint32_t) * i, sizeof(uint32_t));
      int cant_a_escribir = cantidad_restante > BLOCK_SIZE ? BLOCK_SIZE : cantidad_restante;
      memcpy(blocks_buffer + puntero, data + desplazamiento, cant_a_escribir);
      desplazamiento += cant_a_escribir;
      cantidad_restante -= cant_a_escribir;
    }
  }

  // Actualizar block_file
  FILE *blocks_file = fopen(PATH_BLOQUES, "w");
  fwrite(blocks_buffer, BLOCK_SIZE, BLOCK_COUNT, blocks_file);
  fclose(blocks_file);
}

void* eferrid(char* f_name, uint32_t offset, uint32_t cantidad) {
  void* data_final = malloc(cantidad);
  t_fcb* fcb = levantar_fcb(f_name);
  int desplazamiento = 0;
  int bloque_inicial = offset / BLOCK_SIZE;
  int bloque_final = (offset + cantidad) / BLOCK_SIZE;

  if (bloque_inicial == 0 && bloque_final == 0)
  {
    memcpy(data_final, blocks_buffer + fcb->f_dp + offset, cantidad);
  } else if (bloque_inicial == bloque_final) {
    uint32_t puntero_a_leer;
    memcpy(&puntero_a_leer, blocks_buffer + fcb->f_ip + sizeof(uint32_t) * (bloque_inicial - 1), sizeof(uint32_t));
    memcpy(data_final, blocks_buffer + puntero_a_leer + (offset - BLOCK_SIZE * bloque_inicial), cantidad);
  } else {
    int bloques_a_leer = (bloque_final - bloque_inicial) + 1;
    int cantidad_restante = cantidad;
    int desplazamiento = 0;

    if(bloque_inicial == 0) {
      memcpy(data_final, blocks_buffer + fcb->f_dp + offset, BLOCK_SIZE - offset);
      desplazamiento += BLOCK_SIZE - offset;
      cantidad_restante -= desplazamiento;
    } else {
      uint32_t puntero_primer_bloque_a_leer;
      memcpy(&puntero_primer_bloque_a_leer, blocks_buffer + fcb->f_ip + sizeof(uint32_t) * (bloque_inicial - 1), sizeof(uint32_t));
      memcpy(data_final + desplazamiento, blocks_buffer + puntero_primer_bloque_a_leer + (offset - BLOCK_SIZE * bloque_inicial), BLOCK_SIZE * (bloque_inicial + 1) - offset);    
      desplazamiento += BLOCK_SIZE * (bloque_inicial + 1) - offset;
      cantidad_restante -= BLOCK_SIZE * (bloque_inicial + 1) - offset;    
    }
    bloques_a_leer--;
    for (int i = bloque_inicial + 1; i <= bloque_final; i++)
    {
      uint32_t puntero;
      memcpy(&puntero, blocks_buffer + fcb->f_ip + sizeof(uint32_t) * i, sizeof(uint32_t));
      int cant_a_leer = cantidad_restante > BLOCK_SIZE ? BLOCK_SIZE : cantidad_restante;
      memcpy(data_final + desplazamiento, blocks_buffer + puntero, cant_a_leer);
      desplazamiento += cant_a_leer;
      cantidad_restante -= cant_a_leer;
    }
    
  }
  
  return data_final;
}