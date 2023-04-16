#include "cpu.h"

t_log *logger_cpu;
int socket_servidor_cpu;
int socket_memoria;

int main(int argc, char **argv)
{
	if (argc > 1 && strcmp(argv[1], "-test") == 0)
	{
		run_tests();
	}
	else
	{
		logger_cpu = log_create("./log/cpu.log", "CPU", true, LOG_LEVEL_INFO);

		t_config *config_cpu = config_create("./cfg/cpu.config");
		// char *retardo_instruccion = config_get_string_value(config_cpu, "RETARDO_INSTRUCCION");
		char *ip_memoria = config_get_string_value(config_cpu, "IP_MEMORIA");
		char *puerto_memoria = config_get_string_value(config_cpu, "PUERTO_MEMORIA");
		char *puerto_escucha_cpu = config_get_string_value(config_cpu, "PUERTO_ESCUCHA");
		// char *tam_max_segmento = config_get_string_value(config_cpu, "TAM_MAX_SEGMENTO");

		//*********************
		// CLIENTE - MEMORIA
		// HANDSHAKE - MEMORIA
		if (realizar_handshake(logger_cpu, ip_memoria, puerto_memoria, HANDSHAKE_CPU, "Memoria") == -1)
		{
			return EXIT_FAILURE;
		}

		//*********************
		// SERVIDOR
		socket_servidor_cpu = iniciar_servidor(logger_cpu, puerto_escucha_cpu);
		if (socket_servidor_cpu == -1)
		{
			log_error(logger_cpu, "No se pudo iniciar el servidor en CPU...");
			return EXIT_FAILURE;
		}
		log_info(logger_cpu, "CPU escuchando conexiones...");
		while (server_escuchar(logger_cpu, config_cpu, socket_servidor_cpu));
		return EXIT_SUCCESS;
	}
}