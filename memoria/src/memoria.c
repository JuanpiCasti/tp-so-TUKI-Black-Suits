#include "memoria.h"

int main(int argc, char **argv)
{
	if (argc > 1 && strcmp(argv[1], "-test") == 0)
	{
		run_tests();
	}
	else
	{
		t_config *config_memoria = config_create("./cfg/memoria.config");
		char *puerto_escucha_memoria = config_get_string_value(config_memoria, "PUERTO_ESCUCHA");
		char *tam_memoria = config_get_string_value(config_memoria, "TAM_MEMORIA");
		char *tam_segmento = config_get_string_value(config_memoria, "TAM_SEGMENTO_0");
		char *cant_segmentos = config_get_string_value(config_memoria, "CANT_SEGMENTOS");
		char *retardo_memoria = config_get_string_value(config_memoria, "RETARDO_MEMORIA");
		char *retardo_compactacion = config_get_string_value(config_memoria, "RETARDO_COMPACTACION");
		char *algoritmo_asignacion = config_get_string_value(config_memoria, "ALGORITMO_ASIGNACION");
	}
}
