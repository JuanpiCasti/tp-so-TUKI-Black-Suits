#include "utils.h"

void inicializar_registros() {
    AX = malloc(4);
    BX = malloc(4);
    CX = malloc(4);
    DX = malloc(4);
    EBX = malloc(8);
    ECX = malloc(8);
    EAX = malloc(8);
    EDX = malloc(8);
    RAX = malloc(16);
    RBX = malloc(16);
    RCX = malloc(16);
    RDX = malloc(16);
}

void levantar_loggers_cpu() {
    logger_cpu_extra = log_create("./log/cpu_extra.log", "CPU", true, LOG_LEVEL_INFO);
    logger_cpu = log_create("./log/cpu_extra.log", "CPU", true, LOG_LEVEL_INFO);
}

void levantar_config_cpu() {
    CONFIG_CPU = config_create("./cfg/cpu.config");
    RETARDO_INSTRUCCION = config_get_string_value(CONFIG_CPU, "RETARDO_INSTRUCCION");
    IP_MEMORIA = config_get_string_value(CONFIG_CPU, "IP_MEMORIA");
    PUERTO_MEMORIA = config_get_string_value(CONFIG_CPU, "PUERTO_MEMORIA");
    PUERTO_ESCUCHA_CPU = config_get_string_value(CONFIG_CPU, "PUERTO_ESCUCHA");
    TAM_MAX_SEGMENTO = config_get_string_value(CONFIG_CPU, "TAM_MAX_SEGMENTO");
}



void cambiar_contexto() {
    // TODO:

}

as_instruction decode(t_instruccion instruccion) {

    // strcmp con instruccion -> instruccion y devuelve que elemetno del enum as_instruction es.

}

void ejecutar_instrucciones() {
    while(true) {
        // TODO: ejecutar hasta que encuentre un YIELD o un EXIT (break)
        // Un ciclo de ejecucion implica hacer un decode de la instruccion,
        // ejecutar la funcion asociada a esa instruccion,
        // y aumentar el program counter en 1.
    }
}