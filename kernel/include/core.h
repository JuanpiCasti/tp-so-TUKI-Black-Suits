#ifndef CORE_H
#define CORE_H
#include "utils.h"
#include "comunicacion.h"

void encolar_proceso(t_pcb *new_pcb, t_list *cola, pthread_mutex_t *mutex_cola, char *estado_anterior, char *estado_actual);
void planificacion_largo_plazo(); // Proceso del planificador a largo plazo
void planificacion_corto_plazo(); // Proceso del planificador a corto plazo

#endif