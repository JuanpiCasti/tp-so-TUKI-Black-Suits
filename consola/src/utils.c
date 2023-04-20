#include "utils.h"

FILE* leer_archivo(char* nombre_archivo, t_log* logger) {
    FILE* archivo_instrucciones = fopen(nombre_archivo, "r");
    if (archivo_instrucciones == NULL) {
        log_error(logger, "Error al abrir el archivo\n");
        exit(-1);
    }
    
    return archivo_instrucciones;
}

t_list* parsear_archivo(char* nombre_archivo, t_log* logger) {
    FILE* archivo_a_parsear = leer_archivo(nombre_archivo, logger);
    t_list* instrucciones = list_create();
    char *linea = malloc(sizeof(t_instruccion));

    t_instruccion* instruccion = malloc(sizeof(t_instruccion));

    while (fgets(linea, sizeof(t_instruccion), archivo_a_parsear))
    {
        if (sscanf(linea, "%s %s %s %s", instruccion->instruccion, instruccion->arg1, instruccion->arg2, instruccion->arg3) == 4) {
            // Instrucción con 3 parámetros
        } else if (sscanf(linea, "%s %s %s", instruccion->instruccion, instruccion->arg1, instruccion->arg2) == 3) {
            // Instrucción con 2 parámetros
        } else if (sscanf(linea, "%s %s", instruccion->instruccion, instruccion->arg1) == 2) {
            // Instrucción con 1 parámetro
        } else if (sscanf(linea, "%s", instruccion->instruccion) == 1) {
            // Instrucción sin parámetros
        }
        
        list_add(instrucciones, (void *)instruccion);
        instruccion = malloc(sizeof(t_instruccion));
    }

    fclose(archivo_a_parsear);

    return instrucciones;
}

void instruccion_destruir(void* instruccion) {
    free((t_instruccion*) instruccion);
}

t_paquete* crear_paquete_instrucciones() {
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->cop = PAQUETE_INSTRUCCIONES;
    paquete->size = 0;
    paquete->stream = NULL;
    return paquete;
}

void* serializar_paquete_instrucciones(t_paquete* paquete, t_list* instrucciones)
{
    int cant_instrucciones = list_size(instrucciones);
    int tam_buffer_instrucciones = cant_instrucciones * sizeof(t_instruccion);

    uint32_t size_paquete = tam_buffer_instrucciones + sizeof(uint32_t) + sizeof(cod_op);

    paquete->size = size_paquete;

    paquete->stream = malloc(size_paquete);
    memcpy(paquete->stream, &paquete->cop, sizeof(cod_op));
    memcpy(paquete->stream + sizeof(cod_op), &size_paquete, sizeof(uint32_t));

    void* buffer_instrucciones = malloc(tam_buffer_instrucciones);
    int desplazamiento = 0;

    for (int i = 0; i < cant_instrucciones; i++) {
        t_instruccion* instruccion = list_get(instrucciones, i);
        memcpy(buffer_instrucciones + desplazamiento, instruccion, sizeof(t_instruccion));
        desplazamiento += sizeof(t_instruccion);
    }

    memcpy(paquete->stream + sizeof(cod_op) + sizeof(uint32_t), buffer_instrucciones, tam_buffer_instrucciones);

    return paquete->stream;
}


t_list* deserializar_instrucciones(void* stream) {
    int cop;
    memcpy(&cop, stream, sizeof(cod_op));
    uint32_t tam_instrucciones;
    memcpy(&tam_instrucciones, stream + sizeof(cod_op), sizeof(uint32_t));
    int cant_instrucciones = tam_instrucciones / sizeof(t_instruccion);

    t_list* lista_instrucciones = list_create();

    int desplazamiento = 0;
    for (int i = 0; i < cant_instrucciones; i++)
    {
        t_instruccion* instruccion = malloc(sizeof(t_instruccion));
        memcpy(instruccion->instruccion, stream + sizeof(cod_op) + sizeof(uint32_t) + desplazamiento* sizeof(t_instruccion), sizeof(char [20]));
        memcpy(instruccion->instruccion, stream + sizeof(cod_op) + sizeof(uint32_t) + desplazamiento* sizeof(t_instruccion), sizeof(char [20]));
        memcpy(instruccion->instruccion, stream + sizeof(cod_op) + sizeof(uint32_t) + desplazamiento* sizeof(t_instruccion), sizeof(char [20]));
        list_add(lista_instrucciones, instruccion);
    }

    return lista_instrucciones;
    
    
}