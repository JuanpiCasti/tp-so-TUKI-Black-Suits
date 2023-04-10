#include "consola.h"

int main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "-test") == 0)
        run_tests();
    else
    {
        t_config *config_consola = config_create("./cfg/consola.config");
        char *ip_kernel = config_get_string_value(config_consola, "IP_KERNEL");
        char *puerto_kernel = config_get_string_value(config_consola, "PUERTO_KERNEL");
    }
}