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
        if (argc < 3)
        {
            log_error(logger_consola, "Por favor especificar archivo de configuración e instrucciones a enviar");
            log_destroy(logger_consola);
            return EXIT_FAILURE;
        }
        config_consola = config_create(argv[1]);
        char *ip_kernel = config_get_string_value(config_consola, "IP_KERNEL");
        char *puerto_kernel = config_get_string_value(config_consola, "PUERTO_KERNEL");

        //*********************
        // HANDSHAKE - KERNEL
        if (realizar_handshake(logger_consola, ip_kernel, puerto_kernel, HANDSHAKE_CONSOLA, "Kernel") == -1)
        {
            return EXIT_FAILURE;
        }

        //*********************
        // SEND - INSTRUCCIONES
        enviar_instrucciones(logger_consola, ip_kernel, puerto_kernel, argv[2]);

        terminar_programa(logger_consola, config_consola);
        return EXIT_SUCCESS;
    }
}