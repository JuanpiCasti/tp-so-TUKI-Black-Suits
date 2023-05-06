#ifndef COMUNICACION_H
#define COMUNICACION_H

#include "shared_utils.h"
#include "utils.h"


void procesar_conexion(void *void_args);
t_list *recv_instrucciones(t_log *logger, int cliente_socket);

void* serializar_contexto_pcb(t_pcb* pcb, uint32_t tam_contexto); // Serializa los datos de contexto de ejecucion de un PCB y retorna un puntero a estos
void deserializar_contexto_pcb(void* buffer,t_pcb* pcb); // Deserializa un stream y actualiza el contexto de un pcb
                                                                      // Ademas, pone en cop el codigo de operacion enviado por el
                                                                      // cpu (CPU_YIELD, CPU_EXIT...)

int mandar_a_cpu(t_pcb* pcb, uint32_t tam_contexto); // Toma un PCB y el tamanio de su contexto, y lo envia al CPU para ser ejecutado, devuelve socket del cpu
void* recibir_nuevo_contexto(int socket_cpu, cod_op_kernel *cop); // Recibe el nuevo contexto del CPU y lo actualiza al PCB

#endif