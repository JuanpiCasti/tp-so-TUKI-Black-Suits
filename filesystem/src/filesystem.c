#include "filesystem.h"

t_log *logger_filesystem;
t_config *config_filesystem;
int socket_servidor_filesystem;
int socket_memoria;

int main(int argc, char **argv)
{
	if (argc > 1 && strcmp(argv[1], "-test") == 0)
	{
		run_tests();
	}
	else
	{
		logger_filesystem = log_create("./log/filesystem.log", "FILESYSTEM", true, LOG_LEVEL_INFO);

		config_filesystem = config_create("./cfg/filesystem.config");
		char *ip_memoria = config_get_string_value(config_filesystem, "IP_MEMORIA");
		char *puerto_memoria = config_get_string_value(config_filesystem, "PUERTO_MEMORIA");
		char *puerto_escucha_filesystem = config_get_string_value(config_filesystem, "PUERTO_ESCUCHA");
		// char *path_superbloque = config_get_string_value(config_filesystem, "PATH_SUPERBLOQUE");
		// char *path_bitmap = config_get_string_value(config_filesystem, "PATH_BITMAP");
		// char *path_bloques = config_get_string_value(config_filesystem, "PATH_BLOQUES");
		// char *path_fcb = config_get_string_value(config_filesystem, "PATH_FCB");
		// char *retardo_acceso_bloque = config_get_string_value(config_filesystem, "RETARDO_ACCESO_BLOQUE");

		//*********************
		// HANDSHAKE - MEMORIA
		if (realizar_handshake(logger_filesystem, ip_memoria, puerto_memoria, HANDSHAKE_FILESYSTEM, "Memoria") == -1)
		{
			return EXIT_FAILURE;
		}

		//*********************
		// SERVIDOR
		socket_servidor_filesystem = iniciar_servidor(logger_filesystem, puerto_escucha_filesystem);
		if (socket_servidor_filesystem == -1)
		{
			log_error(logger_filesystem, "No se pudo iniciar el servidor en Filesystem...");
			return EXIT_FAILURE;
		}
		log_info(logger_filesystem, "Filesystem escuchando conexiones...");
		while (server_escuchar(logger_filesystem, config_filesystem, socket_servidor_filesystem, (void *)procesar_conexion));
		return EXIT_SUCCESS;
	}
}