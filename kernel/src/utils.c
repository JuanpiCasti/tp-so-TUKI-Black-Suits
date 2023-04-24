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
        memcpy(&instruccion->arg2, stream + desplazamiento + sizeof(char[20])*2, sizeof(char[20]));
        memcpy(&instruccion->arg3, stream + desplazamiento + sizeof(char[20])*3, sizeof(char[20]));
        list_add(lista_instrucciones, instruccion);
        desplazamiento += sizeof(t_instruccion);
    }

    return lista_instrucciones;
}