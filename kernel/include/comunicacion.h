#ifndef COMUNICACION_H
#define COMUNICACION_H

#include "shared_utils.h"
#include "utils.h"

void procesar_conexion(void *void_args);
t_list *recv_instrucciones(t_log *logger, int cliente_socket);
t_pcb *crear_pcb(t_list *instrucciones, int socket_consola);

void *serializar_contexto_pcb(t_pcb *pcb, uint32_t tam_contexto); // Serializa los datos de contexto de ejecucion de un PCB y retorna un puntero a estos
void deserializar_contexto_pcb(void *buffer, t_pcb *pcb);         // Deserializa un stream y actualiza el contexto de un pcb
                                                                  // Ademas, pone en cop el codigo de operacion enviado por el
                                                                  // cpu (CPU_YIELD, CPU_EXIT...)

void mandar_a_cpu(t_pcb *pcb, uint32_t tam_contexto);              // Toma un PCB y el tamanio de su contexto, y lo envia al CPU para ser ejecutado, devuelve socket del cpu
void *recibir_nuevo_contexto(int socket_cpu, cod_op_kernel *cop); // Recibe el nuevo contexto del CPU y lo actualiza al PCB

void devolver_resultado(t_pcb* pcb, cod_op_kernel exit_code); // devuelve resultado de ejecucion a la consola de origen
t_list* solicitar_tabla_segmentos(t_log* logger, char* ip, char* puerto, uint32_t pid);
#endif
