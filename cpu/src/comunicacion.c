#include "cpu.h"

void *recibir_buffer(int cliente_socket)
{
    uint32_t size;
    recv(cliente_socket, &size, sizeof(uint32_t), NULL);
    void *buffer = malloc(size);
    recv(cliente_socket, buffer, size, NULL);
    return buffer;
}

void serializar_contexto(void *buffer, cod_op_kernel cop, int tamanio_contexto)
{
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

    if (cop == CPU_IO) {
        t_instruccion* instruccion = list_get(INSTRUCTION_LIST, PROGRAM_COUNTER - 1);
        uint32_t wait_time = atoi(instruccion->arg1);
        memcpy(buffer + desplazamiento, &wait_time, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
    }

    if (cop == CPU_WAIT || cop == CPU_SIGNAL) {
        t_instruccion* instruccion = list_get(INSTRUCTION_LIST, PROGRAM_COUNTER - 1);
        char* recurso = instruccion ->arg1;
        memcpy(buffer + desplazamiento, recurso, 20);
        desplazamiento += 20;
    }

    if (cop == CPU_CREATE_SEGMENT) {
        t_instruccion* instruccion = list_get(INSTRUCTION_LIST, PROGRAM_COUNTER - 1);
        uint32_t id_seg = atoi(instruccion -> arg1);
        uint32_t tam_seg = atoi(instruccion -> arg2);
        //printf("ID: %d, TAM: %d\n", id_seg, tam_seg);
        
        memcpy(buffer + desplazamiento, &id_seg, sizeof(uint32_t)); // ID SEGMENTO
        desplazamiento += sizeof(uint32_t);
        memcpy(buffer + desplazamiento, &tam_seg, sizeof(uint32_t)); // TAMANIO SEGMENTO
        desplazamiento += sizeof(uint32_t);
    }

    if (cop == CPU_DELETE_SEGMENT) {
        t_instruccion* instruccion = list_get(INSTRUCTION_LIST, PROGRAM_COUNTER - 1);
        uint32_t id_seg = atoi(instruccion -> arg1);
        memcpy(buffer + desplazamiento, &id_seg, sizeof(uint32_t)); // ID SEGMENTO
        desplazamiento += sizeof(uint32_t);
    }

    if (cop == CPU_F_OPEN || cop == CPU_F_CLOSE) {
        t_instruccion* instruccion = list_get(INSTRUCTION_LIST, PROGRAM_COUNTER - 1);
        char* f_name = instruccion -> arg1;
        memcpy(buffer + desplazamiento, f_name, sizeof(char[30]));
        desplazamiento += sizeof(char[30]);
    }

    if (cop == CPU_F_SEEK || cop == CPU_F_TRUNCATE) {
        t_instruccion* instruccion = list_get(INSTRUCTION_LIST, PROGRAM_COUNTER - 1);
        char* f_name = instruccion -> arg1;
        uint32_t num = atoi(instruccion -> arg2);
        memcpy(buffer + desplazamiento, f_name, sizeof(char[30]));
        desplazamiento += sizeof(char[30]);
        memcpy(buffer + desplazamiento, &num, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
    }
}

void devolver_contexto(int cliente_socket, cod_op_kernel cop)
{
    int tamanio_contexto = TAMANIO_CONTEXTO;
    
    if (cop == CPU_IO)
    {
        tamanio_contexto += sizeof(uint32_t); // Tiempo del proceso bloqueado en IO
    }

    if (cop == CPU_WAIT || cop == CPU_SIGNAL) {
        tamanio_contexto += 20;
    } 

    if(cop == CPU_CREATE_SEGMENT) {
        tamanio_contexto += sizeof(uint32_t) * 2;
    }

    if(cop == CPU_DELETE_SEGMENT) {
        tamanio_contexto += sizeof(uint32_t);
    }

    if (cop == CPU_F_OPEN || CPU_F_CLOSE) {
        tamanio_contexto += sizeof(char[30]);
    }

    if (cop == CPU_F_SEEK || CPU_F_TRUNCATE) {
        tamanio_contexto += sizeof(char[30]) + sizeof(uint32_t);
    }

    int tamanio_paquete = sizeof(cod_op_kernel) + // Cod OP
                          sizeof(uint32_t) +      // Size
                          tamanio_contexto;       // Program Counter

    void *buffer = malloc(
        tamanio_paquete // Program Counter
    );

    serializar_contexto(buffer, cop, tamanio_contexto);
    send(cliente_socket, buffer, tamanio_paquete, NULL);
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
            void *buffer = recibir_buffer(cliente_socket);
            cambiar_contexto(buffer);
            //imprimir_contexto_actual();
            cod_op_kernel cop = ejecutar_instrucciones(); // Ejecuta hasta encontrar un YIELD o EXIT
            devolver_contexto(cliente_socket, cop);
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
