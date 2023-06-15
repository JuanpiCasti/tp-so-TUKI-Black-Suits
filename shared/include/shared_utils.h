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
	CREATE_SEGTABLE
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
	EXIT_RESOURCE_NOT_FOUND
	// BLOCK calculo que tambien, ya veremos cuando lleguemos ahi
} cod_op_kernel;

typedef struct {
    uint32_t id_seg;
    uint32_t base;
    uint32_t tam;
	uint8_t activo;
} t_ent_ts; // Entrada de la tabla de segmentos.

extern char* cod_op_kernel_description[6];

extern uint32_t TAMANIO_CONTEXTO;

int iniciar_servidor(t_log *logger, char *puerto);
int esperar_cliente(t_log *logger, int socket_servidor);
int crear_conexion(t_log *logger, char *ip, char *puerto);
void terminar_programa(t_log *logger, t_config *config);
int server_escuchar(t_log *logger, int server_socket, void *(*procesar_conexion)(void *));
int enviar_handshake(t_log *logger, int socket_cliente, cod_op handshake);
int realizar_handshake(t_log *logger, char *ip_servidor, char *puerto_servidor, cod_op handshake, char *tipo_servidor);
void aceptar_handshake(t_log *logger, int socket_cliente, cod_op cop);
void rechazar_handshake(t_log *logger, int socket_cliente);
int conectar_servidor(t_log *logger, char *ip, char *puerto, char *tipo_servidor);

char *imprimir_cadena(char *cadena, size_t longitud); // Imprime cadena no terminada en '\0'

void *serializar_instrucciones(t_list *instrucciones, int cant_instrucciones, uint32_t tam_buffer_instrucciones);
// Esta funcion devuelve un stream del tamanio sizeof(uint32_t) + tam_buffer_instrucciones. Por lo que los primeros
// 4 bytes son para indicar el tamanio de lo que leer a continuacion.
t_list *deserializar_instrucciones(void *stream, uint32_t tam_instrucciones);
void destroy_instruccion(void* element);

t_list* solicitar_tabla_segmentos(t_log* logger, char* ip, char* puerto);
#endif
