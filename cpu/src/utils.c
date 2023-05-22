#include "utils.h"

void inicializar_registros()
{
    memset(AX, 0, sizeof(AX));
    AX[0] = '\0';
    memset(BX, 0, sizeof(BX));
    BX[0] = '\0';
    memset(CX, 0, sizeof(CX));
    CX[0] = '\0';
    memset(DX, 0, sizeof(DX));
    DX[0] = '\0';
    memset(EAX, 0, sizeof(EAX));
    EAX[0] = '\0';
    memset(EBX, 0, sizeof(EBX));
    EBX[0] = '\0';
    memset(ECX, 0, sizeof(ECX));
    ECX[0] = '\0';
    memset(EDX, 0, sizeof(EDX));
    EDX[0] = '\0';
    memset(RAX, 0, sizeof(RAX));
    RAX[0] = '\0';
    memset(RBX, 0, sizeof(RBX));
    RBX[0] = '\0';
    memset(RCX, 0, sizeof(RCX));
    RCX[0] = '\0';
    memset(RDX, 0, sizeof(RDX));
    RDX[0] = '\0';
    INSTRUCTION_LIST = list_create();
}

void levantar_loggers_cpu()
{
    logger_cpu_extra = log_create("./log/cpu_extra.log", "CPU", false, LOG_LEVEL_INFO);
    logger_cpu = log_create("./log/cpu_extra.log", "CPU", true, LOG_LEVEL_INFO);
}

void levantar_config_cpu()
{
    CONFIG_CPU = config_create("./cfg/cpu.config");
    RETARDO_INSTRUCCION = config_get_int_value(CONFIG_CPU, "RETARDO_INSTRUCCION");
    IP_MEMORIA = config_get_string_value(CONFIG_CPU, "IP_MEMORIA");
    PUERTO_MEMORIA = config_get_string_value(CONFIG_CPU, "PUERTO_MEMORIA");
    PUERTO_ESCUCHA_CPU = config_get_string_value(CONFIG_CPU, "PUERTO_ESCUCHA");
    TAM_MAX_SEGMENTO = config_get_string_value(CONFIG_CPU, "TAM_MAX_SEGMENTO");
}

void cambiar_contexto(void *buffer)
{
    uint32_t desplazamiento = 0;
    // PID
    memcpy(&PID_RUNNING, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    
    // Registros CPU
    memcpy(AX, buffer + desplazamiento, 4);
    desplazamiento += 4;
    memcpy(BX, buffer + desplazamiento, 4);
    desplazamiento += 4;
    memcpy(CX, buffer + desplazamiento, 4);
    desplazamiento += 4;
    memcpy(DX, buffer + desplazamiento, 4);
    desplazamiento += 4;
    memcpy(EAX, buffer + desplazamiento, 8);
    desplazamiento += 8;
    memcpy(EBX, buffer + desplazamiento, 8);
    desplazamiento += 8;
    memcpy(ECX, buffer + desplazamiento, 8);
    desplazamiento += 8;
    memcpy(EDX, buffer + desplazamiento, 8);
    desplazamiento += 8;
    memcpy(RAX, buffer + desplazamiento, 16);
    desplazamiento += 16;
    memcpy(RBX, buffer + desplazamiento, 16);
    desplazamiento += 16;
    memcpy(RCX, buffer + desplazamiento, 16);
    desplazamiento += 16;
    memcpy(RDX, buffer + desplazamiento, 16);
    desplazamiento += 16;

    // Program counter
    memcpy(&PROGRAM_COUNTER, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    // Instrucciones
    uint32_t tam_instrucciones = 0;
    memcpy(&tam_instrucciones, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    INSTRUCTION_LIST = deserializar_instrucciones(buffer + desplazamiento, tam_instrucciones);
}

void imprimir_contexto_actual()
{
    printf("PC: %d\n", PROGRAM_COUNTER);
    printf("AX: %s\n", imprimir_cadena(AX, 4));
    printf("BX: %s\n", imprimir_cadena(BX, 4));
    printf("CX: %s\n", imprimir_cadena(CX, 4));
    printf("DX: %s\n", imprimir_cadena(DX, 4));
    printf("EAX: %s\n", imprimir_cadena(EAX, 8));
    printf("EBX: %s\n", imprimir_cadena(EBX, 8));
    printf("ECX: %s\n", imprimir_cadena(ECX, 8));
    printf("EDX: %s\n", imprimir_cadena(EDX, 8));
    printf("RAX: %s\n", imprimir_cadena(RAX, 16));
    printf("RBX: %s\n", imprimir_cadena(RBX, 16));
    printf("RCX: %s\n", imprimir_cadena(RCX, 16));
    printf("RDX: %s\n", imprimir_cadena(RDX, 16));

    printf("Instrucciones:\n");
    for (int i = 0; i < list_size(INSTRUCTION_LIST); i++)
    {
        t_instruccion *instruccion = list_get(INSTRUCTION_LIST, i);
        printf("\t%d: %s %s %s %s\n", i, instruccion->instruccion, instruccion->arg1, instruccion->arg2, instruccion->arg3);
    }
}

void loggear_ejecucion(t_instruccion* instruccion) {
    log_info(logger_cpu, "PID: %d - Ejecutando: %s - arg1: %s, arg2: %s, arg3: %s", PID_RUNNING,
     instruccion->instruccion,
     instruccion->arg1,
     instruccion->arg2,
     instruccion->arg3);
}

as_instruction decode(t_instruccion *instruccion)
{
    if (strcmp(instruccion->instruccion, "SET") == 0)
    {
        return SET;
    }
    else if (strcmp(instruccion->instruccion, "YIELD") == 0)
    {
        return YIELD;
    }
    else if (strcmp(instruccion->instruccion, "EXIT") == 0)
    {
        return EXIT;
    }
    else
    {
        log_error(logger_cpu, "La intrucción no es válida.");
        return EXIT;
    }
}

void ejecutar_set(t_instruccion *instruccion)
{
    if (strcmp(instruccion->arg1, "AX") == 0)
    {
        strncpy(AX, instruccion->arg2, 4);
    }
    else if (strcmp(instruccion->arg1, "BX") == 0)
    {
        strncpy(BX, instruccion->arg2, 4);
    }
    else if (strcmp(instruccion->arg1, "CX") == 0)
    {
        strncpy(CX, instruccion->arg2, 4);
    }
    else if (strcmp(instruccion->arg1, "DX") == 0)
    {
        strncpy(DX, instruccion->arg2, 4);
    }
    else if (strcmp(instruccion->arg1, "EAX") == 0)
    {
        strncpy(EAX, instruccion->arg2, 8);
    }
    else if (strcmp(instruccion->arg1, "EBX") == 0)
    {
        strncpy(EBX, instruccion->arg2, 8);
    }
    else if (strcmp(instruccion->arg1, "ECX") == 0)
    {
        strncpy(ECX, instruccion->arg2, 8);
    }
    else if (strcmp(instruccion->arg1, "EDX") == 0)
    {
        strncpy(EDX, instruccion->arg2, 8);
    }
    else if (strcmp(instruccion->arg1, "RAX") == 0)
    {
        strncpy(RAX, instruccion->arg2, 16);
    }
    else if (strcmp(instruccion->arg1, "RBX") == 0)
    {
        strncpy(RBX, instruccion->arg2, 16);
    }
    else if (strcmp(instruccion->arg1, "RCX") == 0)
    {
        strncpy(RCX, instruccion->arg2, 16);
    }
    else if (strcmp(instruccion->arg1, "RDX") == 0)
    {
        strncpy(RDX, instruccion->arg2, 16);
    }
    else
    {
        log_error(logger_cpu, "El argumento 1 de la instrucción SET no corresponde a ningún registro.");
    }
}

cod_op_kernel ejecutar_instrucciones()
{
    t_instruccion *instruccion = list_get(INSTRUCTION_LIST, PROGRAM_COUNTER);
    PROGRAM_COUNTER += 1;
    as_instruction instruction_code = decode(instruccion);

    while (true)
    {
        loggear_ejecucion(instruccion);

        switch (instruction_code)
        {
        case SET:
            sleep(RETARDO_INSTRUCCION / 1000);
            ejecutar_set(instruccion);
            break;
        case YIELD:
            return CPU_YIELD;
        case EXIT:
            return CPU_EXIT;

        default:
            break;
        }

        //imprimir_contexto_actual();


        if (PROGRAM_COUNTER >= list_size(INSTRUCTION_LIST))
        {
            return CPU_EXIT; // o hacer otra cosa para manejar este error
        }

        instruccion = list_get(INSTRUCTION_LIST, PROGRAM_COUNTER);
        PROGRAM_COUNTER += 1;
        instruction_code = decode(instruccion);
    }
}
