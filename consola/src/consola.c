#include "consola.h"

t_log *logger_consola;
int socket_kernel;

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

        //*********************
        // CLIENTE - KERNEL
        socket_kernel = conectar_servidor(logger_consola, ip_kernel, puerto_kernel, "Kernel", HANDSHAKE_CONSOLA, config_consola);
        if (socket_kernel == -1)
        {
            return EXIT_FAILURE;
        }

        if (enviar_handshake(logger_consola, socket_kernel,HANDSHAKE_CONSOLA) == -1) {
            terminar_programa(logger_consola, config_consola);
            close(socket_kernel);
            return EXIT_FAILURE;
        }

        terminar_programa(logger_consola, config_consola);
        close(socket_kernel);
        
        return EXIT_SUCCESS;
    }
}