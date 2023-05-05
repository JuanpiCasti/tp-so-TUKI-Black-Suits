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
            t_pcb *n_pcb = crear_pcb(instrucciones);
            imprimir_pcb(n_pcb);
            encolar_proceso(n_pcb, NEW, &mutex_NEW);
            log_info(logger_kernel, "Se crea el proceso %d en NEW", n_pcb->pid);

            // SACAR PRINT, USADO SOLO EN PRUEBAS RAPIDAS
            // printf("Cola NEW\n");
            // for (int i = 0; i < list_size(NEW); i++)
            // {
            //     t_pcb* pcb = list_get(NEW, i);
            //     printf("PID: %d\n", pcb->pid);
            // }
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

void* serializar_contexto_pcb(t_pcb* pcb) {
    // Se envia:
    // cod_op, (sizeof(cod_op))
    // registros CPU, (4x4 bytes, 4x8 bytes, 4x16 bytes, en orden alfabetico)
    // uint32, program_counter (4 bytes)
    // uint32, tamanio de las instrucciones
    // instrucciones...
    
    uint32_t tam_instrucciones = list_size(pcb -> instrucciones) * sizeof(t_instruccion); 
    void* buffer = malloc(sizeof(cod_op) + 
                          4*4 + // (AX, BX, CX, DX) 
                          4*8 + // (EAX, EBX, ECX, EDX)
                          4*16 + // (RAX, RBX, RCX, RDX)
                          sizeof(uint32_t) +
                          sizeof(uint32_t) +
                          tam_instrucciones
                          );
    
    uint32_t desplazamiento = 0;
    cod_op cop = NUEVO_CONTEXTO_PCB;

    memcpy(buffer, &cop, sizeof(cod_op));
    desplazamiento += sizeof(cod_op);
    
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
    void* buffer_instrucciones = serializar_instrucciones(pcb->instrucciones, sizeof(pcb->instrucciones), tam_instrucciones);
    memcpy(buffer + desplazamiento, buffer_instrucciones, sizeof(uint32_t) + tam_instrucciones);
    free(buffer_instrucciones);

    return buffer;

}

void enviar_contexto_a_cpu(t_pcb* pcb) {

    void* buffer = serializar_contexto_pcb(pcb);

    //conn

    free(buffer);
}