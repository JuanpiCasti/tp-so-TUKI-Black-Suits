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
            t_pcb *n_pcb = crear_pcb(instrucciones, cliente_socket);
            // imprimir_pcb(n_pcb);
            pthread_mutex_lock(&mutex_NEW);
            list_add(NEW, n_pcb);
            log_info(logger_kernel, "Se crea el proceso %d en NEW", n_pcb->pid);
            pthread_mutex_unlock(&mutex_NEW);
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

void *serializar_contexto_pcb(t_pcb *pcb, uint32_t tam_contexto)
{
    // Se envia:
    // cod_op, (sizeof(cod_op))
    // PID, sizeof(uint32_t)
    // registros CPU, (4x4 bytes, 4x8 bytes, 4x16 bytes, en orden alfabetico)
    // uint32, program_counter (4 bytes)
    // uint32, tamanio de las instrucciones
    // instrucciones...

    uint32_t tam_instrucciones = list_size(pcb->instrucciones) * sizeof(t_instruccion);
    void *buffer = malloc(tam_contexto + sizeof(cod_op) + sizeof(uint32_t));

    uint32_t desplazamiento = 0;
    cod_op cop = NUEVO_CONTEXTO_PCB;

    memcpy(buffer, &cop, sizeof(cod_op));
    desplazamiento += sizeof(cod_op);

    memcpy(buffer + desplazamiento, &tam_contexto, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer + desplazamiento, &(pcb->pid), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    // Registros CPU
    memcpy(buffer + desplazamiento, &(pcb->registros_cpu->AX), 4);
    desplazamiento += 4;
    memcpy(buffer + desplazamiento, &(pcb->registros_cpu->BX), 4);
    desplazamiento += 4;
    memcpy(buffer + desplazamiento, &(pcb->registros_cpu->CX), 4);
    desplazamiento += 4;
    memcpy(buffer + desplazamiento, &(pcb->registros_cpu->DX), 4);
    desplazamiento += 4;

    memcpy(buffer + desplazamiento, &(pcb->registros_cpu->EAX), 8);
    desplazamiento += 8;
    memcpy(buffer + desplazamiento, &(pcb->registros_cpu->EBX), 8);
    desplazamiento += 8;
    memcpy(buffer + desplazamiento, &(pcb->registros_cpu->ECX), 8);
    desplazamiento += 8;
    memcpy(buffer + desplazamiento, &(pcb->registros_cpu->EDX), 8);
    desplazamiento += 8;

    memcpy(buffer + desplazamiento, &(pcb->registros_cpu->RAX), 16);
    desplazamiento += 16;
    memcpy(buffer + desplazamiento, &(pcb->registros_cpu->RBX), 16);
    desplazamiento += 16;
    memcpy(buffer + desplazamiento, &(pcb->registros_cpu->RCX), 16);
    desplazamiento += 16;
    memcpy(buffer + desplazamiento, &(pcb->registros_cpu->RDX), 16);
    desplazamiento += 16;

    // Program counter
    memcpy(buffer + desplazamiento, &(pcb->program_counter), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    // Instrucciones
    void *buffer_instrucciones = serializar_instrucciones(pcb->instrucciones, list_size(pcb->instrucciones), tam_instrucciones);
    memcpy(buffer + desplazamiento, buffer_instrucciones, sizeof(uint32_t) + tam_instrucciones);
    free(buffer_instrucciones);

    return buffer;
}

void deserializar_contexto_pcb(void *buffer, t_pcb *pcb)
{
    int desplazamiento = 0;

    memcpy(&(pcb->registros_cpu->AX), buffer + desplazamiento, 4);
    desplazamiento += 4;
    memcpy(&(pcb->registros_cpu->BX), buffer + desplazamiento, 4);
    desplazamiento += 4;
    memcpy(&(pcb->registros_cpu->CX), buffer + desplazamiento, 4);
    desplazamiento += 4;
    memcpy(&(pcb->registros_cpu->DX), buffer + desplazamiento, 4);
    desplazamiento += 4;
    memcpy(&(pcb->registros_cpu->EAX), buffer + desplazamiento, 8);
    desplazamiento += 8;
    memcpy(&(pcb->registros_cpu->EBX), buffer + desplazamiento, 8);
    desplazamiento += 8;
    memcpy(&(pcb->registros_cpu->ECX), buffer + desplazamiento, 8);
    desplazamiento += 8;
    memcpy(&(pcb->registros_cpu->EDX), buffer + desplazamiento, 8);
    desplazamiento += 8;
    memcpy(&(pcb->registros_cpu->RAX), buffer + desplazamiento, 16);
    desplazamiento += 16;
    memcpy(&(pcb->registros_cpu->RBX), buffer + desplazamiento, 16);
    desplazamiento += 16;
    memcpy(&(pcb->registros_cpu->RCX), buffer + desplazamiento, 16);
    desplazamiento += 16;
    memcpy(&(pcb->registros_cpu->RDX), buffer + desplazamiento, 16);
    desplazamiento += 16;
    memcpy(&(pcb->program_counter), buffer + desplazamiento, sizeof(uint32_t));

}

int mandar_a_cpu(t_pcb *pcb, uint32_t tam_contexto)
{
    int socket_cpu = crear_conexion(logger_kernel_extra, IP_CPU, PUERTO_CPU);

    uint32_t tam_paquete = tam_contexto + sizeof(uint32_t) + sizeof(cod_op);
    void *buffer = serializar_contexto_pcb(pcb, tam_contexto);

    send(socket_cpu, buffer, tam_paquete, NULL);

    return socket_cpu;
}

void *recibir_nuevo_contexto(int socket_cpu, cod_op_kernel *cop)
{
    recv(socket_cpu, cop, sizeof(cod_op_kernel), NULL);

    uint32_t size;
    recv(socket_cpu, &size, sizeof(uint32_t), NULL);

    void *buffer = malloc(size);
    recv(socket_cpu, buffer, size, NULL);
    

    close(socket_cpu);
    return buffer;
}


void devolver_resultado(t_pcb* pcb, cod_op_kernel exit_code) {
    send(pcb->socket_consola, &exit_code, sizeof(cod_op_kernel), NULL);
}