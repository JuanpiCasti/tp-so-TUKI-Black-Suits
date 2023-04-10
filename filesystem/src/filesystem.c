#include "filesystem.h"

int main(int argc, char ** argv){
    if(argc > 1 && strcmp(argv[1],"-test")==0) {
		run_tests();
	} else {
        t_config* config_filesystem = config_create("./cfg/filesystem.config");
	    char* ip_filesystem = config_get_string_value(config_filesystem, "IP_FILESYSTEM");
	    char* puerto_escucha_filesystem = config_get_string_value(config_filesystem, "PUERTO_ESCUCHA");
        char* puerto_memoria= config_get_string_value(config_filesystem, "PUERTO_MEMORIA");
    }
}