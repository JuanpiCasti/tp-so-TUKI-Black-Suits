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
            //imprimir_pcb(n_pcb);
            pthread_mutex_lock(&mutex_NEW);
            list_add(NEW, n_pcb);
            log_info(logger_kernel, "Se crea el proceso %d en NEW", n_pcb->pid);
            pthread_mutex_unlock(&mutex_NEW);


            pthread_mutex_lock(&mutex_PROCESOS_EN_MEMORIA);
            list_add(PROCESOS_EN_MEMORIA, n_pcb);
            pthread_mutex_unlock(&mutex_PROCESOS_EN_MEMORIA);
            sem_post(&semaforo_NEW);

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

t_pcb *crear_pcb(t_list *instrucciones, int socket_consola)
{
    t_pcb *pcb = malloc(sizeof(t_pcb));

    pthread_mutex_lock(&mutex_next_pid);
    pcb->pid = next_pid;
    next_pid++;
    pthread_mutex_unlock(&mutex_next_pid);

    pcb->instrucciones = instrucciones;
    pcb->program_counter = 0;

    t_registros_cpu *registros_cpu = malloc(sizeof(t_registros_cpu));
    memset(registros_cpu->AX, 0, sizeof(registros_cpu->AX));
    memset(registros_cpu->BX, 0, sizeof(registros_cpu->BX));
    memset(registros_cpu->CX, 0, sizeof(registros_cpu->CX));
    memset(registros_cpu->DX, 0, sizeof(registros_cpu->DX));
    memset(registros_cpu->EAX, 0, sizeof(registros_cpu->EAX));
    memset(registros_cpu->EBX, 0, sizeof(registros_cpu->EBX));
    memset(registros_cpu->ECX, 0, sizeof(registros_cpu->ECX));
    memset(registros_cpu->EDX, 0, sizeof(registros_cpu->EDX));
    memset(registros_cpu->RAX, 0, sizeof(registros_cpu->RAX));
    memset(registros_cpu->RBX, 0, sizeof(registros_cpu->RBX));
    memset(registros_cpu->RCX, 0, sizeof(registros_cpu->RCX));
    memset(registros_cpu->RDX, 0, sizeof(registros_cpu->RDX));
    pcb->registros_cpu = registros_cpu;

    pcb->estimado_HRRN = ESTIMACION_INICIAL;
    pcb->llegada_ready = 0;
    pcb->ultima_rafaga = 0;
    pcb->archivos_abiertos = list_create();
    pcb->socket_consola = socket_consola;
    pcb->recursos_asignados = list_create();

    
    pcb -> tabla_segmentos = solicitar_tabla_segmentos(logger_kernel_extra, IP_MEMORIA, PUERTO_MEMORIA, pcb->pid);

    return pcb;
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
    desplazamiento += sizeof(uint32_t) + tam_instrucciones;

    //imprimir_pcb(pcb);
    // Tabla de segmentos
    uint32_t tam_tabla_segmentos = sizeof(t_ent_ts) * list_size(pcb->tabla_segmentos) + sizeof(uint32_t);
    void *buffer_tabla_segmentos = serializar_tabla_segmentos(pcb->tabla_segmentos, tam_tabla_segmentos);
    memcpy(buffer + desplazamiento, buffer_tabla_segmentos, tam_tabla_segmentos);
    free(buffer_tabla_segmentos);
    desplazamiento += tam_tabla_segmentos;

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

void mandar_a_cpu(t_pcb *pcb, uint32_t tam_contexto)
{
    //int socket_cpu = crear_conexion(logger_kernel_extra, IP_CPU, PUERTO_CPU);

    uint32_t tam_paquete = tam_contexto + sizeof(cod_op) + sizeof(uint32_t);
    void *buffer = serializar_contexto_pcb(pcb, tam_contexto);

    send(socket_cpu, buffer, tam_paquete, NULL);
    free(buffer);
    //return socket_cpu;
}

void *recibir_nuevo_contexto(int socket_cpu, cod_op_kernel *cop)
{
    recv(socket_cpu, cop, sizeof(cod_op_kernel), NULL);

    uint32_t size;
    recv(socket_cpu, &size, sizeof(uint32_t), NULL);

    void *buffer = malloc(size);
    recv(socket_cpu, buffer, size, NULL);
    

    //close(socket_cpu);
    return buffer;
}


void devolver_resultado(t_pcb* pcb, cod_op_kernel exit_code) {
    send(pcb->socket_consola, &exit_code, sizeof(cod_op_kernel), NULL);
}

t_pcb* buscar_proceso_en_memoria(uint32_t pid) {
    t_pcb* pcb;
    for (int i = 0; i < list_size(PROCESOS_EN_MEMORIA); i++)
    {
        pcb = list_get(PROCESOS_EN_MEMORIA, i);
        if (pcb->pid == pid)
        {
            return pcb;
        }
    }
}

t_list* deserializar_tabla_segmentos(void* buffer, uint32_t size) {
    t_list* tabla = list_create();
    uint32_t despl = 0;

    t_ent_ts* entrada;
    
    for (int i = 0; i < size; i++)
    {

        entrada = malloc(sizeof(t_ent_ts));

        memcpy(&entrada->id_seg, buffer + despl, sizeof(uint32_t));
        despl += sizeof(uint32_t);

        memcpy(&entrada->base, buffer + despl, sizeof(uint32_t));
        despl += sizeof(uint32_t);

        memcpy(&entrada->tam, buffer + despl, sizeof(uint32_t));
        despl += sizeof(uint32_t);

        memcpy(&entrada->activo, buffer + despl, sizeof(uint8_t));
        despl += sizeof(uint8_t);

        list_add(tabla, entrada);

    }
    return tabla;
}

t_list* solicitar_tabla_segmentos(t_log* logger, char* ip, char* puerto, uint32_t pid) {
    cod_op cod = CREATE_SEGTABLE;
	void* buffer_pid = malloc(sizeof(cod_op) + sizeof(uint32_t));

	memcpy(buffer_pid, &cod, sizeof(cod_op));
	memcpy(buffer_pid + sizeof(cod_op), &pid, sizeof(uint32_t));

    send(socket_memoria, buffer_pid, sizeof(cod_op) + sizeof(uint32_t), NULL);
	free(buffer_pid);
    uint32_t size;
    recv(socket_memoria, &size, sizeof(uint32_t), NULL);

    void* buffer = malloc(size * sizeof(t_ent_ts));
    recv(socket_memoria, buffer, size * sizeof(t_ent_ts), NULL);

    t_list* tabla = deserializar_tabla_segmentos(buffer, size);
    free(buffer);
    return tabla;
}

void recibir_nuevas_bases(int socket_memoria) {
    uint32_t tam_buffer;
    recv(socket_memoria, &tam_buffer, sizeof(uint32_t), NULL);
    tam_buffer -= sizeof(uint32_t);

    void *buffer = malloc(tam_buffer);
    recv(socket_memoria, buffer, tam_buffer, NULL);
    int desplazamiento = 0;

    int cant_segmentos = tam_buffer / (sizeof(uint32_t) * 3);
    
    uint32_t pid;
    uint32_t id;
    uint32_t n_base;

    for (int i = 0; i < cant_segmentos; i++)
    {
        memcpy(&pid, buffer + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(&id, buffer + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(&n_base, buffer + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        // Search PID in PROCESOS_EN_MEMORIA
        t_pcb *proceso_en_memoria = buscar_proceso_en_memoria(pid);
        
        t_ent_ts *segmento = list_get(proceso_en_memoria->tabla_segmentos, id);
        segmento ->base = n_base;
        
    }
    

    free(buffer);

}


