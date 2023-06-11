#include "utils.h"

char* t_algo_asig_desc[] = {"FIRST", "BEST", "WORST"};

void levantar_loggers_memoria() {
    logger_memoria = log_create("./log/memoria.log", "MEMORIA", true, LOG_LEVEL_INFO);
    logger_memoria_extra = log_create("./log/memoria_extra.log", "MEMORIA", true, LOG_LEVEL_INFO);
}

void levantar_config_memoria() {
    config_memoria = config_create("./cfg/memoria.config");
    PUERTO_ESCUCHA_MEMORIA = config_get_int_value(config_memoria, "PUERTO_ESCUCHA");
    TAM_MEMORIA = config_get_int_value(config_memoria, "TAM_MEMORIA");
    TAM_SEGMENTO_0 = config_get_int_value(config_memoria, "TAM_SEGMENTO_0");
    CANT_SEGMENTOS = config_get_int_value(config_memoria, "CANT_SEGMENTOS");
    RETARDO_MEMORIA = config_get_int_value(config_memoria, "RETARDO_MEMORIA");
    RETARDO_COMPACTACION = config_get_int_value(config_memoria, "RETARDO_COMPACTACION");
    char * algo_asig = config_get_string_value(config_memoria, "ALGORITMO_ASIGNACION");

    if (strcmp(algo_asig, "FIRST") == 0)
    {
        ALGORITMO_ASIGNACION = FIRST;
    } else if (strcmp(algo_asig, "BEST") == 0)
    {
        ALGORITMO_ASIGNACION = BEST;
    } else if (strcmp(algo_asig, "WORST") == 0)
    {
        ALGORITMO_ASIGNACION = WORST;
    } else {
        log_error(logger_memoria_extra, "ALGORITMO DE ASIGNACION DESCONOCIDO");
    }

    config_destroy(config_memoria);
}



void crear_segmento_0() {
    t_esp* espacio = list_get(LISTA_ESPACIOS_LIBRES, 0);
    espacio->base += TAM_SEGMENTO_0;
    espacio->limite -= TAM_SEGMENTO_0;
    ESPACIO_LIBRE_TOTAL -= TAM_SEGMENTO_0;
}

void levantar_estructuras_administrativas() {
    ESPACIO_USUARIO = malloc(TAM_MEMORIA);
    ESPACIO_LIBRE_TOTAL = 0;

    LISTA_ESPACIOS_LIBRES = list_create();

    t_esp* espacio_inicial = malloc(sizeof(t_esp));
    espacio_inicial->base = 0;
    espacio_inicial->limite = TAM_MEMORIA;

    list_add(LISTA_ESPACIOS_LIBRES, espacio_inicial);

    crear_segmento_0();
}

bool comparador_base(void* data1, void* data2) {
    t_esp* esp1 = (t_esp*)data1;
    t_esp* esp2 = (t_esp*)data2;

    return esp1->base < esp2->base;
}

void print_lista_esp(t_list* lista) {
    printf("Lista de espacios libres:\n");
    for (int i = 0; i < list_size(lista); i++) {
        t_esp* elemento = list_get(lista, i);
        printf("Elemento %d: base=%u, limite=%u\n", i+1, elemento->base, elemento->limite);
    }
}