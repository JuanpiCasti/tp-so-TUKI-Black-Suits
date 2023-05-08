#include "cpu.h"

void* recibir_buffer(int cliente_socket) {
    uint32_t size;
    recv(cliente_socket, &size, sizeof(uint32_t), NULL);
    void* buffer = malloc(size);
    recv(cliente_socket, buffer, size, NULL);
    return buffer;
}

void serializar_contexto(void* buffer, cod_op_kernel cop, int tamanio_contexto) {
    int desplazamiento = 0;

    memcpy(buffer + desplazamiento, &cop, sizeof(cod_op_kernel));
    desplazamiento += sizeof(cod_op_kernel);
    memcpy(buffer + desplazamiento, &tamanio_contexto, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer + desplazamiento, AX, 4);
    desplazamiento += 4;
    memcpy(buffer + desplazamiento, BX, 4);
    desplazamiento += 4;
    memcpy(buffer + desplazamiento, CX, 4);
    desplazamiento += 4;
    memcpy(buffer + desplazamiento, DX, 4);
    desplazamiento += 4;
    memcpy(buffer + desplazamiento, EAX, 8);
    desplazamiento += 8;
    memcpy(buffer + desplazamiento, EBX, 8);
    desplazamiento += 8;
    memcpy(buffer + desplazamiento, ECX, 8);
    desplazamiento += 8;
    memcpy(buffer + desplazamiento, EDX, 8);
    desplazamiento += 8;
    memcpy(buffer + desplazamiento, RAX, 16);
    desplazamiento += 16;
    memcpy(buffer + desplazamiento, RBX, 16);
    desplazamiento += 16;
    memcpy(buffer + desplazamiento, RCX, 16);
    desplazamiento += 16;
    memcpy(buffer + desplazamiento, RDX, 16);
    desplazamiento += 16;
    memcpy(buffer + desplazamiento, &PROGRAM_COUNTER, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
}

void devolver_contexto(int cliente_socket, cod_op_kernel cop) {
    int tamanio_contexto = // Por ahora fijo 
        4*4 + 4*8 + 4*16 // Registros
        + sizeof(uint32_t); // program counter

    int tamanio_paquete = sizeof(cod_op_kernel) + // cod op
        sizeof(uint32_t) + // size
        tamanio_contexto; // program counter

    void* buffer = malloc(
        tamanio_paquete // program counter
    );

    serializar_contexto(buffer, cop, tamanio_contexto);
    send(cliente_socket, buffer, tamanio_paquete, NULL);
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
        case NUEVO_CONTEXTO_PCB:
            void* buffer = recibir_buffer(cliente_socket);
            cambiar_contexto(buffer);
            printf("AAAAAA");
            imprimir_contexto_actual();
            cod_op_kernel cop = ejecutar_instrucciones(); // Ejecuta hasta encontrar un YIELD o EXIT
            // devolver_contexto(cliente_socket, cop);
            break;
        default:
            log_error(logger, "Algo anduvo mal en el server de CPU");
            log_info(logger, "Cop: %d", cop);
            return;
        }
    }

    log_warning(logger, "El cliente se desconectó del server");
    return;
}
