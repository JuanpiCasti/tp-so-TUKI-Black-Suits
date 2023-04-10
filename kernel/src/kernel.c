#include "kernel.h"

int main(int argc, char ** argv){
	if(argc > 1 && strcmp(argv[1],"-test")==0) {
		run_tests();
	} else {
    	t_config* config_kernel = config_create("./cfg/kernel.config");
		char* ip_memoria = config_get_string_value(config_kernel, "IP_MEMORIA");
    	char* puerto_memoria = config_get_string_value(config_kernel, "PUERTO_MEMORIA");
    	char* ip_fylesistem = config_get_string_value(config_kernel, "IP_FILESYSTEM");
		char* puerto_filesystem = config_get_string_value(config_kernel, "PUERTO_FILESYSTEM");
		char* ip_cpu = config_get_string_value(config_kernel, "IP_CPU");
    	char* puerto_cpu = config_get_string_value(config_kernel, "PUERTO_CPU");
		char* puerto_escucha_kernel = config_get_string_value(config_kernel, "PUERTO_ESCUCHA");
		char* algoritmo_planificacion = config_get_string_value(config_kernel, "ALGORITMO_PLANIFICACION");
    	char* estimacion_inicial = config_get_string_value(config_kernel, "ESTIMACION_INICIAL");
		char* hrrn_alfa = config_get_string_value(config_kernel, "HRRN_ALFA");
		char* grado_max_multiprogramacion = config_get_string_value(config_kernel, "GRADO_MAX_MULTIPROGRAMACION");
    	char* recursos = config_get_string_value(config_kernel, "RECURSOS");
		char* instancias_recursos = config_get_string_value(config_kernel, "INSTANCIAS_RECURSOS");
	}
}