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
