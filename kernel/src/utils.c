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
        printf("\t%d: %s %s %s %s\n", i, instruccion->instruccion, instruccion->arg1, instruccion->arg2, instruccion->arg3);
    }
    printf("Program Counter: %d\n", pcb->program_counter);
    printf("Registros CPU:\n");
    printf("\tAX: %s\n", pcb->registros_cpu->AX);
    printf("\tBX: %s\n", pcb->registros_cpu->BX);
    printf("\tCX: %s\n", pcb->registros_cpu->CX);
    printf("\tDX: %s\n", pcb->registros_cpu->DX);
    printf("\tEAX: %s\n", pcb->registros_cpu->EAX);
    printf("\tEBX: %s\n", pcb->registros_cpu->EBX);
    printf("\tECX: %s\n", pcb->registros_cpu->ECX);
    printf("\tEDX: %s\n", pcb->registros_cpu->EDX);
    printf("\tRAX: %s\n", pcb->registros_cpu->RAX);
    printf("\tRBX: %s\n", pcb->registros_cpu->RBX);
    printf("\tRCX: %s\n", pcb->registros_cpu->RCX);
    printf("\tRDX: %s\n", pcb->registros_cpu->RDX);
    printf("Tabla de Segmentos:\n");
    printf("Estimado HRRN: %f\n", pcb->estimado_HRRN);
    printf("Tiempo ready: %ld\n", (long)pcb->tiempo_ready);
    printf("Archivos abiertos:\n");
}

void inicializar_loggers_kernel()
{
    logger_kernel_extra = log_create("./log/kernel_extra.log", "KERNEL_EXTRA", false, LOG_LEVEL_INFO);
    logger_kernel = log_create("./log/kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
}

void levantar_config_kernel()
{
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

void inicializar_colas()
{
    NEW = list_create();
    READY = list_create();
    BLOCKED = list_create();
    RUNNING = NULL;
}

void inicializar_semaforos()
{
    if (pthread_mutex_init(&mutex_next_pid, NULL) != 0)
    {
        log_error(logger_kernel_extra, "No se pudo inicializar el semaforo para next_pid");
        exit(-1);
    }
    if (pthread_mutex_init(&mutex_NEW, NULL) != 0)
    {
        log_error(logger_kernel_extra, "No se pudo inicializar el semaforo para la cola de NEW");
        exit(-1);
    }
    if (pthread_mutex_init(&mutex_READY, NULL) != 0)
    {
        log_error(logger_kernel_extra, "No se pudo inicializar el semaforo para la cola de READY");
        exit(-1);
    }
    if (pthread_mutex_init(&mutex_RUNNING, NULL) != 0)
    {
        log_error(logger_kernel_extra, "No se pudo inicializar el semaforo para RUNNING");
        exit(-1);
    }
    if (pthread_mutex_init(&mutex_BLOCKED, NULL) != 0)
    {
        log_error(logger_kernel_extra, "No se pudo inicializar el semaforo para la cola de BLOCKED");
        exit(-1);
    }
    if (pthread_mutex_init(&mutex_EXIT, NULL) != 0)
    {
        log_error(logger_kernel_extra, "No se pudo inicializar el semaforo para la cola de EXIT");
        exit(-1);
    }
}

void encolar_proceso(t_pcb *new_pcb, t_list *cola, pthread_mutex_t *mutex_cola)
{
    pthread_mutex_lock(mutex_cola); // Wait
    list_add(cola, new_pcb);
    pthread_mutex_unlock(mutex_cola); // Signal
}

void loggear_cambio_estado(char *estado_anterior, char *estado_actual, t_pcb *pcb)
{
    log_info(logger_kernel, "PID: %d - Estado anterior: %s - Estado actual: %s", pcb->pid, estado_anterior, estado_actual);
}

void loggear_cola_ready()
{
    char *lista_pids = string_new();

    uint32_t first_pid = ((t_pcb *)(list_get(READY, 0)))->pid;
    string_append(&lista_pids, string_itoa(first_pid));

    for (int i = 1; i < list_size(READY); i++)
    {
        t_pcb *pcb = list_get(READY, i);
        string_append(&lista_pids, ",");
        string_append(&lista_pids, string_itoa(pcb->pid));
    }

    log_info(logger_kernel, "Cola Ready FIFO: [%s]", lista_pids);
}

t_pcb *siguiente_proceso_a_ejecutar()
{
    if (strcmp(ALGORITMO_PLANIFICACION, "FIFO") == 0)
    {
        if (list_size(READY) > 0)
        {
            // Saca un proceso de ready segun FIFO
            return (t_pcb *)(list_remove(READY, 0));
        }
        else
        {
            // No hay procesos en READY
            return NULL;
        }
    }
    else if (strcmp(ALGORITMO_PLANIFICACION, "HRRN") == 0)
    {
        // Saca un proceso de ready segun HRRN
    }

    log_error(logger_kernel_extra, "El algoritmo indicado en el archivo de configuracion es desconocido");
    exit(-1);
}

void planificacion()
{
    uint32_t grado_multiprogramacion = 0;

    while (true)
    {
        pthread_mutex_lock(&mutex_NEW);

        // PASO de NEW a READY - Largo Plazo
        if (list_size(NEW) > 0 && grado_multiprogramacion < GRADO_MAX_MULTIPROGRAMACION)
        {
            t_pcb *new_pcb = list_remove(NEW, 0);
            pthread_mutex_unlock(&mutex_NEW);

            list_add(READY, new_pcb);
            loggear_cambio_estado("NEW", "READY", new_pcb);
            loggear_cola_ready();

            grado_multiprogramacion++;
        }
        else
        {
            pthread_mutex_unlock(&mutex_NEW);
        }

        // Paso de READY a RUNNING - Corto Plazo
        if (list_size(READY) > 0 && RUNNING == NULL)
        {
            t_pcb *r_pcb = siguiente_proceso_a_ejecutar();

            RUNNING = r_pcb;
            loggear_cambio_estado("READY", "RUNNING", r_pcb);
            uint32_t tam_contexto = sizeof(cod_op) +
                                    4 * 4 +  // (AX, BX, CX, DX)
                                    4 * 8 +  // (EAX, EBX, ECX, EDX)
                                    4 * 16 + // (RAX, RBX, RCX, RDX)
                                    sizeof(uint32_t) +
                                    sizeof(uint32_t) +
                                    list_size(r_pcb->instrucciones) * sizeof(t_instruccion);

            int socket_cpu = mandar_a_cpu(RUNNING, tam_contexto);
            // cod_op_kernel cop;
            // void* buffer = recibir_nuevo_contexto(socket_cpu, &cop);
            // deserializar_contexto_pcb(buffer, r_pcb);

            // switch (cop)
            // {
            // case CPU_YIELD:
            //     encolar_proceso(r_pcb, READY, &mutex_READY);
            //     RUNNING = NULL;
            //     break;
            // case CPU_EXIT:
            //     encolar_proceso(r_pcb, EXIT, &mutex_EXIT);
            //     RUNNING = NULL;
            //     break;
            // default:
            //     break;
            }

        }
}

