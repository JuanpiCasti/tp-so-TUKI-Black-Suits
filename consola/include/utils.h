#ifndef UTILS_CONSOLA_H
#define UTILS_CONSOLA_H

#include <stdio.h>
#include <consola.h>
#include <commons/collections/list.h>

int enviar_instrucciones(t_log *logger, char *ip, char *puerto, char *archivo_instrucciones);
cod_op_kernel recibir_resultado(int socket_kernel);

#endif