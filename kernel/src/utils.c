#include "utils.h"

t_list *deserializar_instrucciones(void *stream, uint32_t tam_instrucciones)
{
    int cant_instrucciones = tam_instrucciones / sizeof(t_instruccion);

    t_list *lista_instrucciones = list_create();

    int desplazamiento = 0;
    for (int i = 0; i < cant_instrucciones; i++)
    {
        t_instruccion *instruccion = malloc(sizeof(t_instruccion));
        memcpy(&instruccion->instruccion, stream + desplazamiento, sizeof(char[20]));
        memcpy(&instruccion->arg1, stream + desplazamiento + sizeof(char[20]), sizeof(char[20]));
        memcpy(&instruccion->arg2, stream + desplazamiento + sizeof(char[20]) * 2, sizeof(char[20]));
        memcpy(&instruccion->arg3, stream + desplazamiento + sizeof(char[20]) * 3, sizeof(char[20]));
        list_add(lista_instrucciones, instruccion);
        desplazamiento += sizeof(t_instruccion);
    }

    return lista_instrucciones;
}

t_pcb *crear_pcb(t_list *instrucciones, double estimacion_inicial)
{
    t_pcb *pcb = malloc(sizeof(t_pcb));

    pthread_mutex_lock(&mutex_next_pid);
    pcb->pid = next_pid;
    next_pid++;
    pthread_mutex_unlock(&mutex_next_pid);

    pcb->instrucciones = instrucciones;
    pcb->program_counter = 0;

    pcb->registros_cpu = malloc(4 * 4 + 8 * 4 + 16 * 4);
    pcb->registros_cpu->AX = malloc(4);
    pcb->registros_cpu->BX = malloc(4);
    pcb->registros_cpu->CX = malloc(4);
    pcb->registros_cpu->DX = malloc(4);
    pcb->registros_cpu->EAX = malloc(8);
    pcb->registros_cpu->EBX = malloc(8);
    pcb->registros_cpu->ECX = malloc(8);
    pcb->registros_cpu->EDX = malloc(8);
    pcb->registros_cpu->RAX = malloc(16);
    pcb->registros_cpu->RBX = malloc(16);
    pcb->registros_cpu->RCX = malloc(16);
    pcb->registros_cpu->RDX = malloc(16);

    pcb->estimado_HRRN = estimacion_inicial;
    pcb->tiempo_ready = time(NULL);
    pcb->archivos_abiertos = list_create();

    return pcb;
}

void imprimir_pcb(t_pcb *pcb)
{
    printf("PID: %d\n", pcb->pid);
    printf("Instrucciones:\n");
    for (int i = 0; i < pcb->instrucciones->elements_count; i++)
    {
        char *instruccion = list_get(pcb->instrucciones, i);
        printf("\t%d: %s\n", i, instruccion);
    }
    printf("Program Counter: %d\n", pcb->program_counter);
    printf("Registros CPU:\n");
    printf("\tAX: %p\n", pcb->registros_cpu->AX);
    printf("\tBX: %p\n", pcb->registros_cpu->BX);
    printf("\tCX: %p\n", pcb->registros_cpu->CX);
    printf("\tDX: %p\n", pcb->registros_cpu->DX);
    printf("\tEAX: %p\n", pcb->registros_cpu->EAX);
    printf("\tEBX: %p\n", pcb->registros_cpu->EBX);
    printf("\tECX: %p\n", pcb->registros_cpu->ECX);
    printf("\tEDX: %p\n", pcb->registros_cpu->EDX);
    printf("\tRAX: %p\n", pcb->registros_cpu->RAX);
    printf("\tRBX: %p\n", pcb->registros_cpu->RBX);
    printf("\tRCX: %p\n", pcb->registros_cpu->RCX);
    printf("\tRDX: %p\n", pcb->registros_cpu->RDX);
    printf("Tabla de Segmentos:\n");
    printf("Estimado HRRN: %f\n", pcb->estimado_HRRN);
    printf("Tiempo ready: %ld\n", (long)pcb->tiempo_ready);
    printf("Archivos abiertos:\n");
}

void inicializar_colas() {
    NEW = list_create();
    READY = list_create();
    BLOCKED = list_create();
}

void inicializar_semaforos() {
    if (pthread_mutex_init(&mutex_next_pid, NULL) != 0) {
        log_error(logger_kernel_extra, "No se pudo inicializar el semaforo para next_pid");
    }
}

void encolar_proceso(t_pcb* new_pcb, t_list* cola, pthread_mutex_t mutex_cola) {
    pthread_mutex_lock(&mutex_cola); // Wait
    list_add(cola, new_pcb);c
    pthread_mutex_unlock(&mutex_cola); // Signal
}

void loggear_cambio_estado(char* estado_anterior, char* estado_actual, t_pcb* pcb) {
    log_info(logger_kernel, "PID: %d - Estado anterior: %s - Estado actual: %s", pcb->pid, estado_anterior, estado_actual);
}

void loggear_cola_ready() {
    // TODO: que cambie segun el algoritmo
    char* lista_pids = string_new();

    uint32_t first_pid = ((t_pcb *)(list_get(READY, 0)))->pid;
    string_append(&lista_pids, string_itoa(first_pid));

    for (int i = 1; i < list_size(READY); i++)
    {
        t_pcb* pcb = list_get(READY, i);
        string_append(&lista_pids, ",");
        string_append(&lista_pids, string_itoa(pcb->pid));
    }

    log_info(logger_kernel, "Cola Ready FIFO: [%s]", lista_pids);
}

void planificador_largo_plazo() {
    uint32_t grado_multiprogramacion = 0;
    while(true) {
        pthread_mutex_lock(&mutex_NEW);
        if (list_size(NEW) > 0 && grado_multiprogramacion < GRADO_MAX_MULTIPROGRAMACION)
        {
            t_pcb* new_pcb = list_remove(NEW, 0);
            pthread_mutex_unlock(&mutex_NEW);

            encolar_proceso(new_pcb, READY, mutex_READY);
            grado_multiprogramacion++;

            loggear_cambio_estado("NEW", "READY", new_pcb);
            loggear_cola_ready();
            // TODO: enviar mensaje a memoria para que inicialice las estructuras necesarias

            // SACAR PRINT, USADO SOLO EN PRUEBAS RAPIDAS
        }
        pthread_mutex_unlock(&mutex_NEW);

        // TODO: if, si el PC apunta a EXIT, mandar el PCB a EXIT (o free)


        //     // SACAR PRINT, USADO SOLO EN PRUEBAS RAPIDAS
        // printf("Cola NEW\n");
        //     for (int i = 0; i < list_size(NEW); i++)
        //     {
        //         t_pcb* pcb = list_get(NEW, i);
        //         printf("PID: %d\n", pcb->pid);
        //     }
    }
}

t_pcb* siguiente_proceso_a_ejecutar() {
    if(strcmp(ALGORITMO_PLANIFICACION, "FIFO") == 0) {
        // Saca un proceso de ready segun FIFO
        return (t_pcb*)(list_remove(READY, 0));
    } else if (strcmp(ALGORITMO_PLANIFICACION, "HRRN") == 0) {
        // Saca un proceso de ready segun HRRN
    }

    log_error(logger_kernel_extra, "El algoritmo indicado en el archivo de configuracion es desconocido");
    exit(-1);
}

void planificador_corto_plazo() {
    // CHECKPOINT 2: solo FIFO
    // TODO: algoritmo HRRN

    while(true) {
        pthread_mutex_lock(&mutex_READY);
        pthread_mutex_lock(&mutex_RUNNING);
        if(list_size(READY) > 0 && RUNNING == NULL){

            t_pcb* r_pcb = siguiente_proceso_a_ejecutar();
            RUNNING = r_pcb;
            loggear_cambio_estado("READY", "RUNNING", r_pcb);

            // TODO: Envio del proceso a la CPU
            // Lo que haria es, serializar el pcb, mandarlo a la CPU
            // y esperar a que lo devuelva con los registros actualizados 
            pthread_mutex_unlock(&mutex_READY);
            
            pthread_mutex_unlock(&mutex_RUNNING);

        }
    }
}
