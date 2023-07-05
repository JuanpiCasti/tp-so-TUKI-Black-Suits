#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

typedef enum
{
	HANDSHAKE_CONSOLA,
	HANDSHAKE_KERNEL,
	HANDSHAKE_CPU,
	HANDSHAKE_FILESYSTEM,
	HANDSHAKE_MEMORIA,
	PAQUETE_INSTRUCCIONES,
	NUEVO_CONTEXTO_PCB,
	CREATE_SEGTABLE,
	MEMORIA_CREATE_SEGMENT,
	MEMORIA_FREE_SEGMENT,
	MEMORIA_MOV_IN,
	MEMORIA_MOV_OUT,
	ABRIR_ARCHIVO,
	CREAR_ARCHIVO,
	TRUNCAR_ARCHIVO,
	COMPACTAR
} cod_op;

typedef struct
{
	t_log *log;
	int socket;
} t_conexion;

typedef struct
{
	char instruccion[20];
	char arg1[20];
	char arg2[20];
	char arg3[20];
} t_instruccion;

typedef enum
{
	CPU_EXIT,
	CPU_YIELD,
	CPU_IO,
	CPU_WAIT, 
	CPU_SIGNAL,
	CPU_CREATE_SEGMENT,
	CPU_DELETE_SEGMENT,
	EXIT_RESOURCE_NOT_FOUND,
	EXIT_OUT_OF_MEMORY,
	MEMORIA_NECESITA_COMPACTACION,
	MEMORIA_SEGMENTO_CREADO,
	CPU_SEG_FAULT,
	CPU_F_OPEN,
  CPU_F_CLOSE,
  CPU_F_SEEK,
  CPU_F_TRUNCATE
} cod_op_kernel;

typedef struct {
    uint32_t id_seg;
    uint32_t base;
    uint32_t tam;
	uint8_t activo;
} t_ent_ts; // Entrada de la tabla de segmentos

extern char* cod_op_kernel_description[11];

extern uint32_t TAMANIO_CONTEXTO;

int iniciar_servidor(t_log *logger, char *puerto);
int esperar_cliente(t_log *logger, int socket_servidor);
int crear_conexion(t_log *logger, char *ip, char *puerto);
void terminar_programa(t_log *logger, t_config *config);
int server_escuchar(t_log *logger, int server_socket, void *(*procesar_conexion)(void *));
int enviar_handshake(t_log *logger, int socket_cliente, cod_op handshake);
int realizar_handshake(t_log *logger, int socket_servidor, cod_op handshake);
void aceptar_handshake(t_log *logger, int socket_cliente, cod_op cop);
void rechazar_handshake(t_log *logger, int socket_cliente);
int conectar_servidor(t_log *logger, char *ip, char *puerto, char *tipo_servidor);

char *imprimir_cadena(char *cadena, size_t longitud); // Imprime cadena no terminada en '\0'

void *serializar_instrucciones(t_list *instrucciones, int cant_instrucciones, uint32_t tam_buffer_instrucciones);
// Esta funcion devuelve un stream del tamanio sizeof(uint32_t) + tam_buffer_instrucciones. Por lo que los primeros
// 4 bytes son para indicar el tamanio de lo que leer a continuacion.
t_list *deserializar_instrucciones(void *stream, uint32_t tam_instrucciones);
void destroy_instruccion(void* element);

void* serializar_tabla_segmentos(t_list* tabla, uint32_t tam_tabla);
t_list *deserializar_segmentos(void *stream, uint32_t cant_segmentos);
#endif
