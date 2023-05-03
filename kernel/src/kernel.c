#include "kernel.h"

t_log *logger_kernel_extra;
t_log *logger_kernel;

t_config *config_kernel;
uint32_t GRADO_MAX_MULTIPROGRAMACION;


int socket_servidor_kernel;
int socket_filesystem;
int socket_cpu;
int socket_memoria;

// Process ID del proximo PCB que se cree
uint32_t next_pid;
pthread_mutex_t mutex_next_pid;


t_list* NEW;
pthread_mutex_t mutex_NEW;

t_list* READY;
pthread_mutex_t mutex_READY;

t_list* BLOCKED;
pthread_mutex_t mutex_BLOCKED;

t_list* RUNNING;
pthread_mutex_t mutex_RUNNING;

t_list* EXIT;
pthread_mutex_t mutex_EXIT;


int main(int argc, char **argv)
{
	if (argc > 1 && strcmp(argv[1], "-test") == 0)
	{
		run_tests();
	}
	else
	{
		logger_kernel_extra = log_create("./log/kernel_extra.log", "KERNEL_EXTRA", true, LOG_LEVEL_INFO);
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
		// char* hrrn_alfa = config_get_string_value(config_kernel, "HRRN_ALFA");
		GRADO_MAX_MULTIPROGRAMACION = config_get_int_value(config_kernel, "GRADO_MAX_MULTIPROGRAMACION");
		// char* recursos = config_get_string_value(config_kernel, "RECURSOS");
		// char* instancias_recursos = config_get_string_value(config_kernel, "INSTANCIAS_RECURSOS");

		// //*********************
		// // HANDSHAKE - FILESYSTEM
		// if (realizar_handshake(logger_kernel_extra, ip_filesystem, puerto_filesystem, HANDSHAKE_KERNEL, "Filesystem") == -1)
		// {
		// 	return EXIT_FAILURE;
		// }

		// //*********************
		// // HANDSHAKE - CPU
		// if (realizar_handshake(logger_kernel_extra, ip_cpu, puerto_cpu, HANDSHAKE_KERNEL, "CPU") == -1)
		// {
		// 	return EXIT_FAILURE;
		// }

		// //*********************
		// // HANDSHAKE - MEMORIA
		// if (realizar_handshake(logger_kernel_extra, ip_memoria, puerto_memoria, HANDSHAKE_KERNEL, "Memoria") == -1)
		// {
		// 	return EXIT_FAILURE;
		// }

		inicializar_colas();
		inicializar_semaforos();
		next_pid = 1;

		// Planificador
		pthread_t hilo_planificador;
		pthread_create(&hilo_planificador, NULL, planificador_largo_plazo, NULL);

		//*********************
		// SERVIDOR
		socket_servidor_kernel = iniciar_servidor(logger_kernel_extra, puerto_escucha_kernel);
		if (socket_servidor_kernel == -1)
		{
			log_error(logger_kernel_extra, "No se pudo iniciar el servidor en Kernel...");
			return EXIT_FAILURE;
		}
		log_info(logger_kernel_extra, "Kernel escuchando conexiones...");
		while (server_escuchar(logger_kernel_extra, config_kernel, socket_servidor_kernel, (void *)procesar_conexion));

		pthread_detach(hilo_planificador);
		return EXIT_SUCCESS;
	}
}
