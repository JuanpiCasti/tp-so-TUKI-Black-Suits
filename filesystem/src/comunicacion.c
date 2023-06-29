#include "comunicacion.h"

void procesar_conexion(void *void_args)
{
    t_conexion *args = (t_conexion *)void_args;
    t_log *logger = args->log;
    int cliente_socket = args->socket;
    free(args);

    cod_op cop;

    while (cliente_socket != -1)
    {
        if (recv(cliente_socket, &cop, sizeof(cod_op), 0) != sizeof(cod_op))
        {
            log_warning(logger, "Cliente desconectado!");
            break;
        }

        char f_name[30];
        uint32_t archivo_ok;

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
        case ABRIR_ARCHIVO:
            recv(cliente_socket, f_name, sizeof(char[30]), NULL);
            archivo_ok = abrir_archivo(f_name);
            send(cliente_socket, &archivo_ok, sizeof(uint32_t), NULL);
            break;
        case CREAR_ARCHIVO:
            recv(cliente_socket, f_name, sizeof(char[30]), NULL);
            archivo_ok = crear_archivo(f_name);
            send(cliente_socket, &archivo_ok, sizeof(uint32_t), NULL);
            break;
        case TRUNCAR_ARCHIVO:
            uint32_t f_size;
            recv(cliente_socket, f_name, sizeof(char[30]), NULL);
            recv(cliente_socket, &f_size, sizeof(uint32_t), NULL);
            truncar_archivo(f_name, f_size);
            archivo_ok = 0;
            send(cliente_socket, &archivo_ok, sizeof(uint32_t), NULL);
            break;
        default:
            log_error(logger, "Algo anduvo mal en el server de Filesystem");
            log_info(logger, "Cop: %d", cop);
            return;
        }
    }

    log_warning(logger, "El cliente se desconectó del server");
    return;
}
