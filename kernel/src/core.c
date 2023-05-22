#include "core.h"

void encolar_proceso(t_pcb *new_pcb, t_list *cola, pthread_mutex_t *mutex_cola, char *estado_anterior, char *estado_actual)
{
    pthread_mutex_lock(mutex_cola); // Wait
    list_add(cola, new_pcb);
    loggear_cambio_estado(estado_anterior, estado_actual, new_pcb);
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
        // Saca un proceso de ready segun HRRN
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

            pthread_mutex_lock(&mutex_READY);
            list_add(READY, new_pcb);
            loggear_cambio_estado("NEW", "READY", new_pcb);
            pthread_mutex_unlock(&mutex_READY);


            loggear_cola_ready();

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

void planificacion_corto_plazo()
{
    while (true)
    {
        // Paso de READY a RUNNING - Corto Plazo
        pthread_mutex_lock(&mutex_READY);
        if (list_size(READY) > 0 && RUNNING == NULL)
        {
            t_pcb *r_pcb = siguiente_proceso_a_ejecutar();
            RUNNING = r_pcb;
            loggear_cambio_estado("READY", "RUNNING", r_pcb);
            pthread_mutex_unlock(&mutex_READY);

            uint32_t tam_contexto = sizeof(cod_op) +
                                    sizeof(uint32_t) + // PID
                                    4 * 4 +  // (AX, BX, CX, DX)
                                    4 * 8 +  // (EAX, EBX, ECX, EDX)
                                    4 * 16 + // (RAX, RBX, RCX, RDX)
                                    sizeof(uint32_t) +
                                    sizeof(uint32_t) +
                                    list_size(r_pcb->instrucciones) * sizeof(t_instruccion);

            int socket_cpu = mandar_a_cpu(RUNNING, tam_contexto);
            cod_op_kernel cop;
            void *buffer = recibir_nuevo_contexto(socket_cpu, &cop);
            deserializar_contexto_pcb(buffer, r_pcb);

            switch (cop)
            {
            case CPU_YIELD:
                encolar_proceso(r_pcb, READY, &mutex_READY, "RUNNING", "READY");
                RUNNING = NULL;
                break;
            case CPU_EXIT:
                encolar_proceso(r_pcb, EXIT, &mutex_EXIT, "RUNNING", "EXIT");

                pthread_mutex_lock(&mutex_mp);
                GRADO_ACTUAL_MPROG--;
                pthread_mutex_unlock(&mutex_mp);
                
                loggear_fin_proceso(r_pcb, CPU_EXIT);
                devolver_resultado(RUNNING, CPU_EXIT);
                RUNNING = NULL;
                break;
            default:
                break;
            }
        } else {
            pthread_mutex_unlock(&mutex_READY);
        }
    }
}
