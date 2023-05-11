#include "shared_utils.h"

char *cod_op_desc[] = {"HANDSHAKE_CONSOLA", "HANDSHAKE_KERNEL", "HANDSHAKE_CPU", "HANDSHAKE_FILESYSTEM", "HANDSHAKE_MEMORIA"};

int iniciar_servidor(t_log *logger, char *puerto)
{
	int socket_servidor;

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, puerto, &hints, &servinfo);

	socket_servidor = socket(servinfo->ai_family,
													 servinfo->ai_socktype,
													 servinfo->ai_protocol);

	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;
}

int esperar_cliente(t_log *logger, int socket_servidor)
{
	int socket_cliente;
	socket_cliente = accept(socket_servidor, NULL, NULL);
	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int crear_conexion(t_log *logger, char *ip, char *puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = 0;
	socket_cliente = socket(server_info->ai_family,
													server_info->ai_socktype,
													server_info->ai_protocol);

	if (socket_cliente == -1)
	{
		log_error(logger, "Error creando el socket para %s:%s", ip, puerto);
		return 0;
	}

	int connection_status = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

	if (connection_status == -1)
	{
		log_error(logger, "Error creando la conexion para %s:%s", ip, puerto);
		freeaddrinfo(server_info);
		return -1;
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

void terminar_programa(t_log *logger, t_config *config)
{
	log_destroy(logger);
	config_destroy(config);
}

int server_escuchar(t_log *logger, int server_socket, void *(*procesar_conexion)(void *))
{
	int cliente_socket = esperar_cliente(logger, server_socket);

	if (cliente_socket != -1)
	{
		pthread_t hilo;
		t_conexion *args = malloc(sizeof(t_conexion));
		args->log = logger;
		args->socket = cliente_socket;
		pthread_create(&hilo, NULL, procesar_conexion, (void *)args);
		pthread_detach(hilo);
		return 1;
	}

	return 0;
}

int enviar_handshake(t_log *logger, int socket_cliente, cod_op handshake)
{
	int resultado;
	send(socket_cliente, &handshake, sizeof(int), 0);
	recv(socket_cliente, &resultado, sizeof(int), MSG_WAITALL);
	if (resultado != -1)
	{
		log_info(logger, "Handshake OK");
	}
	else
	{
		log_error(logger, "Handshake rechazado");
	}

	return resultado;
}

int realizar_handshake(t_log *logger, char *ip_servidor, char *puerto_servidor, cod_op handshake, char *tipo_servidor)
{
	int socket_servidor = conectar_servidor(logger, ip_servidor, puerto_servidor, tipo_servidor);
	if (socket_servidor == -1)
	{
		log_error(logger, "No se pudo conectar al servidor del %s", tipo_servidor);
		return 0;
	}

	if (enviar_handshake(logger, socket_servidor, handshake) == -1)
	{
		close(socket_servidor);
		return -1;
	}

	close(socket_servidor);
	return 0;
}

void aceptar_handshake(t_log *logger, int socket_cliente, cod_op cop)
{
	int result_ok = 0;
	log_info(logger, "Recibido handshake %s.", cod_op_desc[cop]);
	send(socket_cliente, &result_ok, sizeof(int), 0);
}

void rechazar_handshake(t_log *logger, int socket_cliente)
{
	int result_error = -1;
	log_error(logger, "Recibido handshake de un modulo no autorizado, rechazando...");
	send(socket_cliente, &result_error, sizeof(int), 0);
}

int conectar_servidor(t_log *logger, char *ip, char *puerto, char *tipo_servidor)
{
	int socket_servidor = crear_conexion(logger, ip, puerto);
	if (socket_servidor == -1)
	{
		log_error(logger, "No se pudo conectar al servidor %s", tipo_servidor);
		return -1;
	}
	log_info(logger, "Conexión establecida con %s", tipo_servidor);
	return socket_servidor;
}

void *serializar_instrucciones(t_list *instrucciones, int cant_instrucciones, uint32_t tam_buffer_instrucciones)
{
	void *buffer_instrucciones = malloc(tam_buffer_instrucciones);
	int desplazamiento = 0;

	memcpy(buffer_instrucciones, &tam_buffer_instrucciones, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	for (int i = 0; i < cant_instrucciones; i++)
	{
		t_instruccion *instruccion = list_get(instrucciones, i);
		memcpy(buffer_instrucciones + desplazamiento, instruccion->instruccion, sizeof(char[20]));
		memcpy(buffer_instrucciones + desplazamiento + sizeof(char[20]), instruccion->arg1, sizeof(char[20]));
		memcpy(buffer_instrucciones + desplazamiento + sizeof(char[20]) * 2, instruccion->arg2, sizeof(char[20]));
		memcpy(buffer_instrucciones + desplazamiento + sizeof(char[20]) * 3, instruccion->arg3, sizeof(char[20]));
		desplazamiento += sizeof(t_instruccion);
	}

	return buffer_instrucciones;
}

t_list *deserializar_instrucciones(void *stream, uint32_t tam_instrucciones)
{
	int cant_instrucciones = tam_instrucciones / sizeof(t_instruccion);

	t_list *lista_instrucciones = list_create();

	int desplazamiento = 0;
	for (int i = 0; i < cant_instrucciones; i++)
	{
		t_instruccion *instruccion = malloc(sizeof(t_instruccion));
		memcpy(&instruccion->instruccion, stream + desplazamiento, sizeof(char[20]));
		memcpy(&instruccion->arg1, stream + desplazamiento + sizeof(char[20]), sizeof(char[20]));
		memcpy(&instruccion->arg2, stream + desplazamiento + sizeof(char[20]) * 2, sizeof(char[20]));
		memcpy(&instruccion->arg3, stream + desplazamiento + sizeof(char[20]) * 3, sizeof(char[20]));
		list_add(lista_instrucciones, instruccion);
		desplazamiento += sizeof(t_instruccion);
	}

	return lista_instrucciones;
}

char *imprimir_cadena(char *cadena, size_t longitud)
{
	char *cadena_imprimir = malloc(longitud + 1);
	int i;
	for (i = 0; i < longitud && cadena[i] != '\0'; i++)
	{
		if (isprint(cadena[i]))
		{
			cadena_imprimir[i] = cadena[i];
		}
		else
		{
			cadena_imprimir[i] = '?'; // si el caracter no es imprimible, lo reemplazamos por '?'
		}
	}
	cadena_imprimir[i] = '\0'; // aseguramos que la cadena termina en el primer caracter nulo encontrado
	return cadena_imprimir;
}
