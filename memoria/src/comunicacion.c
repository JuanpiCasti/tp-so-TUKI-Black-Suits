#include "comunicacion.h"

void devolver_tabla_inicial(int socket) {
    uint32_t size = sizeof(t_ent_ts) * CANT_SEGMENTOS + sizeof(uint32_t);
    void* buffer = malloc(size);

    memcpy(buffer, &CANT_SEGMENTOS, sizeof(uint32_t));

    void* tabla = crear_tabla_segmentos();

    memcpy(buffer + sizeof(uint32_t), tabla, sizeof(t_ent_ts) * CANT_SEGMENTOS);

    send(socket, buffer, size, NULL);

    free(buffer);
    free(tabla);

}

void recibir_argumentos_creacion(uint32_t* id_seg, uint32_t* tam_seg, int socket) {
    recv(socket, id_seg, sizeof(uint32_t), NULL);
    recv(socket, tam_seg, sizeof(uint32_t), NULL);
}

void devolver_resultado_creacion(cod_op_kernel resultado, int socket, uint32_t base) {
    int tam_buffer = sizeof(cod_op_kernel);
    if(resultado == MEMORIA_SEGMENTO_CREADO) {
        tam_buffer += sizeof(uint32_t);
    }
    void* buffer = malloc(tam_buffer);

    int despl = 0;

    memcpy(buffer, &resultado, sizeof(cod_op_kernel));
    despl += sizeof(cod_op_kernel);

    if(resultado == MEMORIA_SEGMENTO_CREADO) {
        memcpy(buffer + despl, &base, sizeof(uint32_t));
    }

    send(socket, buffer, tam_buffer, NULL);
    free(buffer);
}

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
        case HANDSHAKE_CPU:
        case HANDSHAKE_FILESYSTEM:
        case HANDSHAKE_KERNEL:
            aceptar_handshake(logger, cliente_socket, cop);
            break;
        // Errores
        case HANDSHAKE_CONSOLA:
        case HANDSHAKE_MEMORIA:
            rechazar_handshake(logger, cliente_socket);
            break;
        case CREATE_SEGTABLE:
            devolver_tabla_inicial(cliente_socket);
            break;
        case MEMORIA_CREATE_SEGMENT:
            uint32_t id_seg;
            uint32_t tam_seg;
            recibir_argumentos_creacion(&id_seg, &tam_seg, cliente_socket);

            uint32_t n_base;
            cod_op_kernel resultado = crear_segmento(tam_seg, &n_base);
            devolver_resultado_creacion(resultado, cliente_socket, n_base);
            break;
        default:
            log_error(logger, "Algo anduvo mal en el server Memoria");
            log_info(logger, "Cop: %d", cop);
            return;
        }
    }

    log_warning(logger, "El cliente se desconectó del server");
    return;
}