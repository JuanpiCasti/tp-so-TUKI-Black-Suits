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
        if (realizar_handshake(logger_consola, ip_kernel, puerto_kernel, HANDSHAKE_CONSOLA, "Kernel") == -1)
        {
            return 1;
        }

        //*********************
        // PARSEO - INSTRUCCIONES

        enviar_instrucciones(logger_consola, ip_kernel, puerto_kernel, argv[2]);

        terminar_programa(logger_consola, config_consola);
        return EXIT_SUCCESS;
    }
}