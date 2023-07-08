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
    logger_cpu_extra = log_create("./log/cpu_extra.log", "CPU", true, LOG_LEVEL_INFO);
    logger_cpu = log_create("./log/cpu.log", "CPU", true, LOG_LEVEL_INFO);
}

void levantar_config_cpu()
{
    CONFIG_CPU = config_create("./cfg/cpu.config");
    RETARDO_INSTRUCCION = config_get_int_value(CONFIG_CPU, "RETARDO_INSTRUCCION");
    IP_MEMORIA = config_get_string_value(CONFIG_CPU, "IP_MEMORIA");
    PUERTO_MEMORIA = config_get_string_value(CONFIG_CPU, "PUERTO_MEMORIA");
    PUERTO_ESCUCHA_CPU = config_get_string_value(CONFIG_CPU, "PUERTO_ESCUCHA");
    TAM_MAX_SEGMENTO = config_get_int_value(CONFIG_CPU, "TAM_MAX_SEGMENTO");
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

    list_destroy_and_destroy_elements(INSTRUCTION_LIST, destroy_instruccion);

    INSTRUCTION_LIST = deserializar_instrucciones(buffer + desplazamiento, tam_instrucciones);
    desplazamiento += tam_instrucciones;

    // Segmentos
    uint32_t tam_segmentos;
    memcpy(&tam_segmentos, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    SEGMENT_LIST = deserializar_segmentos(buffer + desplazamiento, tam_segmentos);

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

    printf("Segmentos:\n");
    for (int i = 0; i < list_size(SEGMENT_LIST); i++)
    {
        t_ent_ts* seg = list_get(SEGMENT_LIST, i);
        printf("%d, %d, %d, %d\n", seg->id_seg , seg->base, seg->tam, seg->activo); 
    }
    
}

void loggear_ejecucion(t_instruccion* instruccion) {
    log_info(logger_cpu, "PID: %d - Ejecutando: %s - arg1: %s, arg2: %s, arg3: %s", PID_RUNNING,
     instruccion->instruccion,
     instruccion->arg1,
     instruccion->arg2,
     instruccion->arg3);
}

int32_t mmu(uint32_t dir_logica, uint32_t tamanio_operacion) {
    uint32_t id_seg = dir_logica/  TAM_MAX_SEGMENTO ;
    uint32_t desplazamiento = dir_logica % TAM_MAX_SEGMENTO;

    t_ent_ts* seg = list_get(SEGMENT_LIST, id_seg); 

    if (desplazamiento + tamanio_operacion > (seg->tam))
    {
        log_error(logger_cpu, "PID: %d - ERROR SEG_FAULT - Segmento: %d - Offset: %d - Tamaño: %d", PID_RUNNING, id_seg, desplazamiento, seg->tam);
        return -1;
    }

    return seg->base + desplazamiento;
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
    else if (strcmp(instruccion->instruccion, "I/O") == 0) {
        return IO;
    }
    else if (strcmp(instruccion->instruccion, "WAIT") == 0) {
        return WAIT;
    }
    else if (strcmp(instruccion->instruccion, "SIGNAL") == 0) {
        return SIGNAL;
    }
    else if (strcmp(instruccion->instruccion, "CREATE_SEGMENT") == 0) {
        return CREATE_SEGMENT;
    }
    else if (strcmp(instruccion->instruccion, "DELETE_SEGMENT") == 0) {
        return DELETE_SEGMENT;
    }
    else if (strcmp(instruccion->instruccion, "MOV_IN") == 0) {
        return MOV_IN;
    }
    else if (strcmp(instruccion->instruccion, "MOV_OUT") == 0) {
        return MOV_OUT;
    }
    else if (strcmp(instruccion->instruccion, "F_OPEN") == 0) {
        return F_OPEN;
    }
    else if (strcmp(instruccion->instruccion, "F_CLOSE") == 0) {
        return F_CLOSE;
    }
    else if (strcmp(instruccion->instruccion, "F_SEEK") == 0) {
        return F_SEEK;
    }
    else if (strcmp(instruccion->instruccion, "F_TRUNCATE") == 0) {
        return F_TRUNCATE;
    } else if (strcmp(instruccion->instruccion, "F_READ") == 0) {
        return EFERRID;
    } else if (strcmp(instruccion->instruccion, "F_WRITE") == 0) {
        return EFERRAIT;
    }
    else
    {
        log_error(logger_cpu, "La intrucción no es válida.");
        return EXIT;
    }
}

char* determinar_registro(char* str_registro, uint32_t* tam ) {
    if (strcmp(str_registro, "AX") == 0)
    {
        *tam = 4;
        return AX;
    }
    else if (strcmp(str_registro, "BX") == 0)
    {
        *tam = 4;
        return BX;
    }
    else if (strcmp(str_registro, "CX") == 0)
    {
        *tam = 4;
        return CX;
    }
    else if (strcmp(str_registro, "DX") == 0)
    {
        *tam = 4;
        return DX;
    }
    else if (strcmp(str_registro, "EAX") == 0)
    {
        *tam = 8;
        return EAX;
    }
    else if (strcmp(str_registro, "EBX") == 0)
    {
        *tam = 8;
        return EBX;
    }
    else if (strcmp(str_registro, "ECX") == 0)
    {
        *tam = 8;
        return ECX;
    }
    else if (strcmp(str_registro, "EDX") == 0)
    {
        *tam = 8;
        return EDX;
    }
    else if (strcmp(str_registro, "RAX") == 0)
    {
        *tam = 16;
        return RAX;
    }
    else if (strcmp(str_registro, "RBX") == 0)
    {
        *tam = 16;
        return RBX;
    }
    else if (strcmp(str_registro, "RCX") == 0)
    {
        *tam = 16;
        return RCX;
    }
    else if (strcmp(str_registro, "RDX") == 0)
    {
        *tam = 16;
        return RDX;
    }
}

int ejecutar_mov_in(char* registro, uint32_t direccion_logica, uint32_t tam_registro) {
    uint32_t direccion_fisica = mmu(direccion_logica, tam_registro);
    if (direccion_fisica == -1)
    {
        return -1;
    }

    
    
    void* buffer = malloc(sizeof(cod_op) + sizeof(uint32_t) *3);
    int despl = 0;
    cod_op cop = MEMORIA_MOV_IN;
    
    memcpy(buffer + despl, &cop, sizeof(cod_op));
    despl += sizeof(cod_op);
    memcpy(buffer + despl, &PID_RUNNING, sizeof(uint32_t));
    despl += sizeof(uint32_t);
    memcpy(buffer + despl, &direccion_fisica, sizeof(uint32_t));
    despl += sizeof(uint32_t);
    memcpy(buffer + despl, &tam_registro, sizeof(uint32_t));
    despl += sizeof(uint32_t);
    
    send(socket_memoria, buffer, sizeof(cod_op) + sizeof(uint32_t) * 3, NULL);
    free(buffer);
    
    char* valor = malloc(tam_registro);
    recv(socket_memoria, valor, tam_registro, NULL);
    strncpy(registro, valor, tam_registro);
    //imprimir_contexto_actual();

    char* valor_parseado = imprimir_cadena(valor, tam_registro);
    log_info(logger_cpu, "PID: %d - Acción: LEER - Segmento: %d - Dirección Física: %d - Valor: %s", PID_RUNNING, direccion_logica/  TAM_MAX_SEGMENTO, direccion_fisica, valor_parseado);
    free(valor);
    free(valor_parseado);
    return 0;
}

int ejecutar_mov_out(uint32_t direccion_logica, char* registro, uint32_t tam_registro) {
    //imprimir_contexto_actual();
    uint32_t tam_buffer = sizeof(cod_op) + sizeof(uint32_t) * 3 + tam_registro;
    // Prueba
    //char* cadena = imprimir_cadena(registro, tam_registro);

    uint32_t direccion_fisica = mmu(direccion_logica, tam_registro);
    if (direccion_fisica == -1)
    {
        return -1;
    }
    
    
    void* buffer = malloc(tam_buffer);
    int despl = 0;
    cod_op cop = MEMORIA_MOV_OUT;
    
    memcpy(buffer + despl, &cop, sizeof(cod_op));
    despl += sizeof(cod_op);
    memcpy(buffer + despl, &PID_RUNNING, sizeof(uint32_t));
    despl += sizeof(uint32_t);
    memcpy(buffer + despl, &direccion_fisica, sizeof(uint32_t));
    despl += sizeof(uint32_t);
    memcpy(buffer + despl, &tam_registro, sizeof(uint32_t));
    despl += sizeof(uint32_t);
    memcpy(buffer + despl, registro, tam_registro);
    despl += tam_registro;

    send(socket_memoria, buffer, tam_buffer, NULL);
    free(buffer);
    
    uint32_t mov_out_ok;
    recv(socket_memoria, &mov_out_ok, sizeof(uint32_t), NULL);

    char* valor_parseado = imprimir_cadena(registro, tam_registro);
    log_info(logger_cpu, "PID: %d - Acción: ESCRIBIR - Segmento: %d - Dirección Física: %d - Valor: %s", PID_RUNNING, direccion_logica/  TAM_MAX_SEGMENTO, direccion_fisica, valor_parseado);
    free(valor_parseado);


    return 0;
    
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
        case MOV_IN:
            int tam_registro_in;
            char* registro_in = determinar_registro(instruccion->arg1, &tam_registro_in);
            uint32_t dir_logica_in = atoi(instruccion->arg2);
            int result = ejecutar_mov_in(registro_in, dir_logica_in,tam_registro_in);
            if (result == -1)
            {
                return CPU_SEG_FAULT;
            }
            break;
        case MOV_OUT:
            uint32_t tam_registro_out;
            char* registro_out = determinar_registro(instruccion->arg2, &tam_registro_out);
            uint32_t dir_logica_out = atoi(instruccion->arg1);
            int result_mov_out = ejecutar_mov_out(dir_logica_out, registro_out, tam_registro_out);
            if (result == -1)
            {
                return CPU_SEG_FAULT;
            }
            break;
        case YIELD:
            return CPU_YIELD;
        case EXIT:
            return CPU_EXIT;
        case IO:
            return CPU_IO;
        case WAIT:
            return CPU_WAIT;
        case SIGNAL:
            return CPU_SIGNAL;
        case CREATE_SEGMENT:
            return CPU_CREATE_SEGMENT;
        case DELETE_SEGMENT:
            return CPU_DELETE_SEGMENT;
        case F_OPEN:
            return CPU_F_OPEN;
        case F_CLOSE:
            return CPU_F_CLOSE;
        case F_SEEK:
            return CPU_F_SEEK;
        case F_TRUNCATE:
            return CPU_F_TRUNCATE;
        case EFERRID:
            return CPU_EFERRID;
        case EFERRAIT:
            return CPU_EFERRAIT;
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
