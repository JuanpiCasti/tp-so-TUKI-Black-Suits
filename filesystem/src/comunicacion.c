#include "comunicacion.h"

void procesar_conexion(t_log *logger, int cliente_socket)
{
    cod_op cop;

    while (cliente_socket != -1)
    {
        if (recv(cliente_socket, &cop, sizeof(cod_op), 0) != sizeof(cod_op))
        {
            log_warning(logger, "Cliente desconectado!");
            break;
        }

        switch (cop)
        {
        case HANDSHAKE_KERNEL:
            aceptar_handshake(logger, cliente_socket, cop);
            break;
        // Errores
        case HANDSHAKE_CONSOLA:
        case HANDSHAKE_CPU:
        case HANDSHAKE_FILESYSTEM:
        case HANDSHAKE_MEMORIA:
            rechazar_handshake(logger, cliente_socket);
            break;
        default:
            log_error(logger, "Algo anduvo mal en el server de Filesystem");
            log_info(logger, "Cop: %d", cop);
            return;
        }
    }

    log_warning(logger, "El cliente se desconecto del server");
    return;
}

int server_escuchar(t_log *logger, int server_socket)
{
    int cliente_socket = esperar_cliente(logger, server_socket);

    if (cliente_socket != -1)
    {
        procesar_conexion(logger, cliente_socket);
        return 1;
    }

    return 0;
}
