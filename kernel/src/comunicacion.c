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

        switch (cop)
        {
        case HANDSHAKE_CONSOLA:
            aceptar_handshake(logger, cliente_socket, cop);
            break;
        // Errores
        case HANDSHAKE_CPU:
        case HANDSHAKE_FILESYSTEM:
        case HANDSHAKE_KERNEL:
        case HANDSHAKE_MEMORIA:
            rechazar_handshake(logger, cliente_socket);
            break;
        case PAQUETE_INSTRUCCIONES:
            t_list *instrucciones = recv_instrucciones(logger, cliente_socket);
            for (int i = 0; i < list_size(instrucciones); i++) {
                t_instruccion* instruccion = list_get(instrucciones, i);
                printf("Instrucción: %s, Arg1: %s, Arg2: %s, Arg3: %s\n", instruccion->instruccion, instruccion->arg1, instruccion->arg2, instruccion->arg3);
            }
            break;
        default:
            log_error(logger, "Algo anduvo mal en el server de Kernel");
            log_info(logger, "Cop: %d", cop);
            return;
        }
    }

    log_warning(logger, "El cliente se desconectó del server");
    return;
}

t_list *recv_instrucciones(t_log *logger, int cliente_socket)
{
    log_info(logger, "Recibiendo paquete de instrucciones...");

    uint32_t size;
    recv(cliente_socket, &size, sizeof(uint32_t), NULL);

    void *stream = malloc(size);
    recv(cliente_socket, stream, size, NULL);

    t_list *instrucciones = deserializar_instrucciones(stream, size);
    free(stream);

    return instrucciones;
}