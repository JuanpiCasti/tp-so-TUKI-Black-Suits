#include "memoria.h"

t_log *logger_memoria;
int socket_servidor_memoria;

int main(int argc, char **argv)
{
	if (argc > 1 && strcmp(argv[1], "-test") == 0)
	{
		run_tests();
	}
	else
	{
		logger_memoria = log_create("./log/memoria.log", "MEMORIA", true, LOG_LEVEL_INFO);

		t_config *config_memoria = config_create("./cfg/memoria.config");
		char *puerto_escucha_memoria = config_get_string_value(config_memoria, "PUERTO_ESCUCHA");
		// char *tam_memoria = config_get_string_value(config_memoria, "TAM_MEMORIA");
		// char *tam_segmento = config_get_string_value(config_memoria, "TAM_SEGMENTO_0");
		// char *cant_segmentos = config_get_string_value(config_memoria, "CANT_SEGMENTOS");
		// char *retardo_memoria = config_get_string_value(config_memoria, "RETARDO_MEMORIA");
		// char *retardo_compactacion = config_get_string_value(config_memoria, "RETARDO_COMPACTACION");
		// char *algoritmo_asignacion = config_get_string_value(config_memoria, "ALGORITMO_ASIGNACION");


		//*********************
		//SERVIDOR
		socket_servidor_memoria = iniciar_servidor(logger_memoria, puerto_escucha_memoria);
		if (socket_servidor_memoria == -1)
		{
			log_error(logger_memoria, "No se pudo iniciar el servidor en Memoria...");
			return EXIT_FAILURE;
		}
		log_info(logger_memoria, "Memoria escuchando conexiones...");
		while (server_escuchar(logger_memoria, config_memoria, socket_servidor_memoria));
		return EXIT_SUCCESS;
	}
}
