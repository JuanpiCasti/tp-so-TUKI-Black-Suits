#include "filesystem.h"

int main(int argc, char **argv)
{
	if (argc > 1 && strcmp(argv[1], "-test") == 0)
	{
		run_tests();
	}
	else
	{
		t_config *config_filesystem = config_create("./cfg/filesystem.config");
		char *ip_memoria = config_get_string_value(config_filesystem, "IP_MEMORIA");
		char *puerto_memoria = config_get_string_value(config_filesystem, "PUERTO_MEMORIA");
		char *puerto_escucha_filesystem = config_get_string_value(config_filesystem, "PUERTO_ESCUCHA");
		char *path_superbloque = config_get_string_value(config_filesystem, "PATH_SUPERBLOQUE");
		char *path_bitmap = config_get_string_value(config_filesystem, "PATH_BITMAP");
		char *path_bloques = config_get_string_value(config_filesystem, "PATH_BLOQUES");
		char *path_fcb = config_get_string_value(config_filesystem, "PATH_FCB");
		char *retardo_acceso_bloque = config_get_string_value(config_filesystem, "RETARDO_ACCESO_BLOQUE");
	}
}