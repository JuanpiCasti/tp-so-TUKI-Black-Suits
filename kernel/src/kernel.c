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
char *ALGORITMO_PLANIFICACION;
char** RECURSOS_EXISTENTES;
char** INSTANCIAS_RECURSOS;
double HRRN_ALFA;

int socket_servidor_kernel;
int socket_filesystem;
int socket_cpu;
int socket_memoria;

// Process ID del próximo PCB que se cree
uint32_t next_pid;
pthread_mutex_t mutex_next_pid;

uint32_t GRADO_ACTUAL_MPROG = 0;
pthread_mutex_t mutex_mp;

t_list *NEW;
pthread_mutex_t mutex_NEW;

t_list *READY;
pthread_mutex_t mutex_READY;

t_pcb *RUNNING;
pthread_mutex_t mutex_RUNNING;

t_list *EXIT;
pthread_mutex_t mutex_EXIT;

t_list* RECURSOS; // Cada recurso tiene su lista de bloqueados
pthread_mutex_t mutex_RECURSOS;

t_list *tabla_archivos;

pthread_mutex_t mutex_compactacion;

t_list* PROCESOS_EN_MEMORIA;
pthread_mutex_t mutex_PROCESOS_EN_MEMORIA;

int main(int argc, char **argv)
{
	if (argc > 1 && strcmp(argv[1], "-test") == 0)
	{
		levantar_config_kernel();
		inicializar_colas();
		inicializar_semaforos();
		printf("Corriendo tests...\n");
		run_tests();
	}
	else
	{
		inicializar_loggers_kernel();
		levantar_config_kernel();
		inicializar_colas();
		inicializar_semaforos();
		next_pid = 1; // Primer PID del sistema sera el '1'

		socket_cpu = crear_conexion(logger_kernel_extra, IP_CPU, PUERTO_CPU);
		socket_memoria = crear_conexion(logger_kernel_extra, IP_MEMORIA, PUERTO_MEMORIA);
		socket_filesystem = crear_conexion(logger_kernel_extra, IP_FILESYSTEM, PUERTO_FILESYSTEM);

		//*********************
		// HANDSHAKE - FILESYSTEM
		if (realizar_handshake(logger_kernel_extra, socket_filesystem, HANDSHAKE_KERNEL) == -1)
		{
			return EXIT_FAILURE;
		}

		//*********************
		// HANDSHAKE - CPU
		if (realizar_handshake(logger_kernel_extra, socket_cpu, HANDSHAKE_KERNEL) == -1)
		{
			return EXIT_FAILURE;
		}

		//*********************
		// HANDSHAKE - MEMORIA
		if (realizar_handshake(logger_kernel_extra, socket_memoria, HANDSHAKE_KERNEL) == -1)
		{
			return EXIT_FAILURE;
		}

		// Planificadores
		pthread_t hilo_planificacion_largo;
		if (pthread_create(&hilo_planificacion_largo, NULL, (void *)(planificacion_largo_plazo), NULL) == -1)
		{
			log_error(logger_kernel_extra, "No se pudo crear el hilo del planificador de largo plazo.");
			return EXIT_FAILURE;
		}
		pthread_t hilo_planificacion_corto;
		if (pthread_create(&hilo_planificacion_corto, NULL, (void *)(planificacion_corto_plazo), NULL) == -1)
		{
			log_error(logger_kernel_extra, "No se pudo crear el hilo del planificador de corto plazo.");
			return EXIT_FAILURE;
		}

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

		pthread_detach(hilo_planificacion_largo);
		pthread_detach(hilo_planificacion_corto);

		return EXIT_SUCCESS;
	}
}
