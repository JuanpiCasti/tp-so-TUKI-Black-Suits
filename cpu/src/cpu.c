#include "cpu.h"

t_log *logger_cpu;
t_config *config_cpu;
int socket_servidor_cpu;

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
uint32_t program_counter;

int main(int argc, char **argv)
{
	if (argc > 1 && strcmp(argv[1], "-test") == 0)
	{
		run_tests();
	}
	else
	{
		logger_cpu = log_create("./log/cpu.log", "CPU", true, LOG_LEVEL_INFO);

		config_cpu = config_create("./cfg/cpu.config");
		// char *retardo_instruccion = config_get_string_value(config_cpu, "RETARDO_INSTRUCCION");
		char *ip_memoria = config_get_string_value(config_cpu, "IP_MEMORIA");
		char *puerto_memoria = config_get_string_value(config_cpu, "PUERTO_MEMORIA");
		char *puerto_escucha_cpu = config_get_string_value(config_cpu, "PUERTO_ESCUCHA");
		// char *tam_max_segmento = config_get_string_value(config_cpu, "TAM_MAX_SEGMENTO");

		inicializar_registros();

		//*********************
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
		while (server_escuchar(logger_cpu, socket_servidor_cpu, (void *)procesar_conexion));
		return EXIT_SUCCESS;
	}
}	