#include "utils.h"

t_list *deserializar_instrucciones(void *stream, uint32_t tam_instrucciones)
{
    int cant_instrucciones = tam_instrucciones / sizeof(t_instruccion);

    t_list *lista_instrucciones = list_create();

    int desplazamiento = 0;
    for (int i = 0; i < cant_instrucciones; i++)
    {
        t_instruccion *instruccion = malloc(sizeof(t_instruccion));
        memcpy(&instruccion->instruccion, stream + desplazamiento, sizeof(char[20]));
        memcpy(&instruccion->arg1, stream + desplazamiento + sizeof(char[20]), sizeof(char[20]));
        memcpy(&instruccion->arg2, stream + desplazamiento + sizeof(char[20]) * 2, sizeof(char[20]));
        memcpy(&instruccion->arg3, stream + desplazamiento + sizeof(char[20]) * 3, sizeof(char[20]));
        list_add(lista_instrucciones, instruccion);
        desplazamiento += sizeof(t_instruccion);
    }

    return lista_instrucciones;
}

t_pcb *crear_pcb(t_list *instrucciones, double estimacion_inicial)
{
    t_pcb *pcb = malloc(sizeof(t_pcb));

    pcb->pid = next_pid;
    next_pid++;

    pcb->instrucciones = instrucciones;
    pcb->program_counter = 0;

    pcb->registros_cpu = malloc(4 * 4 + 8 * 4 + 16 * 4);
    pcb->registros_cpu->AX = malloc(4);
    pcb->registros_cpu->BX = malloc(4);
    pcb->registros_cpu->CX = malloc(4);
    pcb->registros_cpu->DX = malloc(4);
    pcb->registros_cpu->EAX = malloc(8);
    pcb->registros_cpu->EBX = malloc(8);
    pcb->registros_cpu->ECX = malloc(8);
    pcb->registros_cpu->EDX = malloc(8);
    pcb->registros_cpu->RAX = malloc(16);
    pcb->registros_cpu->RBX = malloc(16);
    pcb->registros_cpu->RCX = malloc(16);
    pcb->registros_cpu->RDX = malloc(16);

    pcb->estimado_HRRN = estimacion_inicial;
    pcb->tiempo_ready = time(NULL);
    pcb->archivos_abiertos = list_create();

    return pcb;
}

void imprimir_pcb(t_pcb *pcb)
{
    printf("PID: %d\n", pcb->pid);
    printf("Instrucciones:\n");
    for (int i = 0; i < pcb->instrucciones->elements_count; i++)
    {
        char *instruccion = list_get(pcb->instrucciones, i);
        printf("\t%d: %s\n", i, instruccion);
    }
    printf("Program Counter: %d\n", pcb->program_counter);
    printf("Registros CPU:\n");
    printf("\tAX: %p\n", pcb->registros_cpu->AX);
    printf("\tBX: %p\n", pcb->registros_cpu->BX);
    printf("\tCX: %p\n", pcb->registros_cpu->CX);
    printf("\tDX: %p\n", pcb->registros_cpu->DX);
    printf("\tEAX: %p\n", pcb->registros_cpu->EAX);
    printf("\tEBX: %p\n", pcb->registros_cpu->EBX);
    printf("\tECX: %p\n", pcb->registros_cpu->ECX);
    printf("\tEDX: %p\n", pcb->registros_cpu->EDX);
    printf("\tRAX: %p\n", pcb->registros_cpu->RAX);
    printf("\tRBX: %p\n", pcb->registros_cpu->RBX);
    printf("\tRCX: %p\n", pcb->registros_cpu->RCX);
    printf("\tRDX: %p\n", pcb->registros_cpu->RDX);
    printf("Tabla de Segmentos:\n");
    printf("Estimado HRRN: %f\n", pcb->estimado_HRRN);
    printf("Tiempo ready: %ld\n", (long)pcb->tiempo_ready);
    printf("Archivos abiertos:\n");
}
