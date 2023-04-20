#include "consola.h"

t_log *logger_consola;
t_config *config_consola;
int socket_kernel;

int main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "-test") == 0)
        run_tests();
    else
    {
        logger_consola = log_create("./log/consola.log", "CONSOLA", true, LOG_LEVEL_INFO);

        config_consola = config_create(argv[1]);
        char *ip_kernel = config_get_string_value(config_consola, "IP_KERNEL");
        char *puerto_kernel = config_get_string_value(config_consola, "PUERTO_KERNEL");

        //*********************
        // HANDSHAKE - KERNEL
        // if (realizar_handshake(logger_consola, ip_kernel, puerto_kernel, HANDSHAKE_CONSOLA, "Kernel") == -1)
        // {
        //     return EXIT_FAILURE;
        // }

        //*********************
        // PARSEO - INSTRUCCIONES
        t_list* instrucciones = list_create();
        instrucciones = parsear_archivo(argv[2], logger_consola);
        
        // for (int i = 0; i < list_size(instrucciones); i++) {
        //     t_instruccion* instruccion = list_get(instrucciones, i);
        //     printf("Instrucción: %s, Arg1: %s, Arg2: %s, Arg3: %s\n", instruccion->instruccion, instruccion->arg1, instruccion->arg2, instruccion->arg3);
        // }

        t_paquete* paquete_instrucciones = crear_paquete_instrucciones();
        void* stream_instrucciones = serializar_paquete_instrucciones(paquete_instrucciones, instrucciones);
        list_destroy_and_destroy_elements(instrucciones, instruccion_destruir);

        t_list* instrucciones_deserializadas = deserializar_instrucciones(stream_instrucciones);

        for (int i = 0; i < list_size(instrucciones_deserializadas); i++) {
            t_instruccion* instruccion = list_get(instrucciones_deserializadas, i);
            printf("Instrucción: %s, Arg1: %s, Arg2: %s, Arg3: %s\n", instruccion->instruccion, instruccion->arg1, instruccion->arg2, instruccion->arg3);
        }

        terminar_programa(logger_consola, config_consola);
        return EXIT_SUCCESS;
    }
}