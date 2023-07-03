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
    ESPACIO_LIBRE_TOTAL = TAM_MEMORIA;
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
    ESPACIO_LIBRE_TOTAL;

    LISTA_ESPACIOS_LIBRES = list_create();
    LISTA_GLOBAL_SEGMENTOS = list_create();

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

bool comparador_base_segmento(void* data1, void* data2) {
    t_segmento* seg1 = (t_segmento*)data1;
    t_segmento* seg2 = (t_segmento*)data2;

    return seg1->base < seg2->base;
}

void print_lista_esp(t_list* lista) {
    printf("Lista de espacios libres:\n");
    for (int i = 0; i < list_size(lista); i++) {
        t_esp* elemento = list_get(lista, i);
        printf("Elemento %d: base=%u, limite=%u\n", i+1, elemento->base, elemento->limite);
    }
}

void print_lista_segmentos() {
    printf("Lista de segmentos:\n");
    for (int i = 0; i < list_size(LISTA_GLOBAL_SEGMENTOS); i++) {
        t_segmento* elemento = list_get(LISTA_GLOBAL_SEGMENTOS, i);
        printf("PID %u: ID=%u, BASE=%u, LIMITE=%u\n", elemento->pid, elemento->id, elemento->base, elemento->limite);
    }
}

#include "core.h"

void* crear_tabla_segmentos() {
    void* buffer = malloc(sizeof(t_ent_ts) * CANT_SEGMENTOS);
    uint32_t despl = 0;
    uint32_t cero = 0;
    uint32_t i = 0;
    uint8_t estado_inicial = 1;

    memcpy(buffer + despl,&i, sizeof(uint32_t)); // ID
    despl+=sizeof(uint32_t);
    i++;

    memcpy(buffer + despl,&cero, sizeof(uint32_t)); // BASE
    despl+=sizeof(uint32_t);

    memcpy(buffer + despl,&TAM_SEGMENTO_0, sizeof(uint32_t)); // LIMITE
    despl+=sizeof(uint32_t);

    memcpy(buffer + despl, &estado_inicial, sizeof(uint8_t));
    despl+=sizeof(uint8_t);

    estado_inicial = 0;

    for (; i < CANT_SEGMENTOS; i++)
    {
        memcpy(buffer + despl,&i, sizeof(uint32_t)); // ID
        despl+=sizeof(uint32_t);


        memcpy(buffer + despl,&cero, sizeof(uint32_t)); // BASE
        despl+=sizeof(uint32_t);

        memcpy(buffer + despl,&cero, sizeof(uint32_t)); // LIMITE
        despl+=sizeof(uint32_t);

        memcpy(buffer + despl, &estado_inicial, sizeof(uint8_t));
        despl+=sizeof(uint8_t);
    }
    
    return buffer;
}

int buscar_espacio_libre(uint32_t tam) {
    t_esp* esp;
    t_esp* esp_i;
    int i;
    switch (ALGORITMO_ASIGNACION)
    {
    case FIRST:
        esp;
        for (i = 0; i < list_size(LISTA_ESPACIOS_LIBRES); i++)
        {   
            esp = list_get(LISTA_ESPACIOS_LIBRES, i);
            if (esp->limite >= tam)
            {
                return i;
            }
        }

        log_info(logger_memoria_extra, "NO SE ENCONTRO UN ESPACIO LIBRE, SE NECESITA COMPACTAR");
        return -1;
   
        break;
    
    case BEST:
        esp = list_get(LISTA_ESPACIOS_LIBRES, 0);
        esp_i;
        for (i = 1; i < list_size(LISTA_ESPACIOS_LIBRES); i++)
        {   
            esp_i = list_get(LISTA_ESPACIOS_LIBRES, i);
            if (esp_i->limite > esp->limite)
            {
                esp = esp_i;
            }
        }

        if (esp->limite >= tam)
        {
            return i;
        } 

        log_info(logger_memoria_extra, "NO SE ENCONTRO UN ESPACIO LIBRE, SE NECESITA COMPACTAR");
        return -1;

        break;
    
    case WORST:
        esp = list_get(LISTA_ESPACIOS_LIBRES, 0);
        esp_i;
        for (i = 1; i < list_size(LISTA_ESPACIOS_LIBRES); i++)
        {   
            esp_i = list_get(LISTA_ESPACIOS_LIBRES, i);
            if (esp_i->limite < esp->limite && esp_i->limite >= tam)
            {
                esp = esp_i;
            }
        }

        if (esp->limite >= tam)
        {
            return i;
        } 

        log_info(logger_memoria_extra, "NO SE ENCONTRO UN ESPACIO LIBRE, SE NECESITA COMPACTAR");
        return -1;

        break;
    
    default:
        log_error(logger_memoria_extra,"ALGO BUSQUEDA ESPACIO DESCONOCIDO");
    }
    
    return -1;
}

cod_op_kernel crear_segmento(uint32_t tam, uint32_t* base_resultante) {
    //printf("%d, %d\n", ESPACIO_LIBRE_TOTAL, tam);
    if (ESPACIO_LIBRE_TOTAL < tam)
    {
        log_info(logger_memoria_extra, "NO HAY ESPACIO SUFICIENTE PARA CREAR ESE SEGMENTO");
        return EXIT_OUT_OF_MEMORY;
        // Retornar codop indicando que no hay espacio suficiente.
    }
    
    
    int i_espacio = buscar_espacio_libre(tam);

    if (i_espacio == -1)
    {
        // Retornar codop indicando que es necesario compactar.
        return MEMORIA_NECESITA_COMPACTACION;
    }

    t_esp* espacio = list_get(LISTA_ESPACIOS_LIBRES, i_espacio);
    memcpy(base_resultante, &espacio->base, sizeof(uint32_t));

    espacio->base += tam;
    espacio->limite -= tam;
    ESPACIO_LIBRE_TOTAL -= tam;

    if (espacio->limite == 0)
    {
        list_remove(LISTA_ESPACIOS_LIBRES, i_espacio);
        free(espacio);
    }

    return MEMORIA_SEGMENTO_CREADO;
    
}

bool son_contiguos(t_esp* esp1, t_esp* esp2) {
    return esp1 ->base + esp1->limite == esp2 ->base;
}

int buscar_segmento_por_base(uint32_t base) {
    t_segmento* segmento;
    for (int i = 0; i < list_size(LISTA_GLOBAL_SEGMENTOS); i++)
    {
        segmento = list_get(LISTA_GLOBAL_SEGMENTOS, i);
        if (segmento->base == base)
        {
            return i;
        }
    }
    return -1;
}


void borrar_segmento(uint32_t base, uint32_t limite) {
    ESPACIO_LIBRE_TOTAL += limite;

    t_esp* nuevo_esp = malloc(sizeof(t_esp));
    nuevo_esp->base = base;
    nuevo_esp->limite = limite;
    int n_indice = list_add_sorted(LISTA_ESPACIOS_LIBRES, nuevo_esp, comparador_base);
    
    //consolidacion

    if (list_size(LISTA_ESPACIOS_LIBRES) > 1)
    {
         t_esp* posible_espacio_contiguo_abajo = list_get(LISTA_ESPACIOS_LIBRES, n_indice + 1);

        if (son_contiguos(nuevo_esp, posible_espacio_contiguo_abajo))
        {
            nuevo_esp -> limite += posible_espacio_contiguo_abajo->limite;
            list_remove(LISTA_ESPACIOS_LIBRES, n_indice + 1);
            free(posible_espacio_contiguo_abajo);
        }

        if (n_indice > 0)
        {
            t_esp* posible_espacio_contiguo_arriba = list_get(LISTA_ESPACIOS_LIBRES, n_indice - 1);

            if (son_contiguos(posible_espacio_contiguo_arriba, nuevo_esp))
            {
                posible_espacio_contiguo_arriba -> limite += nuevo_esp->limite;
                list_remove(LISTA_ESPACIOS_LIBRES, n_indice);
                free(nuevo_esp);
            }
        }
    }
    
    int indice_segmento = buscar_segmento_por_base(base);

    t_segmento* segmento = list_remove(LISTA_GLOBAL_SEGMENTOS,indice_segmento );
    free(segmento);
    
}

void escribir(uint32_t dir_fisca, void* data, uint32_t size) {
    memcpy(ESPACIO_USUARIO + dir_fisca, data, size);
}

char* leer(uint32_t dir_fisca , uint32_t size) {
    void* data = malloc(size);
    memcpy(data, ESPACIO_USUARIO + dir_fisca, size);
    return data;
}

void compactar() {
    for (int i = 0; i < list_size(LISTA_GLOBAL_SEGMENTOS); i++)
    {
        t_segmento* segmento = list_get(LISTA_GLOBAL_SEGMENTOS, i);
        t_esp* primer_espacio_libre = list_get(LISTA_ESPACIOS_LIBRES, 0);
        if (segmento->base > primer_espacio_libre->base )
        {
            memcpy(ESPACIO_USUARIO + primer_espacio_libre->base, ESPACIO_USUARIO + segmento->base, segmento->limite);
            segmento->base = primer_espacio_libre->base;
            primer_espacio_libre->base += segmento->limite;
            
            //consolidacion
            if (list_size(LISTA_ESPACIOS_LIBRES) > 1)
            {
                t_esp* posible_espacio_contiguo_abajo = list_get(LISTA_ESPACIOS_LIBRES, 1);

                if (son_contiguos(primer_espacio_libre, posible_espacio_contiguo_abajo))
                {
                    primer_espacio_libre -> limite += posible_espacio_contiguo_abajo->limite;
                    list_remove(LISTA_ESPACIOS_LIBRES, 1);
                    free(posible_espacio_contiguo_abajo);
                }
            }
        }
        
    }

    sleep(RETARDO_COMPACTACION/1000);
}