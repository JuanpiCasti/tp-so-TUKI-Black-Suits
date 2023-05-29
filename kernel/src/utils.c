#include "utils.h"

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
    pcb->tiempo_ready = time(NULL);
    pcb->archivos_abiertos = list_create();
    pcb->socket_consola = socket_consola;

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
    printf("\tAX: %s\n", imprimir_cadena(pcb->registros_cpu->AX, 4));
    printf("\tBX: %s\n", imprimir_cadena(pcb->registros_cpu->BX, 4));
    printf("\tCX: %s\n", imprimir_cadena(pcb->registros_cpu->CX, 4));
    printf("\tDX: %s\n", imprimir_cadena(pcb->registros_cpu->DX, 4));
    printf("\tEAX: %s\n", imprimir_cadena(pcb->registros_cpu->EAX, 8));
    printf("\tEBX: %s\n", imprimir_cadena(pcb->registros_cpu->EBX, 8));
    printf("\tECX: %s\n", imprimir_cadena(pcb->registros_cpu->ECX, 8));
    printf("\tEDX: %s\n", imprimir_cadena(pcb->registros_cpu->EDX, 8));
    printf("\tRAX: %s\n", imprimir_cadena(pcb->registros_cpu->RAX, 16));
    printf("\tRBX: %s\n", imprimir_cadena(pcb->registros_cpu->RBX, 16));
    printf("\tRCX: %s\n", imprimir_cadena(pcb->registros_cpu->RCX, 16));
    printf("\tRDX: %s\n", imprimir_cadena(pcb->registros_cpu->RDX, 16));
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
    RECURSOS_EXISTENTES = config_get_array_value(CONFIG_KERNEL, "RECURSOS");
    INSTANCIAS_RECURSOS = config_get_array_value(CONFIG_KERNEL, "INSTANCIAS_RECURSOS");
}

void inicializar_colas()
{
    NEW = list_create();
    READY = list_create();
    EXIT = list_create();
    RUNNING = NULL;
    RECURSOS = levantar_recursos();
}

t_recurso* crear_recurso(char* nombre, uint32_t n_instancias) {
    t_recurso* n_recurso = malloc(sizeof(t_recurso));
    
    strcpy(n_recurso -> nombre, nombre);
    n_recurso -> instancias_disponibles = n_instancias;
    n_recurso -> cola_bloqueados = list_create();

    return n_recurso;
}

t_list* levantar_recursos(){
    t_list* lista_recursos = list_create();

    int i = 0;
    while (RECURSOS_EXISTENTES[i] != NULL)
    {

        list_add(lista_recursos, crear_recurso(RECURSOS_EXISTENTES[i], atoi(INSTANCIAS_RECURSOS[i])));

        // Aniade a la lista de recursos el recurso con su numero de instancias correspondientes
        // ya que coincide su posicion dada en el archivo de config.
        // la funcion config_get_array_value devuelve un array de char*, por eso hay que convertir
        // cada elemento de INSTANCIAS_RECURSOS a un int con la funcion "atoi"

        i++;

    }

    return lista_recursos;
}

void imprimir_lista_recursos(t_list* lista) {
    int i;
    int size = list_size(lista);
    for (i = 0; i < size; i++) {
        t_recurso* recurso = list_get(lista, i);
        printf("Nombre: %s\n", recurso->nombre);
        printf("Instancias disponibles: %d\n", recurso->instancias_disponibles);
    }
}

void inicializar_semaforos()
{
    if (pthread_mutex_init(&mutex_next_pid, NULL) != 0)
    {
        log_error(logger_kernel_extra, "No se pudo inicializar el semaforo para next_pid");
        exit(-1);
    }
    if (pthread_mutex_init(&mutex_mp, NULL) != 0)
    {
        log_error(logger_kernel_extra, "No se pudo inicializar el semaforo para GRADO_ACTUAL_MPROG");
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
    if (pthread_mutex_init(&mutex_EXIT, NULL) != 0)
    {
        log_error(logger_kernel_extra, "No se pudo inicializar el semaforo para la cola de EXIT");
        exit(-1);
    }
    if (pthread_mutex_init(&mutex_RECURSOS, NULL) != 0)
    {
        log_error(logger_kernel_extra, "No se pudo inicializar el semaforo para la lista de recursos disponibles");
        exit(-1);
    }

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

void loggear_fin_proceso(t_pcb* pcb, cod_op_kernel exit_code) {
    log_info(logger_kernel, "Finaliza el proceso %d - Motivo: %s", pcb->pid, cod_op_kernel_description[exit_code]);
}

t_recurso* buscar_recurso_por_nombre(char* nombre_deseado) {
    
    for (int i = 0; i < list_size(RECURSOS); i++) {
        t_recurso* recurso = list_get(RECURSOS, i);
        if (strcmp(recurso->nombre, nombre_deseado) == 0) {
            return recurso;
        }
    }
    
    return NULL;
}
