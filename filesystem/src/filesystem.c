#include "filesystem.h"

t_log *logger_filesystem;

t_config *CONFIG_FILESYSTEM;
char *IP_MEMORIA;
char *PUERTO_MEMORIA;
char *PUERTO_ESCUCHA_FILESYSTEM;
char *PATH_SUPERBLOQUE;
char *PATH_BITMAP;
char *PATH_BLOQUES;
char *PATH_FCB;
char *RETARDO_ACCESO_BLOQUE;

t_config *CONFIG_SUPERBLOQUE;
int BLOCK_SIZE;
int BLOCK_COUNT;

t_bitarray *bitmap;
char *blocks_buffer;

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
		// Rutinas de preparación
		levantar_loggers_filesystem();
		levantar_config_filesystem();
		levantar_superbloque();

		//*********************
		// HANDSHAKE - MEMORIA
		// if (realizar_handshake(logger_filesystem, IP_MEMORIA, PUERTO_MEMORIA, HANDSHAKE_FILESYSTEM, "Memoria") == -1)
		// {
		// 	return EXIT_FAILURE;
		// }

		//*********************
		// PREPARACIÓN DE BLOQUES - TODO: Hacer logs
		bitmap = levantar_bitmap();
		blocks_buffer = levantar_bloques();

		// Prueba de archivo de bloques
		// char* bloque_nuevo = malloc(BLOCK_SIZE);
		// memset(bloque_nuevo, '1', BLOCK_SIZE);
		// modificar_bloque(blocks_buffer, 3, bloque_nuevo);

		// crear_archivo("elpicante");
		// abrir_archivo("elpicante");
		// truncar_archivo("elpicante", 64);

		//*********************
		// SERVIDOR
		// socket_servidor_filesystem = iniciar_servidor(logger_filesystem, PUERTO_ESCUCHA_FILESYSTEM);
		// if (socket_servidor_filesystem == -1)
		// {
		// 	log_error(logger_filesystem, "No se pudo iniciar el servidor en Filesystem...");
		// 	return EXIT_FAILURE;
		// }
		// log_info(logger_filesystem, "Filesystem escuchando conexiones...");
		// while (server_escuchar(logger_filesystem, socket_servidor_filesystem, (void *)procesar_conexion));

		bitarray_destroy(bitmap);
		free(blocks_buffer);
		config_destroy(CONFIG_FILESYSTEM);
		return EXIT_SUCCESS;
	}
}
