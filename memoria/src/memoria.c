#include "memoria.h"

int main(int argc, char ** argv){
    if(argc > 1 && strcmp(argv[1],"-test")==0)
        run_tests();
    else{  
        t_log* logger = log_create("./cfg/memoria.log", "MEMORIA", true, LOG_LEVEL_INFO);
        log_info(logger, "Soy el proceso de la Memoria!");
        log_destroy(logger);
    } 
}