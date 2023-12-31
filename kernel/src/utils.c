#include "utils.h"

void destroy_pcb(void* element) {
  t_pcb* pcb = (t_pcb*)element;
  
  // Liberar memoria de la lista de instrucciones
  if (pcb->instrucciones != NULL) {
    list_destroy_and_destroy_elements(pcb->instrucciones, destroy_instruccion);
  }
  
  // Liberar memoria de la lista de segmentos
  if (pcb->tabla_segmentos != NULL) {
    list_destroy(pcb->tabla_segmentos);
  }
  
  // Liberar memoria de la lista de archivos abiertos
  if (pcb->archivos_abiertos != NULL) {
    list_destroy(pcb->archivos_abiertos);
  }
  
  // Liberar memoria del struct t_pcb
  free(pcb);
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

    for (int i = 0; i < list_size(pcb->tabla_segmentos); i++) {
        t_ent_ts* elemento = list_get(pcb->tabla_segmentos, i);
        printf("Elemento %d:\n", i);
        printf("ID Seg: %u\n", elemento->id_seg);
        printf("Base: %u\n", elemento->base);
        printf("Tamaño: %u\n", elemento->tam);
        printf("Activo: %u\n", elemento->activo);
    }

    printf("Estimado HRRN: %f\n", pcb->estimado_HRRN);
    printf("Tiempo ready: %ld\n", (long)pcb->llegada_ready);
    printf("Archivos abiertos:\n");

}

void inicializar_loggers_kernel()
{
    logger_kernel_extra = log_create("./log/kernel_extra.log", "KERNEL_EXTRA", true, LOG_LEVEL_INFO);
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
    HRRN_ALFA = config_get_double_value(CONFIG_KERNEL, "HRRN_ALFA");
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
    PROCESOS_EN_MEMORIA = list_create();
    tabla_archivos = list_create();
}

t_recurso *crear_recurso(char *nombre, uint32_t n_instancias)
{
    t_recurso *n_recurso = malloc(sizeof(t_recurso));

    strcpy(n_recurso->nombre, nombre);
    n_recurso->instancias_disponibles = n_instancias;
    n_recurso->cola_bloqueados = list_create();

    return n_recurso;
}

t_list *levantar_recursos()
{
    t_list *lista_recursos = list_create();

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

void imprimir_lista_recursos(t_list *lista)
{
    int i;
    int size = list_size(lista);
    for (i = 0; i < size; i++)
    {
        t_recurso *recurso = list_get(lista, i);
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
    pthread_mutex_unlock(&mutex_READY);
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
    if (pthread_mutex_init(&mutex_compactacion, NULL) != 0)
    {
        log_error(logger_kernel_extra, "No se pudo inicializar el semaforo para el control de compactacion y fread-fwrite");
        exit(-1);
    }
    if (pthread_mutex_init(&mutex_PROCESOS_EN_MEMORIA, NULL) != 0)
    {
        log_error(logger_kernel_extra, "No se pudo inicializar el semaforo para la lista de procesos en memoria");
        exit(-1);
    }

    sem_init(&semaforo_NEW, 0, 0);
    sem_init(&semaforo_READY, 0, 0);
    sem_init(&semaforo_mp, 0, GRADO_MAX_MULTIPROGRAMACION);
}

void loggear_cambio_estado(char *estado_anterior, char *estado_actual, t_pcb *pcb)
{
    log_info(logger_kernel, "PID: %d - Estado anterior: %s - Estado actual: %s", pcb->pid, estado_anterior, estado_actual);
}

void loggear_cola_ready(t_list *cola_ready)
{
    char *lista_pids = string_new();
    t_pcb *pcb;

    if (strcmp(ALGORITMO_PLANIFICACION, "FIFO") == 0)
    {
        uint32_t first_pid = ((t_pcb *)(list_get(cola_ready, 0)))->pid;
        char* first_pid_str = string_itoa(first_pid);
        string_append(&lista_pids, first_pid_str);
        free(first_pid_str);

        for (int i = 1; i < list_size(cola_ready); i++)
        {
            pcb = list_get(cola_ready, i);
            string_append(&lista_pids, ",");
            char* el_pid = string_itoa(pcb->pid);
            string_append(&lista_pids, el_pid);
            free(el_pid);
        }

        log_info(logger_kernel, "Cola Ready FIFO: [%s]", lista_pids);
    }
    else if (strcmp(ALGORITMO_PLANIFICACION, "HRRN") == 0)
    {
        t_list *READY_aux = list_take(cola_ready, list_size(cola_ready));

        double RR_aux = 0;
        double RR_mayor = 0;
        int index_de_RR_mayor = 0;
        double estimado_aux;

        for (int i = 0; i < list_size(READY_aux); i++)
        {
            pcb = list_get(READY_aux, i);
            RR_aux = ((time(NULL) - pcb->llegada_ready) + pcb->estimado_HRRN) / pcb->estimado_HRRN;

            if (RR_aux > RR_mayor)
            {
                RR_mayor = RR_aux;
                index_de_RR_mayor = i;
            }
        }

        pcb = (t_pcb *)(list_remove(READY_aux, index_de_RR_mayor));
        char* first_pid_str = string_itoa(pcb->pid);
        string_append(&lista_pids, first_pid_str);
        free(first_pid_str);
        int size_cola = list_size(READY_aux);
        for (int j = 0; j < size_cola; j++)
        {
            RR_aux = 0;
            RR_mayor = 0;
            index_de_RR_mayor = 0;

            for (int i = 0; i < list_size(READY_aux); i++)
            {
                pcb = list_get(READY_aux, i);
                if (pcb->ultima_rafaga != 0)
                {
                    estimado_aux = HRRN_ALFA * pcb->estimado_HRRN + (1 - HRRN_ALFA) * pcb->ultima_rafaga;
                }

                RR_aux = ((time(NULL) - pcb->llegada_ready) + estimado_aux) / estimado_aux;

                if (RR_aux > RR_mayor)
                {
                    RR_mayor = RR_aux;
                    index_de_RR_mayor = i;
                }
            }


            pcb = (t_pcb *)(list_remove(READY_aux, index_de_RR_mayor));
            string_append(&lista_pids, ",");
            char* el_pid = string_itoa(pcb->pid);
            string_append(&lista_pids, el_pid);
            free(el_pid);
        }

        log_info(logger_kernel, "Cola Ready HRRN: [%s]", lista_pids);
        list_destroy_and_destroy_elements(READY_aux, destroy_pcb);
    }
    free(lista_pids);
}

void loggear_fin_proceso(t_pcb *pcb, cod_op_kernel exit_code)
{
    log_info(logger_kernel, "Finaliza el proceso %d - Motivo: %s", pcb->pid, cod_op_kernel_description[exit_code]);
}

t_recurso *buscar_recurso_por_nombre(char *nombre_deseado)
{

    for (int i = 0; i < list_size(RECURSOS); i++)
    {
        t_recurso *recurso = list_get(RECURSOS, i);
        if (strcmp(recurso->nombre, nombre_deseado) == 0)
        {
            return recurso;
        }
    }

    return NULL;
}

int recurso_asignado(t_pcb* proceso, char* nombre_recurso) {
    for (int i = 0; i < list_size(proceso->recursos_asignados); i++)
    {
        t_asig_r *recurso = list_get(proceso->recursos_asignados, i);
        if (strcmp(recurso->nombre, nombre_recurso) == 0)
        {
             return i;
        }
    }

    return -1;
}

void destroy_t_asig_r(void* element) {
    if (element == NULL) {
        return;
    }
    
    t_asig_r* asig_r = (t_asig_r*)element;
    // Libera la memoria asignada a la estructura
    free(asig_r);
}

bool segmento_activo(t_ent_ts *seg) {
    return seg->activo && (seg->id_seg != 0);
}

void destroy_ent_ts(void* seg) {
    t_ent_ts* ent_seg = (t_ent_ts*) seg;
    free(seg);
}