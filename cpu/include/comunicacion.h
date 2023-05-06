#ifndef COMUNICACION_H
#define COMUNICACION_H

#include "shared_utils.h"

void procesar_conexion(void *void_args);

void devolver_contexto(int cliente_socket, cod_op_kernel cop); // Devuelve contexto actual a KERNEL
#endif