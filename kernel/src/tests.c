#include "tests.h"


void envio_contexto_cpu() {
    // Crear un nuevo t_pcb
    t_pcb* nuevo_pcb = malloc(sizeof(t_pcb));

    // Inicializar los campos del t_pcb
    nuevo_pcb->pid = 1234;
    nuevo_pcb->instrucciones = list_create();
    nuevo_pcb->program_counter = 0;

    t_registros_cpu* registros_cpu = malloc(sizeof(t_registros_cpu));

    // Asignar memoria para los registros de 4 bytes
    registros_cpu->AX = malloc(4);
    registros_cpu->BX = malloc(4);
    registros_cpu->CX = malloc(4);
    registros_cpu->DX = malloc(4);

    // Asignar memoria para los registros de 8 bytes
    registros_cpu->EAX = malloc(8);
    registros_cpu->EBX = malloc(8);
    registros_cpu->ECX = malloc(8);
    registros_cpu->EDX = malloc(8);

    // Asignar memoria para los registros de 16 bytes
    registros_cpu->RAX = malloc(16);
    registros_cpu->RBX = malloc(16);
    registros_cpu->RCX = malloc(16);
    registros_cpu->RDX = malloc(16);

    nuevo_pcb->registros_cpu = registros_cpu;

    // Crear algunas instrucciones de prueba
    t_instruccion instruccion1 = {"SET", "AX", "HOLA", ""};
    t_instruccion instruccion2 = {"MOV", "CX", "10", ""};
    t_instruccion instruccion3 = {"ADD", "BX", "1", ""};

    // Agregar las instrucciones a la lista de instrucciones del t_pcb
    list_add(nuevo_pcb->instrucciones, &instruccion1);
    list_add(nuevo_pcb->instrucciones, &instruccion2);
    list_add(nuevo_pcb->instrucciones, &instruccion3);

    nuevo_pcb->tabla_segmentos = list_create();
    nuevo_pcb->estimado_HRRN = 0.0;
    nuevo_pcb->tiempo_ready = time(NULL);
    nuevo_pcb->archivos_abiertos = list_create();

    uint32_t tam_contexto = sizeof(cod_op) + 
                          4*4 + // (AX, BX, CX, DX) 
                          4*8 + // (EAX, EBX, ECX, EDX)
                          4*16 + // (RAX, RBX, RCX, RDX)
                          sizeof(uint32_t) +
                          sizeof(uint32_t) +
                          list_size(nuevo_pcb->instrucciones) * sizeof(t_instruccion); 
    
    void* buffer = serializar_contexto_pcb(nuevo_pcb, tam_contexto);
    cod_op cop;


    imprimir_pcb(nuevo_pcb);
    mandar_a_cpu(nuevo_pcb, tam_contexto);
    free(buffer);
}

int run_tests()
{
    envio_contexto_cpu();

}
