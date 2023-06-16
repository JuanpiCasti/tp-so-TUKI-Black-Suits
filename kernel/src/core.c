#include "core.h"

void encolar_proceso(t_pcb *new_pcb, t_list *cola, pthread_mutex_t *mutex_cola, char *estado_anterior, char *estado_actual)
{
    pthread_mutex_lock(mutex_cola); // Wait
    list_add(cola, new_pcb);
    loggear_cambio_estado(estado_anterior, estado_actual, new_pcb);
    if(strcmp("READY", estado_actual) == 0)
    { 
        new_pcb->llegada_ready = time(NULL);
        loggear_cola_ready(cola);
    }
    pthread_mutex_unlock(mutex_cola); // Signal

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
        t_pcb *proceso;

        float RR_aux = 0;
        float RR_mayor = 0;
        int index_de_RR_mayor = 0;

        for (int i = 0; i < list_size(READY); i++)
        {
            proceso = list_get(READY, i);
            if (proceso->ultima_rafaga != 0)
            {
                proceso->estimado_HRRN = HRRN_ALFA * proceso->estimado_HRRN + (1 - HRRN_ALFA) * proceso->ultima_rafaga; // S
            }

            RR_aux = ((time(NULL) - proceso->llegada_ready) + proceso->estimado_HRRN) / proceso->estimado_HRRN; // (W + S) / S
            //printf("RR %d: %f\n", proceso->pid, RR_aux);

            if (RR_aux > RR_mayor)
            {
                RR_mayor = RR_aux;
                index_de_RR_mayor = i;
            }
        }

        return (t_pcb *)(list_remove(READY, index_de_RR_mayor));
    }

    log_error(logger_kernel_extra, "El algoritmo indicado en el archivo de configuracion es desconocido");
    exit(-1);
}

void planificacion_largo_plazo()
{
    while (true)
    {
        pthread_mutex_lock(&mutex_NEW);

        // PASO de NEW a READY - Largo Plazo
        pthread_mutex_lock(&mutex_mp);
        if (list_size(NEW) > 0 && GRADO_ACTUAL_MPROG < GRADO_MAX_MULTIPROGRAMACION)
        {
            pthread_mutex_unlock(&mutex_mp);
            t_pcb *new_pcb = list_remove(NEW, 0);
            pthread_mutex_unlock(&mutex_NEW);

            encolar_proceso(new_pcb, READY, &mutex_READY, "NEW", "READY");

            pthread_mutex_lock(&mutex_mp);
            GRADO_ACTUAL_MPROG++;
            pthread_mutex_unlock(&mutex_mp);
        }
        else
        {
            pthread_mutex_unlock(&mutex_NEW);
            pthread_mutex_unlock(&mutex_mp);
        }
    }
}

void *bloquear_proceso_io(void *wait_time_arg)
{
    int wait_time = *(int *)wait_time_arg; // Castea el puntero a void a puntero a entero y lo derreferencia.

    t_pcb *proceso = RUNNING;
    RUNNING = NULL;
    loggear_cambio_estado("RUNNING", "BLOQUEADO", proceso);
    pthread_mutex_unlock(&mutex_RUNNING);
    log_info(logger_kernel, "PID: %d - Ejecuta IO: %d", proceso->pid, wait_time);
    log_info(logger_kernel, "PID: %d - Bloqueado por: IO", proceso->pid);
    sleep(wait_time);
    encolar_proceso(proceso, READY, &mutex_READY, "BLOQUEADO", "READY");
    pthread_exit(NULL);
}

void desalojar_a_ready()
{
    pthread_mutex_lock(&mutex_RUNNING); // Wait
    encolar_proceso(RUNNING, READY, &mutex_READY, "RUNNING", "READY");
    RUNNING = NULL;
    pthread_mutex_unlock(&mutex_RUNNING);
}

void desalojar()
{
    pthread_mutex_lock(&mutex_RUNNING);
    RUNNING = NULL;
    pthread_mutex_unlock(&mutex_RUNNING);
}

void terminar_proceso(t_pcb *proceso, cod_op_kernel motivo)
{
    encolar_proceso(proceso, EXIT, &mutex_EXIT, "RUNNING", "EXIT");

    pthread_mutex_lock(&mutex_mp);
    GRADO_ACTUAL_MPROG--;
    pthread_mutex_unlock(&mutex_mp);

    loggear_fin_proceso(proceso, motivo);
    devolver_resultado(proceso, motivo);

    desalojar();
}

void wait_recurso(t_pcb *proceso, char *nombre_recurso)
{
    t_recurso *recurso = buscar_recurso_por_nombre(nombre_recurso);

    if (recurso == NULL)
    {
        terminar_proceso(proceso, EXIT_RESOURCE_NOT_FOUND);
        return;
    }

    recurso->instancias_disponibles--;
    log_info(logger_kernel, "PID: %d - Wait: %s - Instancias: %d", proceso->pid, nombre_recurso, recurso->instancias_disponibles);

    if (recurso->instancias_disponibles < 0)
    {
        list_add(recurso->cola_bloqueados, proceso);
        log_info(logger_kernel, "PID: %d - Bloqueado por: %s", proceso->pid, nombre_recurso);
        desalojar();
    }

    // TODO: log wait
}

void signal_recurso(t_pcb *proceso, char *nombre_recurso)
{
    t_recurso *recurso = buscar_recurso_por_nombre(nombre_recurso);

    if (recurso == NULL)
    {
        terminar_proceso(proceso, EXIT_RESOURCE_NOT_FOUND);
        return;
    }

    recurso->instancias_disponibles++;
    log_info(logger_kernel, "PID: %d - Signal: %s - Instancias: %d", proceso->pid, nombre_recurso, recurso->instancias_disponibles);

    if (list_size(recurso->cola_bloqueados) > 0)
    {
        t_pcb *proceso_bloqueado = list_remove(recurso->cola_bloqueados, 0);
        encolar_proceso(proceso_bloqueado, READY, &mutex_READY, "BLOQUEADO", "READY");
    }
}

void solicitar_creacion_segmento(uint32_t id_seg, uint32_t tam, t_pcb* pcb) {
    int socket_memoria = crear_conexion(logger_kernel_extra, IP_MEMORIA, PUERTO_MEMORIA);

    int tam_buffer = sizeof(uint32_t) * 3 + sizeof(cod_op);

    void* buffer = malloc(tam_buffer);
    int despl = 0;
    cod_op cop = MEMORIA_CREATE_SEGMENT;

    memcpy(buffer + despl, &cop, sizeof(cod_op));
    despl += sizeof(cod_op);
    memcpy(buffer + despl, &pcb->pid, sizeof(uint32_t));
    despl+= sizeof(uint32_t);
    memcpy(buffer + despl, &id_seg, sizeof(uint32_t));
    despl += sizeof(uint32_t);
    memcpy(buffer + despl, &tam, sizeof(uint32_t));
    despl += sizeof(uint32_t);

    send(socket_memoria, buffer, tam_buffer, NULL);
    free(buffer);

    cod_op_kernel resultado;
    recv(socket_memoria, &resultado, sizeof(cod_op_kernel), NULL);
    switch (resultado)
    {
    case EXIT_OUT_OF_MEMORY:
        terminar_proceso(pcb, EXIT_OUT_OF_MEMORY);
        break;
    case MEMORIA_NECESITA_COMPACTACION:
        // TODO: COMPACTACION
        log_info(logger_kernel, "La memoria necesita ser compactada!!");
        break;
    case MEMORIA_SEGMENTO_CREADO:
        uint32_t base;
        recv(socket_memoria, &base, sizeof(uint32_t), NULL);
        t_ent_ts* segmento = list_get(pcb -> tabla_segmentos, id_seg);
        segmento -> base = base;
        segmento -> tam = tam;
        segmento -> activo = 1;
        log_info(logger_kernel, "PID: %d - Crear Segmento - Id: %d - Tamaño: %d", pcb->pid, id_seg, tam);
        //imprimir_pcb(pcb);
        break;
    
    default:
        break;

    close(socket_memoria);
    }
}

void solicitar_liberacion_segmento(uint32_t base, uint32_t tam, uint32_t pid, uint32_t seg_id) {
    int socket_memoria = crear_conexion(logger_kernel_extra, IP_MEMORIA, PUERTO_MEMORIA);
    int tam_buffer = sizeof(uint32_t) * 4 + sizeof(cod_op);

    void* buffer = malloc(tam_buffer);
    int despl = 0;

    cod_op cop = MEMORIA_FREE_SEGMENT;
    memcpy(buffer + despl, &cop, sizeof(cod_op));
    despl += sizeof(cod_op);
    memcpy(buffer + despl, &pid, sizeof(cod_op));
    despl += sizeof(uint32_t);
    memcpy(buffer + despl, &seg_id, sizeof(cod_op));
    despl += sizeof(uint32_t);
    memcpy(buffer + despl, &base, sizeof(uint32_t));
    despl += sizeof(uint32_t);
    memcpy(buffer + despl, &tam, sizeof(uint32_t));
    despl += sizeof(uint32_t);

    send(socket_memoria, buffer, tam_buffer, NULL);
    free(buffer);
    close(socket_memoria);
    log_info(logger_kernel, "PID: %d - Eliminar Segmento - Id: %d - Tamaño: %d", pid, seg_id, tam);
}

void planificacion_corto_plazo()
{
    t_pcb *r_pcb;
    time_t llegada_running;

    while (true)
    {
        // Paso de READY a RUNNING - Corto Plazo
        pthread_mutex_lock(&mutex_READY);
        pthread_mutex_lock(&mutex_RUNNING);

        if (list_size(READY) > 0 && RUNNING == NULL)
        {
            r_pcb = siguiente_proceso_a_ejecutar();
            llegada_running = time(NULL);
            RUNNING = r_pcb;
            pthread_mutex_unlock(&mutex_RUNNING);
            loggear_cambio_estado("READY", "RUNNING", r_pcb);
            pthread_mutex_unlock(&mutex_READY);
        }
        else
        {
            pthread_mutex_unlock(&mutex_READY);
            pthread_mutex_unlock(&mutex_RUNNING);
        }

        pthread_mutex_lock(&mutex_READY);
        pthread_mutex_lock(&mutex_RUNNING);

        if (RUNNING != NULL)
        {
            pthread_mutex_unlock(&mutex_READY);
            r_pcb = RUNNING;
            pthread_mutex_unlock(&mutex_RUNNING);

            uint32_t tam_contexto = sizeof(cod_op) +
                                    sizeof(uint32_t) + // PID
                                    4 * 4 +            // (AX, BX, CX, DX)
                                    4 * 8 +            // (EAX, EBX, ECX, EDX)
                                    4 * 16 +           // (RAX, RBX, RCX, RDX)
                                    sizeof(uint32_t) +
                                    sizeof(uint32_t) +
                                    list_size(r_pcb->instrucciones) * sizeof(t_instruccion) +
                                    sizeof(uint32_t) +
                                    list_size(r_pcb->tabla_segmentos) * sizeof(t_ent_ts) + 500;

            int socket_cpu = mandar_a_cpu(r_pcb, tam_contexto);
            cod_op_kernel cop;
            void *buffer = recibir_nuevo_contexto(socket_cpu, &cop);
            deserializar_contexto_pcb(buffer, r_pcb);

            char nombre_recurso[20];
            uint32_t id_segmento;
            uint32_t tam_segmento;
            switch (cop)
            {
            case CPU_YIELD:
                free(buffer);
                RUNNING->ultima_rafaga = time(NULL) - llegada_running;
                desalojar_a_ready();
                break;
            case CPU_EXIT:
                free(buffer);
                terminar_proceso(r_pcb, CPU_EXIT);
                break;
            case CPU_IO:
                uint32_t wait_time;
                memcpy(&wait_time, buffer + TAMANIO_CONTEXTO, sizeof(uint32_t));
                free(buffer);
                pthread_t hilo_io;
                r_pcb->ultima_rafaga = time(NULL) - llegada_running;
                pthread_mutex_lock(&mutex_RUNNING);
                pthread_create(&hilo_io, NULL, bloquear_proceso_io, (void *)(&wait_time));
                pthread_detach(hilo_io);
                break;
            case CPU_WAIT:
                memcpy(&nombre_recurso, buffer + TAMANIO_CONTEXTO, 20);
                free(buffer);
                wait_recurso(r_pcb, nombre_recurso);
                break;
            case CPU_SIGNAL:
                memcpy(&nombre_recurso, buffer + TAMANIO_CONTEXTO, 20);
                free(buffer);
                signal_recurso(r_pcb, nombre_recurso);
                break;
            case CPU_CREATE_SEGMENT:
                memcpy(&id_segmento, buffer + TAMANIO_CONTEXTO, sizeof(uint32_t));
                memcpy(&tam_segmento, buffer + TAMANIO_CONTEXTO + sizeof(uint32_t), sizeof(uint32_t));
                free(buffer);
                //printf("ID: %d, TAM: %d\n", id_segmento, tam_segmento);
                solicitar_creacion_segmento(id_segmento, tam_segmento, r_pcb);
                //imprimir_pcb(r_pcb);
                break;
            case CPU_DELETE_SEGMENT:
                memcpy(&id_segmento, buffer + TAMANIO_CONTEXTO, sizeof(uint32_t));
                free(buffer);
                t_ent_ts* seg = list_get(r_pcb->tabla_segmentos, id_segmento);
                solicitar_liberacion_segmento(seg->base, seg->tam, r_pcb->pid, id_segmento);
                seg->base = 0;
                seg->tam = 0;
                seg->activo = 0;
                //imprimir_pcb(r_pcb);
                break;
            case CPU_SEG_FAULT:
                free(buffer);
                terminar_proceso(r_pcb, CPU_SEG_FAULT);
            default:
                break;
            }
        }
        else
        {
            pthread_mutex_unlock(&mutex_READY);
            pthread_mutex_unlock(&mutex_RUNNING);
        }
    }
}
