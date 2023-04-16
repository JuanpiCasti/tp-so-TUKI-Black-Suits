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

typedef enum
{
	HANDSHAKE_CONSOLA,
	HANDSHAKE_KERNEL,
	HANDSHAKE_CPU,
	HANDSHAKE_FILESYSTEM,
	HANDSHAKE_MEMORIA
} cod_op;

typedef struct
{
	t_log *log;
	t_config *config;
	int socket;
} t_conexion;

int iniciar_servidor(t_log *logger, char *puerto);
int esperar_cliente(t_log *logger, int socket_servidor);
int crear_conexion(t_log *logger, char *ip, char *puerto);
void terminar_programa(t_log *logger, t_config *config);
int enviar_handshake(t_log *logger, int socket_cliente, cod_op handshake);
int realizar_handshake(t_log *logger, char* ip_servidor, char* puerto_servidor, cod_op handshake, char* tipo_servidor);
void aceptar_handshake(t_log *logger, int socket_cliente, cod_op cop);
void rechazar_handshake(t_log *logger, int socket_cliente);
int conectar_servidor(t_log *logger, char *ip, char *puerto, char *tipo_servidor);

#endif