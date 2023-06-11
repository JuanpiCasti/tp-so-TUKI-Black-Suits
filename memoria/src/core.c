#include "core.h"

void* crear_tabla_segmentos() {
    void* buffer = malloc(sizeof(t_ent_ts) * CANT_SEGMENTOS);
    uint32_t despl = 0;
    uint32_t cero = 0;
    uint32_t i = 0;
    memcpy(buffer,&i, despl+=sizeof(uint32_t)); // ID
    memcpy(buffer,&cero, despl+=sizeof(uint32_t)); // BASE
    memcpy(buffer,&TAM_SEGMENTO_0, despl+=sizeof(uint32_t)); // LIMITE


    for (; i < CANT_SEGMENTOS; i++)
    {
        memcpy(buffer,&i, despl+=sizeof(uint32_t));
        memcpy(buffer,&cero, despl+=sizeof(uint32_t)); // BASE
        memcpy(buffer,&cero, despl+=sizeof(uint32_t)); // LIMITE
    }
    
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
        return NULL;
   
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
        return NULL;

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
        return NULL;

        break;
    
    default:
        log_error(logger_memoria_extra,"ALGO BUSQUEDA ESPACIO DESCONOCIDO");
    }
    
    return -1;
}

void crear_segmento(uint32_t tam) { // TODO: que retorne un codop que entienda el kernel
    if (ESPACIO_LIBRE_TOTAL < tam)
    {
        log_info(logger_memoria_extra, "NO HAY ESPACIO SUFICIENTE PARA CREAR ESE SEGMENTO");
        return;
        // Retornar codop indicando que no hay espacio suficiente.
    }
    
    
    int i_espacio = buscar_espacio_libre(tam);

    if (i_espacio == -1)
    {
        // Retornar codop indicando que es necesario compactar.
        return;
    }

    t_esp* espacio = list_get(LISTA_ESPACIOS_LIBRES, i_espacio);

    espacio->base += tam;
    espacio->limite -= tam;
    ESPACIO_LIBRE_TOTAL -= tam;

    if (espacio->limite == 0)
    {
        list_remove(LISTA_ESPACIOS_LIBRES, i_espacio);
        free(espacio);
    }
    
}

bool son_contiguos(t_esp* esp1, t_esp* esp2) {
    return esp1 ->base + esp1->limite == esp2 ->base;
}


void borrar_segmento(uint32_t base, uint32_t limite) {

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
    

   
    
    
}

void mov_out(uint32_t dir_fisca, void* data, uint32_t size) {
    memcpy(ESPACIO_USUARIO + dir_fisca, data, size);
}

void* mov_in(uint32_t dir_fisca , uint32_t size) {
    void* data = malloc(size);
    memcpy(data, ESPACIO_USUARIO + dir_fisca, size);
    return data;
}