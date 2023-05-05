#include "kernel.h"

t_log *logger_kernel_extra;
t_log *logger_kernel;

t_config *CONFIG_KERNEL;

char *PUERTO_ESCUCHA_KERNEL;
char *IP_MEMORIA;
char *PUERTO_MEMORIA;
char *IP_FILESYSTEM;
char *PUERTO_FILESYSTEM;
char *IP_CPU;
char *PUERTO_CPU;
double ESTIMACION_INICIAL;
uint32_t GRADO_MAX_MULTIPROGRAMACION;
char* ALGORITMO_PLANIFICACION;


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

t_pcb* RUNNING;
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
		logger_kernel_extra = log_create("./log/kernel_extra.log", "KERNEL_EXTRA", false, LOG_LEVEL_INFO);
		logger_kernel = log_create("./log/kernel.log", "KERNEL", true, LOG_LEVEL_INFO);

		levantar_config_kernel();

		// //*********************
		// // HANDSHAKE - FILESYSTEM
		// if (realizar_handshake(logger_kernel_extra, IP_FILESYSTEM, PUERTO_FILESYSTEM, HANDSHAKE_KERNEL, "Filesystem") == -1)
		// {
		// 	return EXIT_FAILURE;
		// }

		// //*********************
		// // HANDSHAKE - CPU
		// if (realizar_handshake(logger_kernel_extra, IP_CPU, PUERTO_CPU, HANDSHAKE_KERNEL, "CPU") == -1)
		// {
		// 	return EXIT_FAILURE;
		// }

		// //*********************
		// // HANDSHAKE - MEMORIA
		// if (realizar_handshake(logger_kernel_extra, IP_MEMORIA, PUERTO_MEMORIA, HANDSHAKE_KERNEL, "Memoria") == -1)
		// {
		// 	return EXIT_FAILURE;
		// }
		
		inicializar_colas();
		inicializar_semaforos();
		next_pid = 1;

		// Planificadores
		pthread_t hilo_planificacion;
		if (pthread_create(&hilo_planificacion, NULL, (void*)(planificacion), NULL) == -1) {
			log_error(logger_kernel_extra, "No se pudo crear el hilo del planificador de largo plazo.");
			return EXIT_FAILURE;
		}
		
		// pthread_t hilo_planificador_corto_plazo;
		// if(pthread_create(&hilo_planificador_corto_plazo, NULL, (void*)(planificador_corto_plazo), NULL) == -1) {
		// 	log_error(logger_kernel_extra, "No se pudo crear el hilo del planificador de largo plazo.");
		// 	return EXIT_FAILURE;
		// }
		//*********************
		// SERVIDOR
		socket_servidor_kernel = iniciar_servidor(logger_kernel_extra, PUERTO_ESCUCHA_KERNEL);
		if (socket_servidor_kernel == -1)
		{
			log_error(logger_kernel_extra, "No se pudo iniciar el servidor en Kernel...");
			return EXIT_FAILURE;
		}
		log_info(logger_kernel_extra, "Kernel escuchando conexiones...");
		while (server_escuchar(logger_kernel_extra, socket_servidor_kernel, (void *)procesar_conexion));

		pthread_detach(hilo_planificacion);
		return EXIT_SUCCESS;
	}
}
