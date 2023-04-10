#include "memoria.h"

int main(int argc, char ** argv){
    if(argc > 1 && strcmp(argv[1],"-test")==0) {
		run_tests();
	} else {
        t_config* config_memoria = config_create("./cfg/memoria.config");
        char* puerto_escucha_memoria = config_get_string_value(config_memoria, "PUERTO_ESCUCHA");
    }
}
