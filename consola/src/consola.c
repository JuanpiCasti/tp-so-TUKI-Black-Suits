#include "consola.h"

t_log* logger_consola;

int main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "-test") == 0)
        run_tests();
    else
    {
        logger_consola = log_create("./log/consola.log", "CONSOLA", true, LOG_LEVEL_INFO);

        t_config *config_consola = config_create("./cfg/consola.config");
        char *ip_kernel = config_get_string_value(config_consola, "IP_KERNEL");
        char *puerto_kernel = config_get_string_value(config_consola, "PUERTO_KERNEL");

        int socket_kernel = crear_conexion(logger_consola, ip_kernel, puerto_kernel);

        if (socket_kernel == -1)
        {
            log_error(logger_consola, "No se pudo conectar al kernel");
            terminar_programa(logger_consola, socket_kernel, config_consola);
            return EXIT_FAILURE;
        }
        log_info(logger_consola, "Primera conexion a kernel");
        
        if(enviar_handshake(logger_consola, socket_kernel, HANDSHAKE_CONSOLA) == -1) {
            terminar_programa(logger_consola, socket_kernel, config_consola);
            return EXIT_FAILURE;
        }
        // Para que quede conectado hasta que yo quiera pararlo
        int a;
        scanf("%d", &a);

        terminar_programa(logger_consola, socket_kernel, config_consola);
        return EXIT_SUCCESS;

    }
}