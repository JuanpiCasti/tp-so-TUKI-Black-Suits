#include "kernel.h"

t_log *logger_kernel;
t_config *config_kernel;
int socket_servidor_kernel;
int socket_filesystem;
int socket_cpu;
int socket_memoria;

int main(int argc, char **argv)
{
	if (argc > 1 && strcmp(argv[1], "-test") == 0)
	{
		run_tests();
	}
	else
	{
		logger_kernel = log_create("./log/kernel.log", "KERNEL", true, LOG_LEVEL_INFO);

		config_kernel = config_create("./cfg/kernel.config");
		char *puerto_escucha_kernel = config_get_string_value(config_kernel, "PUERTO_ESCUCHA");
		char *ip_memoria = config_get_string_value(config_kernel, "IP_MEMORIA");
		char *puerto_memoria = config_get_string_value(config_kernel, "PUERTO_MEMORIA");
		char *ip_filesystem = config_get_string_value(config_kernel, "IP_FILESYSTEM");
		char *puerto_filesystem = config_get_string_value(config_kernel, "PUERTO_FILESYSTEM");
		char *ip_cpu = config_get_string_value(config_kernel, "IP_CPU");
		char *puerto_cpu = config_get_string_value(config_kernel, "PUERTO_CPU");
		// char* algoritmo_planificacion = config_get_string_value(config_kernel, "ALGORITMO_PLANIFICACION");
		// char* estimacion_inicial = config_get_string_value(config_kernel, "ESTIMACION_INICIAL");
		// char* hrrn_alfa = config_get_string_value(config_kernel, "HRRN_ALFA");
		// char* grado_max_multiprogramacion = config_get_string_value(config_kernel, "GRADO_MAX_MULTIPROGRAMACION");
		// char* recursos = config_get_string_value(config_kernel, "RECURSOS");
		// char* instancias_recursos = config_get_string_value(config_kernel, "INSTANCIAS_RECURSOS");

		// //*********************
		// // HANDSHAKE - FILESYSTEM
		// if (realizar_handshake(logger_kernel, ip_filesystem, puerto_filesystem, HANDSHAKE_KERNEL, "Filesystem") == -1)
		// {
		// 	return EXIT_FAILURE;
		// }

		// //*********************
		// // HANDSHAKE - CPU
		// if (realizar_handshake(logger_kernel, ip_cpu, puerto_cpu, HANDSHAKE_KERNEL, "CPU") == -1)
		// {
		// 	return EXIT_FAILURE;
		// }

		// //*********************
		// // HANDSHAKE - MEMORIA
		// if (realizar_handshake(logger_kernel, ip_memoria, puerto_memoria, HANDSHAKE_KERNEL, "Memoria") == -1)
		// {
		// 	return EXIT_FAILURE;
		// }

		//*********************
		// SERVIDOR
		socket_servidor_kernel = iniciar_servidor(logger_kernel, puerto_escucha_kernel);
		if (socket_servidor_kernel == -1)
		{
			log_error(logger_kernel, "No se pudo iniciar el servidor en Kernel...");
			return EXIT_FAILURE;
		}
		log_info(logger_kernel, "Kernel escuchando conexiones...");
		while (server_escuchar(logger_kernel, config_kernel, socket_servidor_kernel, (void *)procesar_conexion));
		return EXIT_SUCCESS;
	}
}