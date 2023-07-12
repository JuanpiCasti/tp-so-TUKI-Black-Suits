#include "memoria.h"

t_config* config_memoria;
t_log *logger_memoria;
t_log *logger_memoria_extra;

int socket_servidor_memoria;

int PUERTO_ESCUCHA_MEMORIA;
uint32_t TAM_MEMORIA;
uint32_t TAM_SEGMENTO_0;
uint32_t CANT_SEGMENTOS;
uint32_t RETARDO_MEMORIA;
uint32_t RETARDO_COMPACTACION;
t_algo_asig ALGORITMO_ASIGNACION;

void* ESPACIO_USUARIO;
uint32_t ESPACIO_LIBRE_TOTAL;
t_list* LISTA_ESPACIOS_LIBRES;
t_list* LISTA_GLOBAL_SEGMENTOS;
pthread_mutex_t mutex_memoria;

int main(int argc, char **argv)
{
	if (argc > 1 && strcmp(argv[1], "-test") == 0)
	{
		levantar_config_memoria();
		levantar_loggers_memoria();
		levantar_estructuras_administrativas();
		run_tests();
		return EXIT_SUCCESS;
	}
	else
	{
		

		levantar_config_memoria();
		levantar_loggers_memoria();
		levantar_estructuras_administrativas();
		pthread_mutex_init(&mutex_memoria, NULL);



		//*********************
		// SERVIDOR
		char puerto_escucha[10];
		sprintf(puerto_escucha, "%d", PUERTO_ESCUCHA_MEMORIA);
		socket_servidor_memoria = iniciar_servidor(logger_memoria_extra, puerto_escucha);
		if (socket_servidor_memoria == -1)
		{
			log_error(logger_memoria_extra, "No se pudo iniciar el servidor en Memoria...");
			return EXIT_FAILURE;
		}
		log_info(logger_memoria_extra, "Memoria escuchando conexiones...");
		while (server_escuchar(logger_memoria_extra, socket_servidor_memoria, (void *)procesar_conexion));
		return EXIT_SUCCESS;
	}
}
