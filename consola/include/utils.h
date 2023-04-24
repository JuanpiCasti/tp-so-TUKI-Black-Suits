#ifndef UTILS_CONSOLA_H
#define UTILS_CONSOLA_H

#include <stdio.h>
#include <consola.h>
#include <commons/collections/list.h>

FILE* leer_archivo(char* nombre_archivo, t_log* logger);
t_list* parsear_archivo(char* nombre_archivo, t_log* logger);
void instruccion_destruir(void* instruccion);
t_paquete* crear_paquete_instrucciones();
void *serializar_paquete_instrucciones(t_paquete *paquete, t_list *instrucciones, int cant_instrucciones, int tam_buffer_instrucciones, uint32_t size_paquete);
void enviar_instrucciones(t_log* logger, char* ip, char* puerto, char* archivo_instrucciones);

#endif