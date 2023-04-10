#include "cpu.h"

int main(int argc, char ** argv){
    if(argc > 1 && strcmp(argv[1],"-test")==0) {
        run_tests();
    } else {
        t_config* config_cpu = config_create("./cfg/cpu.config");
        char* puerto_escucha_cpu = config_get_string_value(config_cpu, "PUERTO_ESCUCHA");
        char* ip_memoria = config_get_string_value(config_cpu, "IP_MEMORIA");
        char* puerto_memoria= config_get_string_value(config_cpu, "PUERTO_MEMORIA");
    }
}