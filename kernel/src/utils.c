#include "utils.h"

t_pcb *crear_pcb(t_list *instrucciones)
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

    pcb->estimado_HRRN = ESTIMACION_INICIAL;
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
        t_instruccion *instruccion = list_get(pcb->instrucciones, i);
        printf("\t%d: %s %s %s %s\n", i, instruccion -> instruccion, instruccion -> arg1, instruccion -> arg2, instruccion -> arg3);
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


void levantar_config_kernel() {
			CONFIG_KERNEL = config_create("./cfg/kernel.config");

			PUERTO_ESCUCHA_KERNEL = config_get_string_value(CONFIG_KERNEL, "PUERTO_ESCUCHA");
			IP_MEMORIA = config_get_string_value(CONFIG_KERNEL, "IP_MEMORIA");
			PUERTO_MEMORIA = config_get_string_value(CONFIG_KERNEL, "PUERTO_MEMORIA");
			IP_FILESYSTEM = config_get_string_value(CONFIG_KERNEL, "IP_FILESYSTEM");
			PUERTO_FILESYSTEM = config_get_string_value(CONFIG_KERNEL, "PUERTO_FILESYSTEM");
			IP_CPU = config_get_string_value(CONFIG_KERNEL, "IP_CPU");
			PUERTO_CPU = config_get_string_value(CONFIG_KERNEL, "PUERTO_CPU");
			ALGORITMO_PLANIFICACION = config_get_string_value(CONFIG_KERNEL, "ALGORITMO_PLANIFICACION");
			ESTIMACION_INICIAL = config_get_double_value(CONFIG_KERNEL, "ESTIMACION_INICIAL");
			// char* HRRN_ALFA = config_get_string_value(CONFIG_KERNEL, "HRRN_ALFA");
			GRADO_MAX_MULTIPROGRAMACION = config_get_int_value(CONFIG_KERNEL, "GRADO_MAX_MULTIPROGRAMACION");
			// char* RECURSOS = config_get_string_value(CONFIG_KERNEL, "RECURSOS");
			// char* INSTANCIAS_RECURSOS = config_get_string_value(CONFIG_KERNEL, "INSTANCIAS_RECURSOS");
}

void inicializar_colas() {
    NEW = list_create();
    READY = list_create();
    BLOCKED = list_create();
    RUNNING = NULL;
}

void inicializar_semaforos() {
    if (pthread_mutex_init(&mutex_next_pid, NULL) != 0) {
        log_error(logger_kernel_extra, "No se pudo inicializar el semaforo para next_pid");
        exit(-1);
    }
    if (pthread_mutex_init(&mutex_NEW, NULL) != 0) {
        log_error(logger_kernel_extra, "No se pudo inicializar el semaforo para la cola de NEW");
        exit(-1);
    }
    if (pthread_mutex_init(&mutex_READY, NULL) != 0) {
        log_error(logger_kernel_extra, "No se pudo inicializar el semaforo para la cola de READY");
        exit(-1);
    }
    if (pthread_mutex_init(&mutex_RUNNING, NULL) != 0) {
        log_error(logger_kernel_extra, "No se pudo inicializar el semaforo para RUNNING");
        exit(-1);
    }
    if (pthread_mutex_init(&mutex_BLOCKED, NULL) != 0) {
        log_error(logger_kernel_extra, "No se pudo inicializar el semaforo para la cola de BLOCKED");
        exit(-1);
    }
    if (pthread_mutex_init(&mutex_EXIT, NULL) != 0) {
        log_error(logger_kernel_extra, "No se pudo inicializar el semaforo para la cola de EXIT");
        exit(-1);
    }
}

void encolar_proceso(t_pcb* new_pcb, t_list* cola, pthread_mutex_t *mutex_cola) {
    pthread_mutex_lock(mutex_cola); // Wait
    list_add(cola, new_pcb);
    pthread_mutex_unlock(mutex_cola); // Signal
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

t_pcb* siguiente_proceso_a_ejecutar() {
    if(strcmp(ALGORITMO_PLANIFICACION, "FIFO") == 0) {
        if(list_size(READY) > 0) {
            // Saca un proceso de ready segun FIFO
            return (t_pcb*)(list_remove(READY, 0));
        } else {
            // No hay procesos en READY
            return NULL;
        }
    } else if (strcmp(ALGORITMO_PLANIFICACION, "HRRN") == 0) {
        // Saca un proceso de ready segun HRRN
        // TODO: Implementar HRRN
    }

    log_error(logger_kernel_extra, "El algoritmo indicado en el archivo de configuracion es desconocido");
    exit(-1);
}

void planificacion() {
    uint32_t grado_multiprogramacion = 0;

    while(true) {
        pthread_mutex_lock(&mutex_NEW);

        // PASO de NEW a READY
        if (list_size(NEW) > 0 && grado_multiprogramacion < GRADO_MAX_MULTIPROGRAMACION)
        {
            t_pcb* new_pcb = list_remove(NEW, 0);
            pthread_mutex_unlock(&mutex_NEW);


            list_add(READY, new_pcb);
            loggear_cambio_estado("NEW", "READY", new_pcb);
            loggear_cola_ready();

            grado_multiprogramacion++;
            // TODO: enviar mensaje a memoria para que inicialice las estructuras necesarias

            // SACAR PRINT, USADO SOLO EN PRUEBAS RAPIDAS
        } 
        else {
            pthread_mutex_unlock(&mutex_NEW);
        }

        // Paso de READY a RUNNING
        if(list_size(READY) > 0 && RUNNING == NULL){

            t_pcb* r_pcb = siguiente_proceso_a_ejecutar();

            RUNNING = r_pcb;
            
            // TODO:
            // Mandar a CPU
            // Recibir respuesta de CPU, 
            // decidir en base a eso, 
            // si mandar a READY o a EXIT

            loggear_cambio_estado("READY", "RUNNING", r_pcb);
        }

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