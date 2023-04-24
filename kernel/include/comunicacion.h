#ifndef COMUNICACION_H
#define COMUNICACION_H

#include "shared_utils.h"
#include "utils.h"

void procesar_conexion(void *void_args);
t_list* recv_instrucciones(t_log* logger, int cliente_socket);

#endif