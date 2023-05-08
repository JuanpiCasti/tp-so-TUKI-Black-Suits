#include "tests.h"


void envio_contexto_cpu() {
    // Crear un nuevo t_pcb
    t_pcb* nuevo_pcb = malloc(sizeof(t_pcb));

    // Inicializar los campos del t_pcb
    nuevo_pcb->pid = 1234;
    nuevo_pcb->instrucciones = list_create();
    nuevo_pcb->program_counter = 0;

    t_registros_cpu *registros_cpu = malloc(sizeof(t_registros_cpu));
    memset(registros_cpu->AX, 0, sizeof(registros_cpu->AX));
    memset(registros_cpu->BX, 0, sizeof(registros_cpu->BX));
    memset(registros_cpu->CX, 0, sizeof(registros_cpu->CX));
    memset(registros_cpu->DX, 0, sizeof(registros_cpu->DX));
    memset(registros_cpu->EAX, 0, sizeof(registros_cpu->EAX));
    memset(registros_cpu->EBX, 0, sizeof(registros_cpu->EBX));
    memset(registros_cpu->ECX, 0, sizeof(registros_cpu->ECX));
    memset(registros_cpu->EDX, 0, sizeof(registros_cpu->EDX));
    memset(registros_cpu->RAX, 0, sizeof(registros_cpu->RAX));
    memset(registros_cpu->RBX, 0, sizeof(registros_cpu->RBX));
    memset(registros_cpu->RCX, 0, sizeof(registros_cpu->RCX));
    memset(registros_cpu->RDX, 0, sizeof(registros_cpu->RDX));


    nuevo_pcb->registros_cpu = registros_cpu;

    // Crear algunas instrucciones de prueba
    t_instruccion instruccion1 = {"SET", "AX", "CHAU", ""};

    // Agregar las instrucciones a la lista de instrucciones del t_pcb
    list_add(nuevo_pcb->instrucciones, &instruccion1);

    nuevo_pcb->tabla_segmentos = list_create();
    nuevo_pcb->estimado_HRRN = 0.0;
    nuevo_pcb->tiempo_ready = time(NULL);
    nuevo_pcb->archivos_abiertos = list_create();

    uint32_t tam_contexto = 4*4 + // (AX, BX, CX, DX) 
                          4*8 + // (EAX, EBX, ECX, EDX)
                          4*16 + // (RAX, RBX, RCX, RDX)
                          sizeof(uint32_t) +
                          sizeof(uint32_t) +
                          list_size(nuevo_pcb->instrucciones) * sizeof(t_instruccion); 
    
    char* prueba = "HOLACOMOESTASBRO";
    char* prueba2 = "HOLA";
    strcpy(nuevo_pcb->registros_cpu->AX, prueba2);
    strcpy(nuevo_pcb->registros_cpu->RDX, prueba);
    strcpy(nuevo_pcb->registros_cpu->RAX, prueba);
    

    cod_op_kernel cop;
    
    int socket_cpu = mandar_a_cpu(nuevo_pcb, tam_contexto);
    
    void* buffer = recibir_nuevo_contexto(socket_cpu, &cop);
    deserializar_contexto_pcb(buffer, nuevo_pcb);
    imprimir_pcb(nuevo_pcb);
    printf("COP: %d\n", cop);

}

int run_tests()
{
    envio_contexto_cpu();

}
