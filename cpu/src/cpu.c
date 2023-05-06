#include "cpu.h"

t_log *logger_cpu_extra;
t_log* logger_cpu;

t_config *CONFIG_CPU;
char *RETARDO_INSTRUCCION;
char *IP_MEMORIA;
char *PUERTO_MEMORIA;
char *PUERTO_ESCUCHA_CPU;
char *TAM_MAX_SEGMENTO;


void* AX;
void* BX;
void* CX;
void* DX;
void* EAX;
void* EBX;
void* ECX;
void* EDX;
void* RAX;
void* RBX;
void* RCX;
void* RDX;
uint32_t PROGRAM_COUNTER;

int main(int argc, char **argv)
{
	if (argc > 1 && strcmp(argv[1], "-test") == 0)
	{
		run_tests();
	}
	else
	{

		// Rutinas de preparacion
		levantar_loggers_cpu();
		levantar_config_cpu();
		inicializar_registros();


		// *********************
		// HANDSHAKE - MEMORIA
		// if (realizar_handshake(logger_cpu_extra, IP_MEMORIA, PUERTO_MEMORIA, HANDSHAKE_CPU, "Memoria") == -1)
		// {
		// 	return EXIT_FAILURE;
		// }

		//*********************
		// SERVIDOR
		int socket_servidor_cpu = iniciar_servidor(logger_cpu_extra, PUERTO_ESCUCHA_CPU);
		if (socket_servidor_cpu == -1)
		{
			log_error(logger_cpu_extra, "No se pudo iniciar el servidor en CPU...");
			return EXIT_FAILURE;
		}
		log_info(logger_cpu_extra, "CPU escuchando conexiones...");
		while (server_escuchar(logger_cpu_extra, socket_servidor_cpu, (void *)procesar_conexion));
		return EXIT_SUCCESS;
	}
}	